#include "PageTransition.h"
#include "Motion.h"
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QStackedWidget>

namespace {
bool g_running = false;
QParallelAnimationGroup* g_group = nullptr;
}

bool PageTransition::running() { return g_running; }

void PageTransition::slideTo(QStackedWidget* stack, int index, int durationMs) {
    if (!stack || index < 0 || index >= stack->count()) return;
    QWidget* to = stack->widget(index);
    QWidget* from = stack->currentWidget();
    if (!to || to == from) {
        stack->setCurrentIndex(index);
        return;
    }
    const int ms = Motion::ms(durationMs);
    if (ms <= 0 || g_running) {
        stack->setCurrentIndex(index);
        return;
    }

    const int dx = stack->width();
    stack->setCurrentIndex(index);      // 'to' is now current and laid out
    to->move(dx, 0);
    to->show();
    from->move(0, 0);
    from->show();
    from->raise();
    to->raise();

    g_running = true;
    g_group = new QParallelAnimationGroup(stack);
    auto* a1 = new QPropertyAnimation(to, "pos", g_group);
    a1->setDuration(ms);
    a1->setEasingCurve(Motion::outCubic());
    a1->setStartValue(QPoint(dx, 0));
    a1->setEndValue(QPoint(0, 0));
    auto* a2 = new QPropertyAnimation(from, "pos", g_group);
    a2->setDuration(ms);
    a2->setEasingCurve(Motion::outCubic());
    a2->setStartValue(QPoint(0, 0));
    a2->setEndValue(QPoint(-dx, 0));
    g_group->addAnimation(a1);
    g_group->addAnimation(a2);
    QObject::connect(g_group, &QParallelAnimationGroup::finished, stack, [stack, from, to] {
        from->move(0, 0);
        from->hide();
        to->move(0, 0);
        g_running = false;
        if (g_group) { g_group->deleteLater(); g_group = nullptr; }
    });
    g_group->start();
}
