#pragma once
#include <QImage>
#include <QPixmap>
#include <QWidget>

class QPropertyAnimation;

// Thumbnail that slowly zooms on hover (1.0 -> 1.06) and back on leave.
// The zoom is a custom qreal property, so nothing outside this widget is
// invalidated and no layout work happens while the mouse moves.
class KenBurnsThumb : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal zoom READ zoom WRITE setZoom)
public:
    explicit KenBurnsThumb(QWidget* parent = nullptr);

    void setImage(const QImage& image);
    void setPixmap(const QPixmap& pixmap);
    void setPlaceholder(const QColor& colour);
    void setCornerRadius(int r) { radius_ = r; update(); }
    void setZoomEnabled(bool on) { zoomEnabled_ = on; }
    bool hasImage() const { return !pixmap_.isNull(); }

    qreal zoom() const { return zoom_; }
    void setZoom(qreal z);

    QSize sizeHint() const override { return QSize(320, 180); }

protected:
    void paintEvent(QPaintEvent*) override;
    void enterEvent(QEvent*) override;
    void leaveEvent(QEvent*) override;

private:
    void animateTo(qreal target);

    QPixmap pixmap_;
    QColor placeholder_{0x27, 0x27, 0x27};
    qreal zoom_ = 1.0;
    int radius_ = 10;
    bool zoomEnabled_ = true;
    QPropertyAnimation* anim_ = nullptr;
};
