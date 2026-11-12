#pragma once
#include <QWidget>
#include "services/AppContext.h"
#include "youtube/VideoInfo.h"
#include "youtube/StreamResolver.h"
#include "youtube/AccountApi.h"
class VideoStage; class PlayerControls; class VideoGrid; class QLabel; class QPushButton; class QScrollArea; class CommentsWidget;

class WatchPage : public QWidget {
    Q_OBJECT
public:
    explicit WatchPage(AppContext& ctx, QWidget* p = nullptr);
    void openVideo(const VideoInfo& v, const QList<VideoInfo>& context = {});
    void playQueue(const QList<VideoInfo>& list, int index, bool shuffle, bool repeat);
    void setFullscreenLayout(bool fs);
    void onActivated();
    void onDeactivated(bool keepPlaying);
    VideoStage* stage() const { return stage_; }
signals:
    void toggleMini();
    void toggleFullscreen();
protected:
    void resizeEvent(QResizeEvent* e) override;
private slots:
    void onLoading(const VideoInfo& v);
    void onMeta(const ResolvedStream& s);
    void onFailed(const QString& msg);
    void onEnded();
    void onSearchDone(int id, const QList<VideoInfo>& items, const QString& next);
private:
    bool mine() const;
    void playCurrent();
    void refreshButtons();
    void layoutStage();
    void playNextInQueue(bool fromEnd);
    void setDescription(const QString& text);
    AppContext& ctx_;
    class QVBoxLayout* left_ = nullptr;
    VideoStage* stage_;
    PlayerControls* controls_;
    QScrollArea* scroll_;
    QWidget *side_;
    QLabel *title_, *channel_, *status_, *desc_, *stats_, *avatar_;
    QPushButton *subscribe_, *like_, *dislike_, *share_, *save_, *more_;
    CommentsWidget* comments_;
    VideoGrid* related_;
    QTimer* statsTimer_;
    QList<VideoInfo> queue_;
    QList<PlaylistInfo> remotePlaylists_;
    int qIdx_ = -1, relReq_ = -1;
    bool repeat_ = false, fs_ = false, descOpen_ = false, disliked_ = false;
    VideoInfo cur_;
    QString channelId_, descFull_;
};
