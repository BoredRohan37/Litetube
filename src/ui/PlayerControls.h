#pragma once
#include <QWidget>
#include <QSlider>
#include <QMouseEvent>
#include <QStyle>
class QToolButton; class QLabel; class PlaybackSession;

class ClickSlider : public QSlider {
    Q_OBJECT
public:
    explicit ClickSlider(Qt::Orientation o, QWidget* p = nullptr) : QSlider(o, p) {}
protected:
    void mousePressEvent(QMouseEvent* e) override {
        if (e->button() == Qt::LeftButton) {
            setValue(QStyle::sliderValueFromPosition(minimum(), maximum(), e->x(), width()));
            emit sliderMoved(value());
        }
        QSlider::mousePressEvent(e);
    }
};

class PlayerControls : public QWidget {
    Q_OBJECT
public:
    PlayerControls(PlaybackSession* s, bool mini, QWidget* p = nullptr);
    void setNextVisible(bool v);
signals:
    void toggleFullscreen();
    void requestCompact();
    void nextRequested();
    void statsToggled(bool on);
private slots:
    void onPosition(double pos, double dur);
    void buildSettingsMenu();
    void buildSubMenu();
private:
    PlaybackSession* s_;
    bool mini_;
    bool dragging_ = false;
    QToolButton *play_, *next_, *mute_, *cc_, *gear_, *compact_, *full_;
    ClickSlider *seek_, *vol_;
    QLabel* time_;
    QLabel* buffering_;
    class QMenu *gearMenu_, *ccMenu_;
};
