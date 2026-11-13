#include "RippleButton.h"
#include "IconRenderer.h"
#include "Motion.h"
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QVariantAnimation>

RippleButton::RippleButton(const QString& text, QWidget* parent) : QAbstractButton(parent) {
    setText(text);
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    hoverAnim_ = new QVariantAnimation(this);
    hoverAnim_->setDuration(Motion::ms(Motion::Fast));
    hoverAnim_->setEasingCurve(Motion::outCubic());
    connect(hoverAnim_, &QVariantAnimation::valueChanged, this, [this](const QVariant& v) {
        hover_ = v.toReal();
        update();
    });
    rippleRadiusAnim_ = new QPropertyAnimation(this, "rippleRadius", this);
    rippleRadiusAnim_->setEasingCurve(QEasingCurve(QEasingCurve::OutQuad));
    rippleOpacityAnim_ = new QPropertyAnimation(this, "rippleOpacity", this);
    rippleOpacityAnim_->setEasingCurve(Motion::outCubic());
}

void RippleButton::setIconName(const QString& name) { iconName_ = name; update(); }
void RippleButton::setIconSize(int px) { iconPx_ = qMax(8, px); updateGeometry(); update(); }
void RippleButton::setPill(bool on) { pill_ = on; radius_ = on ? height() / 2 : radius_; update(); }
void RippleButton::setCornerRadius(int r) { radius_ = qMax(0, r); update(); }
void RippleButton::setColors(const QColor& base, const QColor& hover, const QColor& text) {
    base_ = base; hover_ = hover; text_ = text; update();
}
void RippleButton::setAccent(const QColor& accent) { accent_ = accent; update(); }
void RippleButton::setCompact(bool on) { compact_ = on; updateGeometry(); update(); }

void RippleButton::setRippleRadius(qreal r) { rippleRadius_ = r; update(); }
void RippleButton::setRippleOpacity(qreal o) { rippleOpacity_ = o; update(); }

QSize RippleButton::sizeHint() const {
    if (compact_) return QSize(iconPx_ + 16, iconPx_ + 16);
    QFontMetrics fm(font());
    const int textW = text().isEmpty() ? 0 : fm.horizontalAdvance(text());
    const int iconW = iconName_.isEmpty() ? 0 : iconPx_ + (textW ? 8 : 0);
    return QSize(textW + iconW + 24, qMax(iconPx_ + 12, fm.height() + 12));
}

void RippleButton::animateHover(qreal target) {
    if (!Motion::enabled()) { hover_ = target; update(); return; }
    hoverAnim_->stop();
    hoverAnim_->setStartValue(hover_);
    hoverAnim_->setEndValue(target);
    hoverAnim_->start();
}

void RippleButton::enterEvent(QEvent*) { animateHover(1.0); }
void RippleButton::leaveEvent(QEvent*) { animateHover(0.0); }
void RippleButton::resizeEvent(QResizeEvent* e) {
    QAbstractButton::resizeEvent(e);
    if (pill_) radius_ = height() / 2;
}

void RippleButton::mousePressEvent(QMouseEvent* e) {
    rippleCentre_ = e->pos();
    const qreal maxR = qSqrt(qreal(width()) * width() + qreal(height()) * height()) * 1.15;
    rippleRadius_ = 0.0;
    rippleOpacity_ = 0.30;
    if (Motion::enabled()) {
        rippleRadiusAnim_->stop();
        rippleRadiusAnim_->setDuration(Motion::Ripple);
        rippleRadiusAnim_->setStartValue(0.0);
        rippleRadiusAnim_->setEndValue(maxR);
        rippleRadiusAnim_->start();
        rippleOpacityAnim_->stop();
        rippleOpacityAnim_->setDuration(Motion::Ripple);
        rippleOpacityAnim_->setStartValue(0.30);
        rippleOpacityAnim_->setEndValue(0.0);
        rippleOpacityAnim_->start();
    } else {
        rippleOpacity_ = 0.0;
    }
    QAbstractButton::mousePressEvent(e);
}

void RippleButton::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    const QRectF r = rect().adjusted(0.5, 0.5, -0.5, -0.5);
    const qreal rad = pill_ ? qMin(r.width(), r.height()) / 2.0 : radius_;

    QColor bg = base_;
    if (hover_ > 0.001) {
        const qreal a = hover_;
        bg = QColor::fromRgbF(base_.redF() + (hover_.redF() - base_.redF()) * a,
                              base_.greenF() + (hover_.greenF() - base_.greenF()) * a,
                              base_.blueF() + (hover_.blueF() - base_.blueF()) * a,
                              base_.alphaF() + (hover_.alphaF() - base_.alphaF()) * a);
    }
    if (isChecked()) {
        bg = QColor(accent_.red(), accent_.green(), accent_.blue(), 46);
    }
    if (bg.alpha() > 0) {
        p.setPen(Qt::NoPen);
        p.setBrush(bg);
        p.drawRoundedRect(r, rad, rad);
    }
    if (hasFocus()) {
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(QColor(255, 255, 255, 46), 1.0));
        p.drawRoundedRect(r, rad, rad);
    }

    if (rippleOpacity_ > 0.001) {
        QPainterPath clip;
        clip.addRoundedRect(r, rad, rad);
        p.save();
        p.setClipPath(clip);
        QColor rc = rippleColor_;
        rc.setAlphaF(qBound(0.0, rippleOpacity_, 1.0));
        p.setPen(Qt::NoPen);
        p.setBrush(rc);
        p.drawEllipse(rippleCentre_, rippleRadius_, rippleRadius_);
        p.restore();
    }

    QColor fg = text_;
    if (!isEnabled()) fg.setAlpha(110);
    else if (isChecked()) fg = accent_.lightness() > 200 ? text_ : QColor(0xF1, 0xF1, 0xF1);

    const int textW = compact_ ? 0 : (text().isEmpty() ? 0 : QFontMetrics(font()).horizontalAdvance(text()));
    const int iconW = iconName_.isEmpty() ? 0 : iconPx_;
    const int gap = (textW && iconW) ? 8 : 0;
    const int contentW = textW + iconW + gap;
    const qreal x0 = (width() - contentW) / 2.0;
    if (iconW) {
        Icons::draw(p, iconName_, QRectF(x0, (height() - iconPx_) / 2.0, iconPx_, iconPx_), fg);
    }
    if (textW) {
        p.setPen(fg);
        p.drawText(QRectF(x0 + iconW + gap, 0, textW, height()), Qt::AlignVCenter | Qt::AlignLeft, text());
    }
}
