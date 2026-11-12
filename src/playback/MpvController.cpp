#include "MpvController.h"
#include "services/AppSettings.h"
#include "utilities/Logger.h"
#include <mpv/client.h>
#include <QMetaObject>
#include <QVariantMap>
#include <QUrl>
#include <QDir>
#include <QFile>
#include <QCoreApplication>

static void wakeup(void* ctx) { QMetaObject::invokeMethod(static_cast<MpvController*>(ctx), "onWakeup", Qt::QueuedConnection); }

void MpvController::setStr(const char* n, const QString& v) { if (h_) mpv_set_property_string(h_, n, v.toUtf8().constData()); }
QString MpvController::prop(const char* name) const {
    if (!h_) return QString();
    char* s = mpv_get_property_string(h_, name);
    if (!s) return QString();
    QString r = QString::fromUtf8(s);
    mpv_free(s);
    return r;
}
QString MpvController::version() const { return prop("mpv-version"); }

bool MpvController::init(quintptr wid, QString* err) {
    shutdown();
    AppSettings& s = AppSettings::instance();
    h_ = mpv_create();
    if (!h_) { if (err) *err = "mpv_create failed"; return false; }
    auto opt = [&](const char* k, const QString& v) { mpv_set_option_string(h_, k, v.toUtf8().constData()); };
    int64_t w = int64_t(wid);
    if (wid) mpv_set_option(h_, "wid", MPV_FORMAT_INT64, &w);
    opt("config", "no"); opt("load-scripts", "no"); opt("terminal", "no"); opt("ytdl", "no");
    opt("osc", "no"); opt("osd-level", "0"); opt("input-default-bindings", "no"); opt("input-vo-keyboard", "yes");
    opt("keep-open", "yes"); opt("idle", "yes"); opt("force-window", "no");
    opt("audio-display", "no"); opt("sub-auto", "no"); opt("cursor-autohide", "no");
    const QString vo = s.str("vo");
    // fall back through the chain instead of failing outright when a driver rejects Direct3D
    opt("vo", vo == "direct3d" ? QStringLiteral("direct3d,gpu") : vo == "auto" ? QString() : vo);
    if (vo == "direct3d" && s.flag("d3dCompat")) {
        for (const char* o : {"vo-direct3d-disable-shaders", "vo-direct3d-only-8bit", "vo-direct3d-force-power-of-2"})
            if (mpv_set_option_string(h_, o, "yes") < 0) LOGW(QString("option not supported by this libmpv: ") + o);
    }
    if (!s.str("ao").isEmpty()) opt("ao", s.str("ao"));
    opt("hwdec", s.str("hwdec"));
    opt("vd-lavc-threads", "2");
    if (s.flag("fastDecode")) { opt("vd-lavc-fast", "yes"); opt("vd-lavc-skiploopfilter", "nonref"); }
    opt("framedrop", "vo"); opt("video-sync", "audio");
    opt("cache", "yes"); opt("cache-secs", "30");
    opt("demuxer-max-bytes", QString("%1MiB").arg(qMax(8, s.num("cacheMB"))));
    opt("demuxer-max-back-bytes", QString("%1MiB").arg(qMax(4, s.num("cacheMB") / 4)));
    opt("network-timeout", QString::number(s.num("netTimeout")));
    if (!s.str("audioLang").isEmpty()) opt("alang", s.str("audioLang"));
    opt("msg-level", "all=warn");
    applySubtitleStyle();
    if (mpv_initialize(h_) < 0) {
        if (err) *err = "mpv_initialize failed";
        mpv_terminate_destroy(h_); h_ = nullptr; return false;
    }
    mpv_request_log_messages(h_, "warn");
    mpv_observe_property(h_, 1, "time-pos", MPV_FORMAT_DOUBLE);
    mpv_observe_property(h_, 2, "duration", MPV_FORMAT_DOUBLE);
    mpv_observe_property(h_, 3, "pause", MPV_FORMAT_FLAG);
    mpv_observe_property(h_, 4, "paused-for-cache", MPV_FORMAT_FLAG);
    mpv_observe_property(h_, 5, "track-list/count", MPV_FORMAT_INT64);
    mpv_observe_property(h_, 6, "eof-reached", MPV_FORMAT_FLAG);
    mpv_set_wakeup_callback(h_, wakeup, this);
    static const char* binds[][2] = {{"WHEEL_UP", "wheelup"}, {"WHEEL_DOWN", "wheeldown"}, {"MBTN_LEFT", "click"},
        {"MBTN_LEFT_DBL", "dblclick"}, {"SPACE", "space"}, {"m", "mute"}, {"UP", "up"}, {"DOWN", "down"}, {"PGUP", "pgup"},
        {"PGDWN", "pgdown"}, {"ESC", "esc"}, {"f", "fullscreen"}, {"LEFT", "left"}, {"RIGHT", "right"}};
    for (auto& b : binds) {
        const QByteArray cmd = QByteArray("script-message lt ") + b[1];
        const char* a[] = {"keybind", b[0], cmd.constData(), nullptr};
        mpv_command(h_, a);
    }
    setVolume(s.num("volume"));
    setMute(s.flag("muted"));
    LOGI("mpv initialised: " + version() + " vo=" + s.str("vo") + " hwdec=" + s.str("hwdec"));
    return true;
}
void MpvController::shutdown() {
    if (!h_) return;
    mpv_set_wakeup_callback(h_, nullptr, nullptr);
    mpv_terminate_destroy(h_);
    h_ = nullptr; pos_ = dur_ = 0; eof_ = false;
}

