#include "PlaybackSession.h"
#include "ui/VideoStage.h"
#include <QApplication>
#include "services/AppSettings.h"
#include "services/HistoryService.h"
#include "services/HistoryService.h"
#include "youtube/ThumbnailService.h"
#include "utilities/Logger.h"

PlaybackSession::PlaybackSession(AppContext& ctx, QObject* p) : QObject(p), ctx_(ctx), mpv_(this) {
    video_ = new VideoWidget();
    quality_ = AppSettings::instance().num("quality");
    loadTimeout_.setSingleShot(true); bufTimeout_.setSingleShot(true);
    connect(&loadTimeout_, &QTimer::timeout, this, [this] { if (state_ == Loading) { mpv_.stop(); fail("Timed out while starting playback"); } });
    connect(&bufTimeout_, &QTimer::timeout, this, [this] { if (state_ == Playing) { mpv_.stop(); fail("Buffering timed out"); } });
    saveTimer_.setInterval(15000);
    connect(&saveTimer_, &QTimer::timeout, this, [this] {
        if (state_ == Playing && !shorts_ && ctx_.history && mpv_.duration() > 30) ctx_.history->updatePosition(cur_.id, mpv_.position());
    });
    saveTimer_.start();
    connect(ctx_.resolver, &StreamResolver::resolved, this, &PlaybackSession::onResolved);
    connect(ctx_.resolver, &StreamResolver::failed, this, &PlaybackSession::onResolveFailed);
    connect(&mpv_, &MpvController::errorOccurred, this, &PlaybackSession::onMpvError);
    connect(&mpv_, &MpvController::fileLoaded, this, &PlaybackSession::onFileLoaded);
    connect(&mpv_, &MpvController::inputAction, this, &PlaybackSession::inputAction);
    connect(&mpv_, &MpvController::bufferingChanged, this, [this](bool b) {
        emit buffering(b);
        if (b) bufTimeout_.start(AppSettings::instance().num("bufferTimeout") * 1000); else bufTimeout_.stop();
    });
    connect(&mpv_, &MpvController::tracksChanged, this, [this](const QVariantList& t) { tracks_ = t; emit tracksChanged(t); });
    connect(&mpv_, &MpvController::positionChanged, this, [this](double pos, double dur) {
        if (posThrottle_.isValid() && posThrottle_.elapsed() < 250) return;
        posThrottle_.restart();
        emit position(pos, dur);
    });
    connect(&mpv_, &MpvController::playbackEnded, this, [this] {
        if (state_ != Playing) return;
        if (ctx_.history && !shorts_) ctx_.history->updatePosition(cur_.id, 0);
        emit ended();
    });
}
PlaybackSession::~PlaybackSession() { mpv_.shutdown(); if (video_ && !video_->parentWidget()) delete video_; }
void PlaybackSession::shutdown() { stop(); mpv_.shutdown(); }

void PlaybackSession::setHost(QWidget* host) { video_->setParent(host); video_->hide(); }
void PlaybackSession::setStage(VideoStage* st) {
    if (st == stage_) { sync(); return; }
    if (stage_) disconnect(stage_, nullptr, this, nullptr);
    stage_ = st;
    if (stage_) connect(stage_, &VideoStage::geometryChanged, this, &PlaybackSession::sync);
    sync();
}
void PlaybackSession::sync() {
    if (!video_ || !video_->parentWidget()) return;
    if (!stage_ || !stage_->isVisible() || !stage_->wantsVideo() || stage_->width() < 8) { video_->hide(); return; }
    video_->setGeometry(QRect(stage_->mapTo(video_->parentWidget(), QPoint(0, 0)), stage_->size()));
    if (!video_->isVisible()) video_->show();
    video_->raise();
}
bool PlaybackSession::ensureInit() {
    if (mpv_.isReady()) return true;
    // Every false return must already have reported a failure: startLoad() returns
    // early here, so a silent false would leave the session stuck in Loading forever.
    if (!stage_ || !video_->isVisible()) { fail("Video output is not available yet (the player page is not visible)."); return false; }
    QString err;
    if (!mpv_.init(video_->nativeId(), &err)) { fail("Video output failed to start: " + err); return false; }
    return true;
}
void PlaybackSession::fail(const QString& msg) {
    state_ = Failed;
    loadTimeout_.stop(); bufTimeout_.stop();
    LOGW("playback failed: " + msg);
    if (stage_) stage_->showMessage(msg);
    emit failed(msg);
}
void PlaybackSession::stop() {
    // Read position/duration BEFORE mpv_.stop() zeroes them, otherwise resume never persists.
    if (ctx_.history && state_ == Playing && !shorts_ && mpv_.duration() > 30)
        ctx_.history->updatePosition(cur_.id, mpv_.position());
    ctx_.resolver->cancel(req_); req_ = -1;
    mpv_.stop();
    loadTimeout_.stop(); bufTimeout_.stop();
    state_ = Idle;
}
void PlaybackSession::play(const VideoInfo& v, double start) {
    stop();
    cur_ = v; res_ = ResolvedStream(); curHeight_ = 0;
    startPos_ = start; retriedExpired_ = false;
    state_ = Resolving;
    if (stage_) stage_->showMessage("Loading…\n" + v.title, ctx_.thumbs ? ctx_.thumbs->cached(v.thumbUrl()) : QPixmap());
    emit loading(v);
    req_ = ctx_.resolver->resolve(v.id);
}
void PlaybackSession::retry() { if (cur_.isValid()) play(cur_, mpv_.position() > 5 ? mpv_.position() : startPos_); }

