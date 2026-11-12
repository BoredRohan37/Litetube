#include "AccountApi.h"
#include "YouTubeApiClient.h"
#include "services/AuthService.h"
#include "utilities/Logger.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QPointer>
#include <algorithm>

bool AccountApi::available() const { return auth_->signedIn(); }

void AccountApi::call(const QString& method, const QString& endpoint, const QUrlQuery& q, const QByteArray& body, Json cb) {
    QPointer<AccountApi> self(this);
    auth_->withToken([=](const QString& tok, const QString& err) {
        if (!self) return;
        if (!err.isEmpty()) { cb({}, err); return; }
        QUrl url("https://www.googleapis.com/youtube/v3/" + endpoint);
        url.setQuery(q);
        QNetworkRequest rq(url);
        rq.setRawHeader("Authorization", "Bearer " + tok.toUtf8());
        rq.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        QNetworkReply* r = nullptr;
        if (method == "GET") r = auth_->nam()->get(rq);
        else if (method == "POST") r = auth_->nam()->post(rq, body);
        else r = auth_->nam()->sendCustomRequest(rq, method.toUtf8(), body);
        connect(r, &QNetworkReply::finished, self.data(), [r, cb] {
            r->deleteLater();
            const QByteArray data = r->readAll();
            const QJsonObject o = QJsonDocument::fromJson(data).object();
            if (o.contains("error")) {
                const QJsonObject e = o["error"].toObject();
                const QString reason = e["errors"].toArray().first().toObject()["reason"].toString();
                cb(o, (reason.isEmpty() ? QString() : reason + ": ") + e["message"].toString());
            } else if (r->error() != QNetworkReply::NoError && data.isEmpty() && r->error() != QNetworkReply::NoError && r->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() >= 300)
                cb(o, r->errorString());
            else cb(o, QString());
        });
    });
}