bool MpvController::load(const QString& video, const QString& audio, double start, bool loop) {
    if (!h_) return false;
    eof_ = false; pos_ = 0; dur_ = 0;
    pendingAudio_ = audio;
    mpv_set_property_string(h_, "start", start > 1 ? QByteArray::number(start, 'f', 1).constData() : "none");
    setLoop(loop);
    // when audio is a separate stream, start paused so video and audio begin together once audio is attached
    mpv_set_property_string(h_, "pause", audio.isEmpty() ? "no" : "yes");
    const QByteArray u = video.toUtf8();
    const char* a[] = {"loadfile", u.constData(), "replace", nullptr};
    LOGI(QString("loadfile video=%1 audio=%2").arg(QUrl(video).host(), audio.isEmpty() ? QStringLiteral("(in same stream)") : QUrl(audio).host()));
    return mpv_command(h_, a) >= 0;
}
void MpvController::applySubtitleStyle() {
    if (!h_) return;
    AppSettings& s = AppSettings::instance();
    auto opt = [&](const char* k, const QString& v) { mpv_set_option_string(h_, k, v.toUtf8().constData()); };
    // MX Player look: bold sans-serif, white, thick black outline, soft shadow, no background box, near the bottom
    const bool haveRoboto = QFile::exists(QCoreApplication::applicationDirPath() + "/fonts/Roboto-Medium.ttf");
    opt("sub-font", (haveRoboto && s.str("subFont") == "Arial") ? QStringLiteral("Roboto Medium") : s.str("subFont"));
    opt("sub-bold", "yes");
    opt("sub-font-size", QString::number(s.num("subSize")));
    opt("sub-color", s.str("subColor"));
    opt("sub-border-color", "#000000");
    opt("sub-border-size", QString::number(s.dbl("subOutline"), 'f', 1));
    opt("sub-shadow-offset", "1.2");
    opt("sub-shadow-color", "#99000000");
    opt("sub-back-color", s.flag("subBox") ? QStringLiteral("#B0000000") : QStringLiteral("#00000000"));
    opt("sub-pos", QString::number(s.num("subPos")));
    opt("sub-ass-override", "force");
    opt("sub-scale", QString::number(s.num("subScale") / 100.0));
    const QString dir = QCoreApplication::applicationDirPath() + "/fonts";
    if (QDir(dir).exists()) opt("sub-fonts-dir", dir);
}

void MpvController::stop() { if (h_) { const char* a[] = {"stop", nullptr}; mpv_command(h_, a); eof_ = false; pos_ = dur_ = 0; } }
void MpvController::setPause(bool p) { if (h_) mpv_set_property_string(h_, "pause", p ? "yes" : "no"); }
bool MpvController::paused() const { return prop("pause") == "yes"; }
void MpvController::seekAbs(double s) {
    if (!h_) return;
    const QByteArray t = QByteArray::number(qMax(0.0, s), 'f', 2);
    const char* a[] = {"seek", t.constData(), "absolute+keyframes", nullptr};
    mpv_command(h_, a);
    eof_ = false;
}
void MpvController::seekRel(double d) {
    if (!h_) return;
    const QByteArray t = QByteArray::number(d, 'f', 2);
    const char* a[] = {"seek", t.constData(), "relative+keyframes", nullptr};
    mpv_command(h_, a);
}
void MpvController::setVolume(int v) { if (h_) mpv_set_property_string(h_, "volume", QByteArray::number(qBound(0, v, 100)).constData()); }
void MpvController::setMute(bool m) { if (h_) mpv_set_property_string(h_, "mute", m ? "yes" : "no"); }
bool MpvController::muted() const { return prop("mute") == "yes"; }
void MpvController::setSpeed(double s) { if (h_) mpv_set_property_string(h_, "speed", QByteArray::number(s, 'f', 2).constData()); }
void MpvController::setAudioTrack(int id) { if (h_) mpv_set_property_string(h_, "aid", QByteArray::number(id).constData()); }
void MpvController::setSubTrack(int id) { if (h_) mpv_set_property_string(h_, "sid", id > 0 ? QByteArray::number(id).constData() : "no"); }
void MpvController::addSubtitle(const QString& url, const QString& title, const QString& lang) {
    if (!h_) return;
    const QByteArray u = url.toUtf8(), t = title.toUtf8(), l = lang.toUtf8();
    const char* a[] = {"sub-add", u.constData(), "select", t.constData(), l.constData(), nullptr};
    mpv_command(h_, a);
}
void MpvController::setSubDelay(double d) { if (h_) mpv_set_property_string(h_, "sub-delay", QByteArray::number(d, 'f', 2).constData()); }
double MpvController::subDelay() const { return prop("sub-delay").toDouble(); }
void MpvController::setAspect(const QString& a) { setStr("video-aspect-override", a.isEmpty() ? "-1" : a); }
void MpvController::setLoop(bool on) { if (h_) mpv_set_property_string(h_, "loop-file", on ? "inf" : "no"); }