void PlaybackSession::onResolved(int id, const ResolvedStream& s) {
    if (id != req_) return;
    res_ = s;
    if (res_.id.isEmpty()) res_.id = cur_.id;
    if (cur_.title.isEmpty()) cur_.title = s.title;
    emit resolvedMeta(res_);
    emit qualitiesChanged();
    startLoad(startPos_);
}
void PlaybackSession::onResolveFailed(int id, const QString& msg) {
    if (id != req_) return;
    fail(msg);
}
void PlaybackSession::startLoad(double pos) {
    QString v, a; int h = 0;
    if (!res_.pick(quality_, v, a, h)) { fail("No playable format found for this video"); return; }
    curHeight_ = h;
    state_ = Loading;
    if (stage_) stage_->showVideo();          // makes the video window visible and sized before mpv opens its output
    if (!ensureInit()) return;
    const bool loop = shorts_ && AppSettings::instance().str("shortsEndAction") == "loop";
    if (!mpv_.load(v, a, pos, loop)) { fail("Could not start playback"); return; }
    loadTimeout_.start(AppSettings::instance().num("bufferTimeout") * 1000);
    emit qualitiesChanged();
}
void PlaybackSession::onFileLoaded() {
    if (state_ != Loading) return;
    state_ = Playing;
    loadTimeout_.stop();
    AppSettings& s = AppSettings::instance();
    mpv_.setSpeed(shorts_ ? 1.0 : s.dbl("speed"));
    if (shorts_) mpv_.setMute(s.flag("shortsMuted"));
    if (ctx_.history) ctx_.history->addWatch(cur_, 0);
    if (s.flag("subsEnabled") && !shorts_) {
        for (int i = 0; i < res_.subs.size(); ++i)
            if (res_.subs[i].lang.startsWith(s.str("subLang"))) { selectSubtitle(i); break; }
    }
    emit started();
}
void PlaybackSession::onMpvError(const QString& msg) {
    if (state_ != Loading && state_ != Playing) return;
    if (msg.startsWith("Video output")) { fail(msg); return; }
    // typical cause: stream URL expired or got rejected -> resolve once more and resume where we were
    if (!retriedExpired_ && hasResolved()) {
        retriedExpired_ = true;
        const double pos = mpv_.position();
        LOGW("mpv error '" + msg + "', re-resolving stream");
        ctx_.resolver->cancel(req_);
        startPos_ = pos;
        state_ = Resolving;
        req_ = ctx_.resolver->resolve(cur_.id);
        return;
    }
    fail(msg);
}
void PlaybackSession::setQuality(int h) {
    quality_ = h;
    if (hasResolved() && (state_ == Playing || state_ == Loading)) startLoad(mpv_.position());
}
void PlaybackSession::selectSubtitle(int idx) {
    if (idx < 0 || idx >= res_.subs.size()) { mpv_.setSubTrack(0); return; }
    const SubTrack& st = res_.subs[idx];
    for (const QVariant& t : tracks_) {
        const QVariantMap m = t.toMap();
        if (m["type"].toString() == "sub" && m["title"].toString() == st.label) { mpv_.setSubTrack(m["id"].toInt()); return; }
    }
    mpv_.addSubtitle(st.url, st.label, st.lang);
}
void PlaybackSession::shiftSubtitles(double d) { mpv_.setSubDelay(d == 0 ? 0 : mpv_.subDelay() + d); }
QString PlaybackSession::statsText() { return mpv_.statsText(); }

