#pragma once
#include <QAbstractButton>
#include <QColor>
#include <QString>

class QPropertyAnimation;
class QVariantAnimation;

// Flat, rounded button with a hover tint and a clipped ripple that expands
// from the click point. Owns no layout-affecting state: painting only.
class RippleButton : public QAbstractButton {
    Q_OBJECT
    Q_PROPERTY(qreal rippleRadius READ rippleRadius WRITE setRippleRadius)
    Q_PROPERTY(qreal rippleOpacity READ rippleOpacity WRITE setRippleOpacity)
public:
    explicit RippleButton(const QString& text = QString(), QWidget* parent = nullptr);

    void setIconName(const QString& name);
    QString iconName() const { return iconName_; }
    void setIconSize(int px);
    void setPill(bool on);                     // full-radius vs 8px corner
    void setCornerRadius(int r);
    void setColors(const QColor& base, const QColor& hover, const QColor& text);
    void setAccent(const QColor& accent);      // used while checked
    void setCompact(bool on);                  // icon-only: no text, square

    qreal rippleRadius() const { return rippleRadius_; }
    void setRippleRadius(qreal r);
    qreal rippleOpacity() const { return rippleOpacity_; }
    void setRippleOpacity(qreal o);

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void enterEvent(QEvent*) override;
    void leaveEvent(QEvent*) override;
    void resizeEvent(QResizeEvent*) override;

private:
    void animateHover(qreal target);

    QString iconName_;
    int iconPx_ = 20;
    bool pill_ = false;
    bool compact_ = false;
    int radius_ = 8;
    QColor base_{0, 0, 0, 0};
    QColor hover_{255, 255, 255, 18};
    QColor text_{0xF1, 0xF1, 0xF1};
    QColor accent_{0xFF, 0, 0};
    QColor rippleColor_{255, 255, 255};
    qreal hover_ = 0.0;
    qreal rippleRadius_ = 0.0;
    qreal rippleOpacity_ = 0.0;
    QPointF rippleCentre_;
    QVariantAnimation* hoverAnim_ = nullptr;
    QPropertyAnimation* rippleRadiusAnim_ = nullptr;
    QPropertyAnimation* rippleOpacityAnim_ = nullptr;
};
