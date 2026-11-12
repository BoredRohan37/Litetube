#pragma once
#include <QWidget>
#include <QTimer>
#include "services/AppContext.h"
#include "shorts/ShortsController.h"
#include "shorts/ShortsNavigation.h"
class VideoStage; class QLabel; class QToolButton; class QPushButton; class QComboBox; class QSlider;

class ShortsPage : public QWidget {
    Q_OBJECT
public:
    explicit ShortsPage(AppContext& ctx, QWidget* p = nullptr);
    void onActivated();
    void onDeactivated();
    void handleAction(const QString& action);     // from keyboard or mpv-captured input
    ShortsController* controller() { return &ctrl_; }
signals:
    void exitRequested();
protected:
    void wheelEvent(QWheelEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;
private slots:
    void onCurrent(const VideoInfo& v, int idx);
private:
    void applyAction(const QString& a);
    void relayout();
    void refreshButtons();
    AppContext& ctx_;
    ShortsController ctrl_;
    ShortsNavigation nav_;
    VideoStage* stage_;
    QWidget* center_;
    QLabel *channel_, *title_, *desc_, *status_;
    QToolButton *like_, *comments_, *share_, *mute_, *prev_, *next_;
    QPushButton* subscribe_;
    QComboBox* topic_;
    QSlider* progress_;
    VideoInfo cur_;
    bool active_ = false;
};
