#include "StreamResolver.h"
#include "services/AppSettings.h"
#include "utilities/Logger.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>
#include <QTimer>
#include <algorithm>

static int codecRank(const QString& c) {
    if (c.startsWith("avc1")) return 0;
    if (c.startsWith("vp9") || c.startsWith("vp09")) return 1;
    if (c.startsWith("av01")) return 3;
    return 2;
}

QList<int> ResolvedStream::heights() const {
    QList<int> h;
    for (const StreamFormat& f : formats) if (f.hasVideo && f.height > 0 && !h.contains(f.height)) h << f.height;
    std::sort(h.begin(), h.end(), std::greater<int>());
    return h;
}

bool ResolvedStream::pick(int maxH, QString& video, QString& audio, int& height) const {
    const int limit = maxH > 0 ? maxH : 100000;
    const StreamFormat *prog = nullptr, *vid = nullptr, *aud = nullptr;
    auto betterV = [](const StreamFormat* cur, const StreamFormat& c) {
        if (!cur) return true;
        if (c.height != cur->height) return c.height > cur->height;
        if (codecRank(c.vcodec) != codecRank(cur->vcodec)) return codecRank(c.vcodec) < codecRank(cur->vcodec);
        return c.tbr > cur->tbr;
    };
    int effLimit = limit;
    for (int pass = 0; pass < 2 && !prog && !vid; ++pass) {
        for (const StreamFormat& f : formats) {
            if (!f.hasVideo || f.height > effLimit) continue;
            const StreamFormat*& slot = f.hasAudio ? prog : vid;
            if (betterV(slot, f)) slot = &f;
        }
        if (!prog && !vid) {            // nothing fits the limit: fall back to the lowest height available
            int lowest = 1 << 30;
            for (const StreamFormat& f : formats) if (f.hasVideo && f.height > 0) lowest = qMin(lowest, f.height);
            if (lowest == (1 << 30)) break;
            effLimit = lowest;
        }
    }
    for (const StreamFormat& f : formats) {
        if (!f.hasAudio || f.hasVideo) continue;
        const bool m4a = f.acodec.startsWith("mp4a");
        const bool curM4a = aud && aud->acodec.startsWith("mp4a");
        if (!aud || (m4a && !curM4a) || (m4a == curM4a && f.abr > aud->abr)) aud = &f;
    }
    if (!prog && !vid) return false;
    if (prog && (!vid || !aud || prog->height >= vid->height)) {
        video = prog->url; audio.clear(); height = prog->height; return true;
    }
    video = vid->url; audio = aud ? aud->url : QString(); height = vid->height;
    return true;
}

ResolvedStream StreamResolver::parse(const QByteArray& json, QString* err) {
    ResolvedStream r;
    const QJsonObject o = QJsonDocument::fromJson(json).object();
    if (o.isEmpty()) { if (err) *err = "Unexpected yt-dlp output"; return r; }
    r.id = o["id"].toString(); r.title = o["title"].toString();
    r.channel = o["channel"].toString(o["uploader"].toString());
    r.channelId = o["channel_id"].toString();
    r.description = o["description"].toString();
    r.duration = int(o["duration"].toDouble());
    r.live = o["is_live"].toBool();
    for (const QJsonValue& v : o["formats"].toArray()) {
        const QJsonObject f = v.toObject();
        const QString url = f["url"].toString(), proto = f["protocol"].toString();
        if (url.isEmpty() || f["ext"].toString() == "mhtml" || f["format_note"].toString() == "storyboard") continue;
        if (proto.startsWith("m3u8") && !r.live) continue;
        StreamFormat sf;
        sf.url = url; sf.height = f["height"].toInt();
        sf.vcodec = f["vcodec"].toString(); sf.acodec = f["acodec"].toString();
        sf.hasVideo = !sf.vcodec.isEmpty() && sf.vcodec != "none";
        sf.hasAudio = !sf.acodec.isEmpty() && sf.acodec != "none";
        sf.tbr = f["tbr"].toDouble(); sf.abr = f["abr"].toDouble();
        if (sf.hasVideo || sf.hasAudio) r.formats << sf;
    }
    const QJsonObject subs = o["subtitles"].toObject();
    for (const QString& lang : subs.keys()) {
        QString url;
        for (const QJsonValue& e : subs[lang].toArray()) {
            if (e.toObject()["ext"].toString() == "vtt") { url = e.toObject()["url"].toString(); break; }
            if (url.isEmpty()) url = e.toObject()["url"].toString();
        }
        if (!url.isEmpty()) r.subs << SubTrack{lang, lang, url};
    }
    const QString pref = AppSettings::instance().str("subLang");
    const QJsonArray auto_ = o["automatic_captions"].toObject()[pref].toArray();
    for (const QJsonValue& e : auto_)
        if (e.toObject()["ext"].toString() == "vtt") { r.subs << SubTrack{pref, pref + " (auto)", e.toObject()["url"].toString()}; break; }
    r.resolvedAt = QDateTime::currentSecsSinceEpoch();
    if (r.formats.isEmpty() && err) *err = "No playable formats were returned for this video.";
    return r;
}

int StreamResolver::resolve(const QString& id) {
    const int rid = ++seq_;
    AppSettings& s = AppSettings::instance();
    auto* p = new QProcess(this);
    procs_[rid] = p;
    QStringList a{"-J", "--no-playlist", "--no-warnings", "--socket-timeout", QString::number(s.num("netTimeout")),
                  "--retries", QString::number(s.num("netRetries")), "https://www.youtube.com/watch?v=" + id};
    connect(p, &QProcess::errorOccurred, this, [this, rid, p](QProcess::ProcessError e) {
        if (e != QProcess::FailedToStart || !procs_.contains(rid)) return;
        procs_.remove(rid);
        emit failed(rid, "yt-dlp not found or failed to start (" + p->program() + ")");
        p->deleteLater();
    });
    connect(p, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this, rid, p](int code, QProcess::ExitStatus) {
        if (!procs_.contains(rid)) { p->deleteLater(); return; }   // cancelled
        procs_.remove(rid);
        const QByteArray out = p->readAllStandardOutput();
        const QString errText = QString::fromUtf8(p->readAllStandardError()).trimmed();
        p->deleteLater();
        if (code != 0) {
            const QStringList lines = errText.split('\n');
            emit failed(rid, lines.isEmpty() ? QStringLiteral("yt-dlp failed") : lines.last());
            return;
        }
        QString err;
        ResolvedStream r = parse(out, &err);
        if (!err.isEmpty() && r.formats.isEmpty()) emit failed(rid, err);
        else emit resolved(rid, r);
    });
    QTimer::singleShot(s.num("netTimeout") * 4000, p, [this, rid, p] {
        if (procs_.contains(rid) && p->state() != QProcess::NotRunning) { procs_.remove(rid); p->kill(); emit failed(rid, "Timed out resolving the stream"); p->deleteLater(); }
    });
    LOGI("resolve " + id);
    p->start(s.str("ytdlpPath"), a);
    return rid;
}
void StreamResolver::cancel(int rid) {
    if (QProcess* p = procs_.take(rid)) { p->kill(); }
}
void StreamResolver::cancelAll() {
    const auto ids = procs_.keys();
    for (int id : ids) cancel(id);
}