QString MpvController::statsText() const {
    auto p = [&](const char* n) { const QString v = prop(n); return v.isEmpty() ? QStringLiteral("-") : v; };
    const double vb = prop("video-bitrate").toDouble() / 1000.0;
    return QString("VO: %1 | decoder: %2 | hwdec: %3\nSize: %4x%5 | FPS: %6 | dropped: %7 (decoder %8)\nVideo: %9 kb/s | cache: %10 s")
        .arg(p("current-vo"), p("video-codec"), p("hwdec-current"), p("video-params/w"), p("video-params/h"))
        .arg(p("estimated-vf-fps")).arg(p("frame-drop-count")).arg(p("decoder-frame-drop-count")).arg(vb, 0, 'f', 0).arg(p("demuxer-cache-duration"));
}

void MpvController::emitTracks() {
    QVariantList list;
    const int n = prop("track-list/count").toInt();
    for (int i = 0; i < n; ++i) {
        const QByteArray b = "track-list/" + QByteArray::number(i) + "/";
        QVariantMap m;
        m["type"] = prop((b + "type").constData()); m["id"] = prop((b + "id").constData()).toInt();
        m["lang"] = prop((b + "lang").constData()); m["title"] = prop((b + "title").constData());
        m["selected"] = prop((b + "selected").constData()) == "yes";
        list << m;
    }
    emit tracksChanged(list);
}

void MpvController::onWakeup() {
    while (h_) {
        mpv_event* e = mpv_wait_event(h_, 0);
        if (e->event_id == MPV_EVENT_NONE) break;
        switch (e->event_id) {
        case MPV_EVENT_PROPERTY_CHANGE: {
            auto* pr = static_cast<mpv_event_property*>(e->data);
            const QByteArray name = pr->name;
            if (pr->format == MPV_FORMAT_NONE) break;
            if (name == "time-pos") { pos_ = *static_cast<double*>(pr->data); emit positionChanged(pos_, dur_); }
            else if (name == "duration") { dur_ = *static_cast<double*>(pr->data); emit positionChanged(pos_, dur_); }
            else if (name == "pause") emit pauseChanged(*static_cast<int*>(pr->data));
            else if (name == "paused-for-cache") emit bufferingChanged(*static_cast<int*>(pr->data));
            else if (name == "track-list/count") emitTracks();
            else if (name == "eof-reached") { const bool v = *static_cast<int*>(pr->data); if (v && !eof_) { eof_ = true; emit playbackEnded(); } else if (!v) eof_ = false; }
            break;
        }
        case MPV_EVENT_FILE_LOADED:
            if (!pendingAudio_.isEmpty()) {
                const QByteArray au = pendingAudio_.toUtf8();
                const char* a[] = {"audio-add", au.constData(), "select", "audio", nullptr};
                const int r = mpv_command(h_, a);
                if (r < 0) LOGW(QString("audio-add failed: ") + mpv_error_string(r));
                pendingAudio_.clear();
                mpv_set_property_string(h_, "pause", "no");
            }
            emit fileLoaded(); emitTracks(); break;
        case MPV_EVENT_END_FILE: {
            auto* ef = static_cast<mpv_event_end_file*>(e->data);
            if (ef->reason == MPV_END_FILE_REASON_ERROR) emit errorOccurred(QString::fromUtf8(mpv_error_string(ef->error)));
            break;
        }
        case MPV_EVENT_CLIENT_MESSAGE: {
            auto* m = static_cast<mpv_event_client_message*>(e->data);
            if (m->num_args >= 2 && QByteArray(m->args[0]) == "lt") emit inputAction(QString::fromUtf8(m->args[1]));
            break;
        }
        case MPV_EVENT_LOG_MESSAGE: {
            auto* lm = static_cast<mpv_event_log_message*>(e->data);
            const QString text = QString::fromUtf8(lm->text).trimmed();
            LOGW(QString("mpv[%1] %2").arg(lm->prefix, text));
            if (lm->log_level <= MPV_LOG_LEVEL_ERROR && QByteArray(lm->prefix) == "vo")
                emit errorOccurred("Video output (Direct3D) error: " + text);
            break;
        }
        case MPV_EVENT_SHUTDOWN: return;
        default: break;
        }
    }
}
