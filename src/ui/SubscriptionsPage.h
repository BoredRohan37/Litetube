#pragma once
#include <QWidget>
#include "services/AppContext.h"
#include "youtube/VideoInfo.h"
#include "youtube/AccountApi.h"
class QListWidget; class VideoGrid; class QLabel; class QPushButton;
class SubscriptionsPage : public QWidget {
    Q_OBJECT
public:
    SubscriptionsPage(AppContext& ctx, QWidget* p = nullptr);
    void refresh();
signals:
    void playRequested(const VideoInfo& v, const QList<VideoInfo>& context);
private slots:
    void onFinished(int id, const QList<VideoInfo>& items, const QString& next);
    void onFailed(int id, const QString& msg);
private:
    void loadChannel();
    void loadAccount();
    QList<ChannelInfo> acctChannels_;
    bool acct_ = false;
    AppContext& ctx_;
    QListWidget* channels_;
    VideoGrid* grid_;
    QLabel* status_;
    QPushButton* unsub_;
    int req_ = -1;
};