void AccountApi::subscriptions(std::function<void(const QList<ChannelInfo>&, const QString&)> cb) {
    QUrlQuery q; q.addQueryItem("part", "snippet"); q.addQueryItem("mine", "true"); q.addQueryItem("maxResults", "50"); q.addQueryItem("order", "relevance");
    call("GET", "subscriptions", q, {}, [cb](const QJsonObject& o, const QString& err) {
        QList<ChannelInfo> l;
        for (const QJsonValue& v : o["items"].toArray()) {
            const QJsonObject sn = v.toObject()["snippet"].toObject();
            l << ChannelInfo{sn["resourceId"].toObject()["channelId"].toString(), sn["title"].toString()};
        }
        cb(l, err);
    });
}
void AccountApi::subscriptionFeed(const QList<ChannelInfo>& channels, std::function<void(const QList<VideoInfo>&, const QString&)> cb) {
    struct State { QList<VideoInfo> all; int pending = 0; QString err; };
    auto st = QSharedPointer<State>::create();
    const QList<ChannelInfo> use = channels.mid(0, 25);
    if (use.isEmpty()) { cb({}, QString()); return; }
    st->pending = use.size();
    auto finish = [this, st, cb] {
        if (--st->pending > 0) return;
        std::sort(st->all.begin(), st->all.end(), [](const VideoInfo& a, const VideoInfo& b) { return a.published < b.published; });   // placeholder order, refined below
        // fetch durations/views in batches of 50
        QStringList ids; for (const VideoInfo& v : st->all) ids << v.id;
        if (ids.isEmpty()) { cb({}, st->err); return; }
        auto out = QSharedPointer<QList<VideoInfo>>::create();
        auto left = QSharedPointer<int>::create((ids.size() + 49) / 50);
        for (int i = 0; i < ids.size(); i += 50) {
            QUrlQuery q; q.addQueryItem("part", "snippet,contentDetails,statistics"); q.addQueryItem("id", ids.mid(i, 50).join(','));
            call("GET", "videos", q, {}, [out, left, cb, st](const QJsonObject& o, const QString& err) {
                if (!err.isEmpty()) st->err = err;
                *out += YouTubeApiClient::parseApiVideos(o["items"].toArray());
                if (--*left == 0) {
                    std::sort(out->begin(), out->end(), [](const VideoInfo& a, const VideoInfo& b) { return a.sortKey > b.sortKey; });
                    cb(*out, st->err);
                }
            });
        }
    };
    for (const ChannelInfo& c : use) {
        QString uploads = c.id; if (uploads.startsWith("UC")) uploads.replace(0, 2, "UU");
        QUrlQuery q; q.addQueryItem("part", "snippet"); q.addQueryItem("playlistId", uploads); q.addQueryItem("maxResults", "4");
        call("GET", "playlistItems", q, {}, [st, finish](const QJsonObject& o, const QString&) {
            for (const QJsonValue& v : o["items"].toArray()) {
                const QJsonObject sn = v.toObject()["snippet"].toObject();
                VideoInfo vi; vi.id = sn["resourceId"].toObject()["videoId"].toString(); vi.title = sn["title"].toString();
                vi.published = sn["publishedAt"].toString();
                if (!vi.id.isEmpty()) st->all << vi;
            }
            finish();
        });
    }
}
void AccountApi::playlists(std::function<void(const QList<PlaylistInfo>&, const QString&)> cb) {
    QUrlQuery q; q.addQueryItem("part", "snippet,contentDetails"); q.addQueryItem("mine", "true"); q.addQueryItem("maxResults", "50");
    call("GET", "playlists", q, {}, [cb](const QJsonObject& o, const QString& err) {
        QList<PlaylistInfo> l;
        for (const QJsonValue& v : o["items"].toArray()) {
            const QJsonObject po = v.toObject();
            l << PlaylistInfo{po["id"].toString(), po["snippet"].toObject()["title"].toString(), po["contentDetails"].toObject()["itemCount"].toInt()};
        }
        cb(l, err);
    });
}
void AccountApi::playlistVideos(const QString& id, const QString& token, std::function<void(const QList<VideoInfo>&, const QString&, const QString&)> cb) {
    QUrlQuery q; q.addQueryItem("part", "contentDetails"); q.addQueryItem("playlistId", id); q.addQueryItem("maxResults", "30");
    if (!token.isEmpty()) q.addQueryItem("pageToken", token);
    call("GET", "playlistItems", q, {}, [this, cb](const QJsonObject& o, const QString& err) {
        if (!err.isEmpty()) { cb({}, QString(), err); return; }
        QStringList ids; for (const QJsonValue& v : o["items"].toArray()) ids << v.toObject()["contentDetails"].toObject()["videoId"].toString();
        const QString next = o["nextPageToken"].toString();
        if (ids.isEmpty()) { cb({}, QString(), QString()); return; }
        QUrlQuery vq; vq.addQueryItem("part", "snippet,contentDetails,statistics"); vq.addQueryItem("id", ids.join(','));
        call("GET", "videos", vq, {}, [cb, next](const QJsonObject& d, const QString& e2) { cb(YouTubeApiClient::parseApiVideos(d["items"].toArray()), next, e2); });
    });
}
void AccountApi::likedVideos(const QString& token, std::function<void(const QList<VideoInfo>&, const QString&, const QString&)> cb) {
    QUrlQuery q; q.addQueryItem("part", "snippet,contentDetails,statistics"); q.addQueryItem("myRating", "like"); q.addQueryItem("maxResults", "30");
    if (!token.isEmpty()) q.addQueryItem("pageToken", token);
    call("GET", "videos", q, {}, [cb](const QJsonObject& o, const QString& err) { cb(YouTubeApiClient::parseApiVideos(o["items"].toArray()), o["nextPageToken"].toString(), err); });
}
void AccountApi::rate(const QString& id, const QString& rating, std::function<void(const QString&)> cb) {
    QUrlQuery q; q.addQueryItem("id", id); q.addQueryItem("rating", rating);
    call("POST", "videos/rate", q, {}, [cb](const QJsonObject&, const QString& err) { cb(err); });
}
void AccountApi::subscribe(const QString& ch, std::function<void(const QString&)> cb) {
    QUrlQuery q; q.addQueryItem("part", "snippet");
    const QJsonObject body{{"snippet", QJsonObject{{"resourceId", QJsonObject{{"kind", "youtube#channel"}, {"channelId", ch}}}}}};
    call("POST", "subscriptions", q, QJsonDocument(body).toJson(), [cb](const QJsonObject&, const QString& err) { cb(err); });
}
void AccountApi::unsubscribe(const QString& ch, std::function<void(const QString&)> cb) {
    QUrlQuery q; q.addQueryItem("part", "id"); q.addQueryItem("mine", "true"); q.addQueryItem("forChannelId", ch);
    call("GET", "subscriptions", q, {}, [this, cb](const QJsonObject& o, const QString& err) {
        const QJsonArray items = o["items"].toArray();
        if (!err.isEmpty() || items.isEmpty()) { cb(err.isEmpty() ? QStringLiteral("Not subscribed") : err); return; }
        QUrlQuery dq; dq.addQueryItem("id", items[0].toObject()["id"].toString());
        call("DELETE", "subscriptions", dq, {}, [cb](const QJsonObject&, const QString& e2) { cb(e2); });
    });
}
void AccountApi::addToPlaylist(const QString& pl, const QString& vid, std::function<void(const QString&)> cb) {
    QUrlQuery q; q.addQueryItem("part", "snippet");
    const QJsonObject body{{"snippet", QJsonObject{{"playlistId", pl}, {"resourceId", QJsonObject{{"kind", "youtube#video"}, {"videoId", vid}}}}}};
    call("POST", "playlistItems", q, QJsonDocument(body).toJson(), [cb](const QJsonObject&, const QString& err) { cb(err); });
}
void AccountApi::createPlaylist(const QString& title, std::function<void(const PlaylistInfo&, const QString&)> cb) {
    QUrlQuery q; q.addQueryItem("part", "snippet,status");
    const QJsonObject body{{"snippet", QJsonObject{{"title", title}}}, {"status", QJsonObject{{"privacyStatus", "private"}}}};
    call("POST", "playlists", q, QJsonDocument(body).toJson(), [cb](const QJsonObject& o, const QString& err) {
        cb(PlaylistInfo{o["id"].toString(), o["snippet"].toObject()["title"].toString(), 0}, err);
    });
}
void AccountApi::postComment(const QString& vid, const QString& text, std::function<void(const QString&)> cb) {
    QUrlQuery q; q.addQueryItem("part", "snippet");
    const QJsonObject body{{"snippet", QJsonObject{{"videoId", vid}, {"topLevelComment", QJsonObject{{"snippet", QJsonObject{{"textOriginal", text}}}}}}}};
    call("POST", "commentThreads", q, QJsonDocument(body).toJson(), [cb](const QJsonObject&, const QString& err) { cb(err); });
}
