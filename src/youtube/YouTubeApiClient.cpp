#include "YouTubeApiClient.h"
#include "services/AppSettings.h"
#include "utilities/Logger.h"
#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QRegularExpression>
#include <QDateTime>
#include <QTimer>

static AppSettings& S() { return AppSettings::instance(); }

YouTubeApiClient::YouTubeApiClient(QObject* p) : QObject(p) {}
bool YouTubeApiClient::hasApiKey() const {
    return !S().str("apiKey").trimmed().isEmpty() && QDateTime::currentSecsSinceEpoch() > apiBlockedUntil_;
}
void YouTubeApiClient::done(int id, const QList<VideoInfo>& v, const QString& next) {
    if (!live_.remove(id)) return;
    emit finished(id, v, next);
}
void YouTubeApiClient::fail(int id, const QString& m) {
    if (!live_.remove(id)) return;
    LOGW("request " + QString::number(id) + " failed: " + m);
    emit failed(id, m);
}
void YouTubeApiClient::cancel(int id) {
    live_.remove(id);
    if (QProcess* p = procs_.take(id)) p->kill();
    for (QNetworkReply* r : replies_.take(id)) { r->disconnect(); r->abort(); r->deleteLater(); }
}
void YouTubeApiClient::cancelAll() {
    const auto ids = QList<int>(live_.begin(), live_.end());
    for (int i : ids) cancel(i);
}

// ---------- official Data API v3 ----------
static int isoSeconds(const QString& iso) {
    static const QRegularExpression re("PT(?:(\\d+)H)?(?:(\\d+)M)?(?:(\\d+)S)?");
    const auto m = re.match(iso);
    return m.hasMatch() ? m.captured(1).toInt() * 3600 + m.captured(2).toInt() * 60 + m.captured(3).toInt() : 0;
}
QList<VideoInfo> YouTubeApiClient::parseApiVideos(const QJsonArray& items) {
    QList<VideoInfo> out;
    for (const QJsonValue& iv : items) {
        const QJsonObject o = iv.toObject(), sn = o["snippet"].toObject();
        VideoInfo v;
        v.id = o["id"].isString() ? o["id"].toString() : o["id"].toObject()["videoId"].toString();
        if (v.id.isEmpty()) continue;
        v.title = sn["title"].toString(); v.channel = sn["channelTitle"].toString(); v.channelId = sn["channelId"].toString();
        v.sortKey = sn["publishedAt"].toString();
        v.published = Format::ago(QDateTime::fromString(sn["publishedAt"].toString(), Qt::ISODate));
        v.seconds = isoSeconds(o["contentDetails"].toObject()["duration"].toString());
        v.viewCount = o["statistics"].toObject().contains("viewCount") ? o["statistics"].toObject()["viewCount"].toString().toLongLong() : -1;
        v.live = sn["liveBroadcastContent"].toString() == "live";
        v.isShort = v.seconds > 0 && v.seconds <= 60;
        out << v;
    }
    return out;
}
void YouTubeApiClient::apiGet(int id, const QString& endpoint, QUrlQuery q, int retries, std::function<void(const QJsonObject&, const ApiErr&)> cb) {
    q.addQueryItem("key", S().str("apiKey").trimmed());
    QUrl url("https://www.googleapis.com/youtube/v3/" + endpoint);
    url.setQuery(q);
    QNetworkRequest rq(url);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    rq.setTransferTimeout(S().num("netTimeout") * 1000);
#endif
    QNetworkReply* r = nam_.get(rq);
    replies_[id].append(r);
    connect(r, &QNetworkReply::finished, this, [=] {
        replies_[id].removeAll(r);
        r->deleteLater();
        if (!live_.contains(id)) return;
        const QByteArray body = r->readAll();
        const QJsonObject o = QJsonDocument::fromJson(body).object();
        ApiErr e;
        if (o.contains("error")) {
            const QJsonObject eo = o["error"].toObject();
            const QString reason = eo["errors"].toArray().first().toObject()["reason"].toString();
            e.message = eo["message"].toString();
            e.quotaOrKey = reason == "quotaExceeded" || reason == "dailyLimitExceeded" || reason == "keyInvalid" || reason == "forbidden" || reason == "rateLimitExceeded";
            if (reason == "quotaExceeded") e.message = "quotaExceeded: " + e.message;
            if (reason == "keyInvalid") e.message = "keyInvalid: " + e.message;
        } else if (r->error() != QNetworkReply::NoError) {
            if (retries > 0) { apiGet(id, endpoint, q, retries - 1, cb); return; }
            e.message = r->errorString(); e.network = true;
        }
        cb(o, e);
    });
}
void YouTubeApiClient::apiList(int id, const QString& endpoint, QUrlQuery q, bool needDetails, std::function<void()> fallback) {
    const int retries = S().num("netRetries");
    auto onErr = [=](const ApiErr& e) {
        if (e.quotaOrKey && fallback) {
            apiBlockedUntil_ = QDateTime::currentSecsSinceEpoch() + 3600;
            LOGW("API unusable (" + e.message + "), falling back to yt-dlp for an hour");
            fallback();
        } else fail(id, e.message);
    };
    apiGet(id, endpoint, q, retries, [=](const QJsonObject& o, const ApiErr& e) {
        if (!e.message.isEmpty()) { onErr(e); return; }
        const QString next = o["nextPageToken"].toString();
        if (!needDetails) { done(id, parseApiVideos(o["items"].toArray()), next); return; }
        QStringList ids;
        for (const QJsonValue& v : o["items"].toArray()) ids << v.toObject()["id"].toObject()["videoId"].toString();
        ids.removeAll(QString());
        if (ids.isEmpty()) { done(id, {}, QString()); return; }
        QUrlQuery dq; dq.addQueryItem("part", "snippet,contentDetails,statistics"); dq.addQueryItem("id", ids.join(','));
        apiGet(id, "videos", dq, retries, [=](const QJsonObject& d, const ApiErr& e2) {
            if (!e2.message.isEmpty()) { onErr(e2); return; }
            done(id, parseApiVideos(d["items"].toArray()), next);
        });
    });
}

