#include "AmbientGlow.h"
#include "Motion.h"
#include <QImage>
#include <QPainter>
#include <QRadialGradient>
#include <QVariantAnimation>

AmbientGlow::AmbientGlow(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    anim_ = new QVariantAnimation(this);
    anim_->setEasingCurve(Motion::inOutQuad());
    connect(anim_, &QVariantAnimation::valueChanged, this, [this](const QVariant& v) {
        glow_ = v.value<QColor>();
        update();
    });
}

void AmbientGlow::setGlow(const QColor& c) { glow_ = c; update(); }
void AmbientGlow::clear() { setGlow(QColor(0x1A, 0x1A, 0x1A)); }

void AmbientGlow::setSourceThumbnail(const QImage& image) {
    if (image.isNull()) { clear(); return; }
    const QImage small = image.scaled(24, 14, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    qint64 r = 0, g = 0, b = 0, n = 0;
    for (int y = 0; y < small.height(); ++y) {
        for (int x = 0; x < small.width(); ++x) {
            const QRgb px = small.pixel(x, y);
            r += qRed(px); g += qGreen(px); b += qBlue(px); ++n;
        }
    }
    if (!n) { clear(); return; }
    QColor avg(int(r / n), int(g / n), int(b / n));
    // Lift the colour so a dark thumbnail still reads as a glow.
    const int hue = qMax(0, avg.hue());
    const int sat = qBound(80, avg.saturation() + 40, 220);
    const int val = qBound(50, avg.value() + 30, 150);
    target_ = QColor::fromHsv(hue, sat, val);
    if (!animated_ || !Motion::enabled()) { setGlow(target_); return; }
    anim_->stop();
    anim_->setDuration(Motion::Ambient);
    anim_->setStartValue(glow_);
    anim_->setEndValue(target_);
    anim_->start();
}

void AmbientGlow::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    QRadialGradient grad(rect().center(), qMax(width(), height()) * 0.62);
    QColor inner = glow_;
    inner.setAlpha(150);
    QColor outer = glow_;
    outer.setAlpha(0);
    grad.setColorAt(0.0, inner);
    grad.setColorAt(1.0, outer);
    p.fillRect(rect(), grad);
}
