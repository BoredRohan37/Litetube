#include "SkeletonCard.h"
#include "Motion.h"
#include <QLinearGradient>
#include <QPainter>
#include <QPainterPath>
#include <QVariantAnimation>

namespace {
int g_active = 0;
}

SkeletonCard::SkeletonCard(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    anim_ = new QVariantAnimation(this);
    anim_->setStartValue(0.0);
    anim_->setEndValue(1.0);
    anim_->setDuration(1300);
    anim_->setLoopCount(-1);
    anim_->setEasingCurve(QEasingCurve::Linear);
    connect(anim_, &QVariantAnimation::valueChanged, this, [this](const QVariant& v) {
        shimmer_ = v.toReal();
        update();
    });
}

SkeletonCard::~SkeletonCard() {
    if (shimmering_) { shimmering_ = false; --g_active; }
}

int SkeletonCard::activeCount() { return g_active; }

void SkeletonCard::setShimmer(qreal v) { shimmer_ = v; update(); }

void SkeletonCard::showEvent(QShowEvent* e) {
    QWidget::showEvent(e);
    if (Motion::enabled() && g_active < kShimmerCap) {
        shimmering_ = true;
        ++g_active;
        anim_->start();
    }
}
void SkeletonCard::hideEvent(QHideEvent* e) {
    QWidget::hideEvent(e);
    if (shimmering_) {
        shimmering_ = false;
        --g_active;
        anim_->stop();
    }
}

void SkeletonCard::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    const QColor bone(0x27, 0x27, 0x27);
    const QColor line(0x32, 0x32, 0x32);

    QRectF thumb;
    QRectF t1, t2, t3;
    if (rows_) {
        thumb = QRectF(0, 0, qMin(168.0, width() * 0.42), height() * 0.82);
        const qreal x = thumb.width() + 12;
        t1 = QRectF(x, 6, width() - x - 6, 14);
        t2 = QRectF(x, 28, (width() - x - 6) * 0.62, 12);
        t3 = QRectF(x, 46, (width() - x - 6) * 0.44, 12);
    } else {
        thumb = QRectF(0, 0, width(), width() * 9.0 / 16.0);
        t1 = QRectF(0, thumb.bottom() + 10, width() * 0.94, 14);
        t2 = QRectF(0, thumb.bottom() + 30, width() * 0.66, 12);
        t3 = QRectF(0, thumb.bottom() + 48, width() * 0.44, 12);
    }
    p.setPen(Qt::NoPen);
    p.setBrush(bone);
    p.drawRoundedRect(thumb, 10, 10);
    p.setBrush(line);
    p.drawRoundedRect(t1, 4, 4);
    p.drawRoundedRect(t2, 4, 4);
    p.drawRoundedRect(t3, 4, 4);

    if (!shimmering_) return;
    // Single moving highlight across the whole card, cheap to paint.
    const qreal bandW = qMax(80.0, width() * 0.55);
    const qreal x = -bandW + shimmer_ * (width() + bandW * 2);
    QLinearGradient g(x, 0, x + bandW, 0);
    g.setColorAt(0.0, QColor(255, 255, 255, 0));
    g.setColorAt(0.5, QColor(255, 255, 255, 16));
    g.setColorAt(1.0, QColor(255, 255, 255, 0));
    p.setBrush(g);
    QPainterPath clip;
    clip.addRoundedRect(thumb, 10, 10);
    p.setClipPath(clip);
    p.fillRect(thumb, g);
}
