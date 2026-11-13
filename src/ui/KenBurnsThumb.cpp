#include "KenBurnsThumb.h"
#include "Motion.h"
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>

KenBurnsThumb::KenBurnsThumb(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    anim_ = new QPropertyAnimation(this, "zoom", this);
    anim_->setEasingCurve(Motion::outCubic());
}

void KenBurnsThumb::setImage(const QImage& image) {
    pixmap_ = image.isNull() ? QPixmap() : QPixmap::fromImage(image);
    update();
}
void KenBurnsThumb::setPixmap(const QPixmap& pixmap) {
    pixmap_ = pixmap;
    update();
}
void KenBurnsThumb::setPlaceholder(const QColor& colour) {
    placeholder_ = colour;
    update();
}
void KenBurnsThumb::setZoom(qreal z) {
    zoom_ = z;
    update();
}

void KenBurnsThumb::animateTo(qreal target) {
    if (!zoomEnabled_ || !Motion::enabled()) {
        setZoom(target);
        return;
    }
    anim_->stop();
    anim_->setDuration(Motion::KenBurns);
    anim_->setStartValue(zoom_);
    anim_->setEndValue(target);
    anim_->start();
}

void KenBurnsThumb::enterEvent(QEvent*) { animateTo(1.06); }
void KenBurnsThumb::leaveEvent(QEvent*) { animateTo(1.0); }

void KenBurnsThumb::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);   // never HighQualityAntialiasing here
    QPainterPath clip;
    clip.addRoundedRect(QRectF(rect()), radius_, radius_);
    p.setClipPath(clip);
    p.fillRect(rect(), placeholder_);
    if (pixmap_.isNull()) return;

    // Cover-fit, then scale by zoom about the centre of the widget.
    const qreal sw = qreal(width()) / pixmap_.width();
    const qreal sh = qreal(height()) / pixmap_.height();
    const qreal s = qMax(sw, sh) * zoom_;
    const QSizeF target(pixmap_.width() * s, pixmap_.height() * s);
    const QRectF dest((width() - target.width()) / 2.0, (height() - target.height()) / 2.0,
                      target.width(), target.height());
    p.drawPixmap(dest, pixmap_, QRectF(pixmap_.rect()));
}
