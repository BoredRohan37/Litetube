#pragma once
#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
#include <QPointer>
#include "MpvController.h"
#include "VideoWidget.h"
#include "services/AppContext.h"
#include "youtube/VideoInfo.h"
#include "youtube/StreamResolver.h"
class VideoStage;

class PlaybackSession : public QObject {
    Q_OBJECT
public:
    enum State { Idle, Resolving, Loading, Playing, Failed };
    PlaybackSession(AppContext& ctx, QObject* p = nullptr);
    ~PlaybackSession() override;

    VideoWidget* widget() const { return video_; }
    MpvController* mpv() { return &mpv_; }
    void setHost(QWidget* host);       // window the floating video widget lives in
    void setStage(VideoStage* stage);  // placeholder the video covers (null = none)
    void shutdown();               // release mpv before the windows are destroyed
    VideoStage* stage() const { return stage_; }

    void play(const VideoInfo& v, double start = 0);
    void stop();
    void retry();
    void setQuality(int height);
    int quality() const { return quality_; }
    int currentHeight() const { return curHeight_; }
    void setShortsMode(bool on) { shorts_ = on; }
    bool shortsMode() const { return shorts_; }
    void selectSubtitle(int index);   // -1 = off
    void shiftSubtitles(double delta);
    const VideoInfo& current() const { return cur_; }
    const ResolvedStream& resolved() const { return res_; }
    bool hasResolved() const { return !res_.id.isEmpty(); }
    State state() const { return state_; }
    QVariantList tracks() const { return tracks_; }
    QString statsText();

signals:
    void loading(const VideoInfo& v);
    void resolvedMeta(const ResolvedStream& s);
    void started();
    void failed(const QString& message);
    void ended();
    void position(double pos, double dur);
    void buffering(bool on);
    void tracksChanged(const QVariantList& tracks);
    void qualitiesChanged();
    void inputAction(const QString& action);

private slots:
    void onResolved(int id, const ResolvedStream& s);
    void onResolveFailed(int id, const QString& msg);
    void onMpvError(const QString& msg);
    void onFileLoaded();
private:
    bool ensureInit();
    void sync();
    void startLoad(double pos);
    void fail(const QString& msg);
    AppContext& ctx_;
    MpvController mpv_;
    QPointer<VideoWidget> video_;
    VideoStage* stage_ = nullptr;
    quintptr wid_ = 0;
    VideoInfo cur_;
    ResolvedStream res_;
    State state_ = Idle;
    int req_ = -1, quality_ = 480, curHeight_ = 0;
    double startPos_ = 0;
    bool shorts_ = false, retriedExpired_ = false, initFailed_ = false;
    QTimer loadTimeout_, bufTimeout_, saveTimer_;
    QElapsedTimer posThrottle_;
    QVariantList tracks_;
};
