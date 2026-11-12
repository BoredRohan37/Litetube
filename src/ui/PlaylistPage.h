#pragma once
#include <QWidget>
#include "services/AppContext.h"
#include "youtube/VideoInfo.h"
class QListWidget; class VideoGrid; class QPushButton; class QLabel;

class PlaylistPage : public QWidget {
    Q_OBJECT
public:
    explicit PlaylistPage(AppContext& ctx, QWidget* p = nullptr);
    void selectList(const QString& name);
signals:
    void playQueueRequested(const QList<VideoInfo>& list, int index, bool shuffle, bool repeat);
private:
    void refreshLists();
    void refreshVideos();
    QString current() const;
    void loadRemote(const QString& id);
    bool isRemote() const;
    QString remoteToken_;
    QList<VideoInfo> remoteVideos_;
    QString remoteId_;
    AppContext& ctx_;
    QListWidget* lists_;
    VideoGrid* grid_;
    QLabel* title_;
    QPushButton *play_, *shuffle_, *repeat_, *up_, *down_, *rename_, *del_;
    bool busy_ = false;
};
