#pragma once
#include <QColor>
#include <QImage>
#include <QWidget>

class QVariantAnimation;

// Soft colour wash behind the player, derived once from the video thumbnail
// (scaled to 24x14 and averaged). Live frames are never sampled.
class AmbientGlow : public QWidget {
    Q_OBJECT
    Q_PROPERTY(QColor glow READ glow WRITE setGlow)
public:
    explicit AmbientGlow(QWidget* parent = nullptr);

    void setSourceThumbnail(const QImage& image);
    void clear();
    QColor glow() const { return glow_; }
    void setGlow(const QColor& c);
    void setAnimated(bool on) { animated_ = on; }

protected:
    void paintEvent(QPaintEvent*) override;

private:
    QColor glow_{0x1A, 0x1A, 0x1A};
    QColor target_{0x1A, 0x1A, 0x1A};
    bool animated_ = true;
    QVariantAnimation* anim_ = nullptr;
};
