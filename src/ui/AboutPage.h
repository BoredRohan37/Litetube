#pragma once
#include <QWidget>
class PlaybackSession; class QLabel;
class AboutPage : public QWidget {
    Q_OBJECT
public:
    explicit AboutPage(PlaybackSession* s, QWidget* p = nullptr);
    void refresh();
private:
    PlaybackSession* s_;
    QLabel *deps_;
};
