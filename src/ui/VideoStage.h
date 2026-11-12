#pragma once
#include <QWidget>
#include <QPixmap>
class QLabel; class QStackedLayout;

// Placeholder for the single, persistent video window. The video window floats above this widget and
// follows its geometry, so mpv never has to re-attach (re-creating the Direct3D device was crashing).
class VideoStage : public QWidget {
    Q_OBJECT
public:
    explicit VideoStage(QWidget* p = nullptr);
    void showMessage(const QString& text, const QPixmap& poster = QPixmap());
    void showVideo();
    bool wantsVideo() const { return wantVideo_; }
signals:
    void geometryChanged();
protected:
    void resizeEvent(QResizeEvent*) override { emit geometryChanged(); }
    void moveEvent(QMoveEvent*) override { emit geometryChanged(); }
    void showEvent(QShowEvent*) override { emit geometryChanged(); }
    void hideEvent(QHideEvent*) override { emit geometryChanged(); }
private:
    QLabel* msg_;
    bool wantVideo_ = false;
};
