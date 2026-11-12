#pragma once
#include <QObject>
#include <QHash>
#include <QSet>
#include <QProcess>
#include <QNetworkAccessManager>
#include <functional>
#include "VideoInfo.h"

class QNetworkReply;
class QUrlQuery;
class QJsonArray;

class YouTubeApiClient : public QObject {
    Q_OBJECT
public:
    enum Filter { All = 0, Shorts = 1, Videos = 2, Live = 3 };
    explicit YouTubeApiClient(QObject* p = nullptr);
    bool hasApiKey() const;
    int search(const QString& q, int filter, const QString& pageToken = QString());
    int trending(const QString& category, const QString& pageToken = QString());
    int shorts(const QString& topic, const QString& pageToken = QString());
    int channelVideos(const QString& channelId, const QString& pageToken = QString());
    void cancel(int id);
    void cancelAll();
    static QList<VideoInfo> parseApiVideos(const QJsonArray& items);
    // personalised feeds through yt-dlp + browser cookies (kind: ytrec, ytsubs, ytwatchlater, ythistory, ytfav)
    int cookieFeed(const QString& kind);
signals:
    void finished(int id, const QList<VideoInfo>& items, const QString& nextToken);
    void failed(int id, const QString& message);
private:
    struct ApiErr { QString message; bool quotaOrKey = false; bool network = false; };
    void apiGet(int id, const QString& endpoint, QUrlQuery q, int retriesLeft, std::function<void(const QJsonObject&, const ApiErr&)> cb);
    void apiList(int id, const QString& endpoint, QUrlQuery q, bool needDetails, std::function<void()> fallback);
    void runYtdlp(int id, const QStringList& target, int skip, int page, bool shortsOnly);
    void done(int id, const QList<VideoInfo>& v, const QString& next);
    void fail(int id, const QString& m);
    int newId() { int i = ++seq_; live_.insert(i); return i; }

    QNetworkAccessManager nam_;
    int seq_ = 0;
    QSet<int> live_;
    QHash<int, QProcess*> procs_;
    QHash<int, QList<QNetworkReply*>> replies_;
    qint64 apiBlockedUntil_ = 0;
};