// ---------- yt-dlp fallback ----------
void YouTubeApiClient::runYtdlp(int id, const QStringList& target, int skip, int page, bool shortsOnly) {
    auto* p = new QProcess(this);
    procs_[id] = p;
    QStringList a{"--flat-playlist", "-J", "--no-warnings", "--socket-timeout", QString::number(S().num("netTimeout"))};
    a += target;
    connect(p, &QProcess::errorOccurred, this, [this, id, p](QProcess::ProcessError e) {
        if (e != QProcess::FailedToStart) return;
        procs_.remove(id);
        fail(id, "yt-dlp not found or failed to start (" + p->program() + ")");
        p->deleteLater();
    });
    connect(p, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [=](int code, QProcess::ExitStatus) {
        const bool cancelled = !procs_.contains(id);
        procs_.remove(id);
        const QByteArray out = p->readAllStandardOutput();
        const QString err = QString::fromUtf8(p->readAllStandardError()).trimmed();
        p->deleteLater();
        if (cancelled || !live_.contains(id)) return;
        if (code != 0) { fail(id, err.split('\n').last()); return; }
        const QJsonArray entries = QJsonDocument::fromJson(out).object()["entries"].toArray();
        QList<VideoInfo> res;
        int idx = 0;
        for (const QJsonValue& ev : entries) {
            if (idx++ < skip) continue;
            const QJsonObject e = ev.toObject();
            VideoInfo v;
            v.id = e["id"].toString(); v.title = e["title"].toString();
            if (v.id.size() != 11 || v.title.startsWith("[")) continue;
            v.channel = e["channel"].toString(e["uploader"].toString()); v.channelId = e["channel_id"].toString();
            v.seconds = int(e["duration"].toDouble()); v.viewCount = e.contains("view_count") && !e["view_count"].isNull() ? qint64(e["view_count"].toDouble()) : -1;
            v.live = e["live_status"].toString() == "is_live";
            v.isShort = e["url"].toString().contains("/shorts/") || (v.seconds > 0 && v.seconds <= 60);
            if (shortsOnly && v.seconds > 61) continue;
            res << v;
        }
        done(id, res, res.isEmpty() ? QString() : QString::number(page));
    });
    QTimer::singleShot(S().num("netTimeout") * 6000, p, [this, id, p] {
        if (procs_.contains(id) && p->state() != QProcess::NotRunning) { procs_.remove(id); p->kill(); fail(id, "Timed out waiting for yt-dlp"); p->deleteLater(); }
    });
    LOGI("yt-dlp " + a.join(' '));
    p->start(S().str("ytdlpPath"), a);
}

