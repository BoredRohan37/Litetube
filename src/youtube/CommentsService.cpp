#include "CommentsService.h"
#include "AccountApi.h"
#include "services/AppSettings.h"
#include "utilities/Logger.h"
#include "utilities/Format.h"
#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QSet>

static AppSettings& S() { return AppSettings::instance(); }

CommentsService::CommentsService(AccountApi* a, QObject* p) : QObject(p), account_(a) {}
void CommentsService::cancel(int id) {
    live_.remove(id);
    if (QProcess* p = procs_.take(id)) p->kill();
    if (QNetworkReply* r = replies_.take(id)) { r->disconnect(); r->abort(); r->deleteLater(); }
}
static Comment fromSnippet(const QJsonObject& sn) {
    Comment c;
    c.author = sn["authorDisplayName"].toString();
    c.text = sn["textDisplay"].toString(sn["textOriginal"].toString());
    c.likes = sn["likeCount"].toInt();
    c.when = Format::ago(QDateTime::fromString(sn["publishedAt"].toString(), Qt::ISODate));
    return c;
}
void CommentsService::parseApi(int id, const QJsonObject& o) {
    if (!live_.remove(id)) return;
    QList<Comment> out;
    for (const QJsonValue& tv : o["items"].toArray()) {
        const QJsonObject t = tv.toObject(), sn = t["snippet"].toObject();
        Comment c = fromSnippet(sn["topLevelComment"].toObject()["snippet"].toObject());
        c.replyCount = sn["totalReplyCount"].toInt();
        for (const QJsonValue& rv : t["replies"].toObject()["comments"].toArray()) c.replies << fromSnippet(rv.toObject()["snippet"].toObject());
        out << c;
    }
    emit loaded(id, out, o["nextPageToken"].toString());
}
int CommentsService::load(const QString& vid, bool newest, const QString& token) {
    const int id = ++seq_;
    live_.insert(id);
    QUrlQuery q;
    q.addQueryItem("part", "snippet,replies"); q.addQueryItem("videoId", vid); q.addQueryItem("maxResults", "20");
    q.addQueryItem("order", newest ? "time" : "relevance"); q.addQueryItem("textFormat", "plainText");
    if (!token.isEmpty()) q.addQueryItem("pageToken", token);
    auto viaApiError = [this, id](const QJsonObject& o, const QString& err) {
        if (!live_.contains(id)) return;
        if (!err.isEmpty()) { live_.remove(id); emit failed(id, err.contains("commentsDisabled") ? QStringLiteral("Comments are turned off for this video.") : err); return; }
        parseApi(id, o);
    };
    if (account_ && account_->available()) { account_->call("GET", "commentThreads", q, {}, viaApiError); return id; }
    const QString key = S().str("apiKey").trimmed();
    if (!key.isEmpty()) {
        q.addQueryItem("key", key);
        QUrl url("https://www.googleapis.com/youtube/v3/commentThreads"); url.setQuery(q);
        QNetworkReply* r = nam_.get(QNetworkRequest(url));
        replies_[id] = r;
        connect(r, &QNetworkReply::finished, this, [this, id, r, viaApiError] {
            replies_.remove(id); r->deleteLater();
            const QJsonObject o = QJsonDocument::fromJson(r->readAll()).object();
            QString err;
            if (o.contains("error")) { const QJsonObject e = o["error"].toObject(); err = e["errors"].toArray().first().toObject()["reason"].toString() + ": " + e["message"].toString(); }
            else if (r->error() != QNetworkReply::NoError) err = r->errorString();
            viaApiError(o, err);
        });
        return id;
    }
    if (!token.isEmpty()) { QTimer::singleShot(0, this, [this, id] { if (live_.remove(id)) emit loaded(id, {}, QString()); }); return id; }   // yt-dlp returns one batch only
    // yt-dlp fallback: one batch of ~40 comments
    auto* p = new QProcess(this);
    procs_[id] = p;
    QStringList a{"-J", "--skip-download", "--no-playlist", "--no-warnings", "--write-comments", "--extractor-args",
                  QString("youtube:comment_sort=%1;max_comments=40,40,0,0").arg(newest ? "new" : "top"),
                  "--socket-timeout", QString::number(S().num("netTimeout")), "https://www.youtube.com/watch?v=" + vid};
    connect(p, &QProcess::errorOccurred, this, [this, id, p](QProcess::ProcessError e) {
        if (e != QProcess::FailedToStart) return;
        procs_.remove(id); p->deleteLater();
        if (live_.remove(id)) emit failed(id, "yt-dlp not found or failed to start");
    });
    connect(p, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this, id, p](int code, QProcess::ExitStatus) {
        const bool cancelled = !procs_.contains(id);
        procs_.remove(id);
        const QByteArray out = p->readAllStandardOutput();
        const QString err = QString::fromUtf8(p->readAllStandardError()).trimmed();
        p->deleteLater();
        if (cancelled || !live_.remove(id)) return;
        if (code != 0) { emit failed(id, err.split('\n').last()); return; }
        QList<Comment> list;
        for (const QJsonValue& cv : QJsonDocument::fromJson(out).object()["comments"].toArray()) {
            const QJsonObject c = cv.toObject();
            if (c["parent"].toString() != "root") continue;
            Comment k; k.author = c["author"].toString(); k.text = c["text"].toString(); k.likes = c["like_count"].toInt();
            k.when = Format::ago(QDateTime::fromSecsSinceEpoch(qint64(c["timestamp"].toDouble())));
            list << k;
        }
        emit loaded(id, list, QString());
    });
    QTimer::singleShot(S().num("netTimeout") * 6000, p, [this, id, p] {
        if (procs_.contains(id) && p->state() != QProcess::NotRunning) { procs_.remove(id); p->kill(); p->deleteLater(); if (live_.remove(id)) emit failed(id, "Timed out loading comments"); }
    });
    LOGI("yt-dlp comments for " + vid);
    p->start(S().str("ytdlpPath"), a);
    return id;
}
