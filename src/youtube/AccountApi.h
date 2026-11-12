#pragma once
#include <QObject>
#include <QJsonObject>
#include <QUrlQuery>
#include <functional>
#include "VideoInfo.h"
class AuthService;

struct ChannelInfo { QString id, title; };
struct PlaylistInfo { QString id, title; int count = 0; };

// Signed-in YouTube Data API calls: subscriptions, playlists, liked videos, rating, subscribing, comments.
class AccountApi : public QObject {
    Q_OBJECT
public:
    using Json = std::function<void(const QJsonObject&, const QString& err)>;
    explicit AccountApi(AuthService* auth, QObject* p = nullptr) : QObject(p), auth_(auth) {}
    bool available() const;
    void call(const QString& method, const QString& endpoint, const QUrlQuery& q, const QByteArray& body, Json cb);

    void subscriptions(std::function<void(const QList<ChannelInfo>&, const QString& err)> cb);
    void subscriptionFeed(const QList<ChannelInfo>& channels, std::function<void(const QList<VideoInfo>&, const QString& err)> cb);
    void playlists(std::function<void(const QList<PlaylistInfo>&, const QString& err)> cb);
    void playlistVideos(const QString& playlistId, const QString& token, std::function<void(const QList<VideoInfo>&, const QString& next, const QString& err)> cb);
    void likedVideos(const QString& token, std::function<void(const QList<VideoInfo>&, const QString& next, const QString& err)> cb);
    void rate(const QString& videoId, const QString& rating, std::function<void(const QString& err)> cb);          // like | dislike | none
    void subscribe(const QString& channelId, std::function<void(const QString& err)> cb);
    void unsubscribe(const QString& channelId, std::function<void(const QString& err)> cb);
    void addToPlaylist(const QString& playlistId, const QString& videoId, std::function<void(const QString& err)> cb);
    void createPlaylist(const QString& title, std::function<void(const PlaylistInfo&, const QString& err)> cb);
    void postComment(const QString& videoId, const QString& text, std::function<void(const QString& err)> cb);
private:
    AuthService* auth_;
};