// ---------- public requests ----------
int YouTubeApiClient::search(const QString& q, int filter, const QString& token) {
    const int id = newId();
    const int n = qBound(5, S().num("maxResults"), 40);
    QString term = q;
    if (filter == Shorts) term += " #shorts";
    auto viaYtdlp = [=] {
        const int page = token.toInt();
        QString t = term;
        if (filter == Live) t += " live";
        runYtdlp(id, {QString("ytsearch%1:%2").arg(n * (page + 1)).arg(t)}, page * n, page + 1, filter == Shorts);
    };
    if (hasApiKey()) {
        QUrlQuery qs;
        qs.addQueryItem("part", "snippet"); qs.addQueryItem("type", "video"); qs.addQueryItem("maxResults", QString::number(n));
        qs.addQueryItem("q", term);
        if (filter == Shorts) qs.addQueryItem("videoDuration", "short");
        if (filter == Live) qs.addQueryItem("eventType", "live");
        if (!token.isEmpty()) qs.addQueryItem("pageToken", token);
        apiList(id, "search", qs, true, viaYtdlp);
    } else viaYtdlp();
    return id;
}
int YouTubeApiClient::trending(const QString& cat, const QString& token) {
    const int id = newId();
    const int n = qBound(5, S().num("maxResults"), 40);
    static const QHash<QString, QString> catIds{{"Music", "10"}, {"Gaming", "20"}, {"Sports", "17"}, {"News", "25"}, {"Movies", "1"}};
    static const QHash<QString, QString> terms{{"Now", "trending videos"}, {"Music", "trending music"}, {"Gaming", "trending gaming"},
                                               {"Sports", "sports highlights"}, {"News", "news today"}, {"Movies", "movie trailers"}};
    auto viaYtdlp = [=] {
        const int page = token.toInt();
        runYtdlp(id, {QString("ytsearch%1:%2").arg(n * (page + 1)).arg(terms.value(cat, "trending videos"))}, page * n, page + 1, false);
    };
    if (hasApiKey()) {
        QUrlQuery qs;
        qs.addQueryItem("part", "snippet,contentDetails,statistics"); qs.addQueryItem("chart", "mostPopular");
        qs.addQueryItem("regionCode", S().str("region")); qs.addQueryItem("maxResults", QString::number(n));
        if (catIds.contains(cat)) qs.addQueryItem("videoCategoryId", catIds[cat]);
        if (!token.isEmpty()) qs.addQueryItem("pageToken", token);
        apiList(id, "videos", qs, false, viaYtdlp);
    } else viaYtdlp();
    return id;
}
int YouTubeApiClient::shorts(const QString& topic, const QString& token) {
    return search(topic.isEmpty() ? QStringLiteral("shorts") : topic, Shorts, token);
}
int YouTubeApiClient::channelVideos(const QString& ch, const QString& token) {
    const int id = newId();
    const int n = qBound(5, S().num("maxResults"), 40);
    auto viaYtdlp = [=] {
        const int page = token.toInt();
        runYtdlp(id, {"--playlist-end", QString::number(n * (page + 1)), "https://www.youtube.com/channel/" + ch + "/videos"}, page * n, page + 1, false);
    };
    if (hasApiKey()) {
        QUrlQuery qs;
        qs.addQueryItem("part", "snippet"); qs.addQueryItem("type", "video"); qs.addQueryItem("order", "date");
        qs.addQueryItem("channelId", ch); qs.addQueryItem("maxResults", QString::number(n));
        if (!token.isEmpty()) qs.addQueryItem("pageToken", token);
        apiList(id, "search", qs, true, viaYtdlp);
    } else viaYtdlp();
    return id;
}

int YouTubeApiClient::cookieFeed(const QString& kind) {
    const int id = newId();
    const QString browser = S().str("cookiesBrowser");
    if (browser.isEmpty()) { QTimer::singleShot(0, this, [this, id] { fail(id, "Enable browser-cookie mode in Settings > Account first."); }); return id; }
    QStringList args{"--cookies-from-browser", browser, "--playlist-end", "40"};
    args << ":" + kind;
    runYtdlp(id, args, 0, 1, false);
    return id;
}
