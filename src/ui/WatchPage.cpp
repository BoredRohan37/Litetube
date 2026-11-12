#include "WatchPage.h"
#include "VideoStage.h"
#include "PlayerControls.h"
#include "VideoGrid.h"
#include "CommentsWidget.h"
#include "TextUtil.h"
#include "playback/PlaybackSession.h"
#include "services/PlaylistService.h"
#include "services/SubscriptionService.h"
#include "services/HistoryService.h"
#include "services/AppSettings.h"
#include "services/AuthService.h"
#include "youtube/YouTubeApiClient.h"
#include "youtube/ThumbnailService.h"
#include "youtube/CommentsService.h"
#include "utilities/ErrorHandler.h"
#include "utilities/SysInfo.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>
#include <QMenu>
#include <QApplication>
#include <QClipboard>
#include <QInputDialog>
#include <QDesktopServices>
#include <QUrl>
#include <algorithm>
#include <random>

WatchPage::WatchPage(AppContext& ctx, QWidget* p) : QWidget(p), ctx_(ctx) {
    auto* root = new QHBoxLayout(this);
    root->setContentsMargins(16, 12, 16, 0); root->setSpacing(20);
    auto* left = new QVBoxLayout; left->setSpacing(0); left_ = left;
    stage_ = new VideoStage;
    controls_ = new PlayerControls(ctx.session, false);
    stats_ = new QLabel; stats_->setObjectName("muted"); stats_->setStyleSheet("font-family:Consolas,monospace;font-size:11px;"); stats_->hide();
    left->addWidget(stage_); left->addWidget(controls_); left->addWidget(stats_);

    scroll_ = new QScrollArea; scroll_->setWidgetResizable(true); scroll_->setFrameShape(QFrame::NoFrame); scroll_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    auto* info = new QWidget; auto* iv = new QVBoxLayout(info); iv->setContentsMargins(0, 10, 8, 20); iv->setSpacing(10);
    title_ = new QLabel; title_->setStyleSheet("font-size:19px;font-weight:600;"); title_->setWordWrap(true);
    auto* row = new QHBoxLayout; row->setSpacing(8);
    avatar_ = new QLabel; avatar_->setFixedSize(40, 40); avatar_->setAlignment(Qt::AlignCenter);
    channel_ = new QLabel; channel_->setStyleSheet("font-size:15px;font-weight:600;");
    subscribe_ = new QPushButton("Subscribe"); subscribe_->setObjectName("subscribe"); subscribe_->setCheckable(true);
    like_ = new QPushButton("👍 Like"); like_->setCheckable(true);
    row->setSpacing(6);
    dislike_ = new QPushButton("👎"); dislike_->setCheckable(true);
    share_ = new QPushButton("➦ Share"); save_ = new QPushButton("⊕ Save"); more_ = new QPushButton("↗");
    more_->setToolTip("Open on youtube.com");
    row->addWidget(avatar_); row->addWidget(channel_); row->addWidget(subscribe_); row->addStretch(1);
    for (auto* b : {like_, dislike_, share_, save_, more_}) row->addWidget(b);
    status_ = new QLabel; status_->setObjectName("muted"); status_->setWordWrap(true);
    auto* descBox = new QFrame; descBox->setObjectName("card");
    auto* dl = new QVBoxLayout(descBox);
    desc_ = new QLabel; desc_->setWordWrap(true); desc_->setTextFormat(Qt::RichText); desc_->setTextInteractionFlags(Qt::TextBrowserInteraction);
    auto* toggle = new QPushButton("Show more"); toggle->setObjectName("flat");
    dl->addWidget(desc_); dl->addWidget(toggle, 0, Qt::AlignLeft);
    comments_ = new CommentsWidget(ctx.comments, ctx.account);
    iv->addWidget(title_); iv->addLayout(row); iv->addWidget(status_); iv->addWidget(descBox); iv->addWidget(comments_);
    iv->addStretch(1);
    scroll_->setWidget(info);
    left->addWidget(scroll_, 1);
    root->addLayout(left, 1);

    side_ = new QWidget; side_->setFixedWidth(360);
    auto* sv = new QVBoxLayout(side_); sv->setContentsMargins(0, 0, 0, 0);
    auto* rl = new QLabel("Related videos"); rl->setObjectName("h2");
    related_ = new VideoGrid(ctx, VideoGrid::Rows);
    sv->addWidget(rl); sv->addWidget(related_, 1);
    root->addWidget(side_);

    connect(controls_, &PlayerControls::requestCompact, this, &WatchPage::toggleMini);
    connect(controls_, &PlayerControls::toggleFullscreen, this, &WatchPage::toggleFullscreen);
    connect(controls_, &PlayerControls::nextRequested, this, [this] { playNextInQueue(false); });
    connect(controls_, &PlayerControls::statsToggled, this, [this](bool on) { stats_->setVisible(on); });
    statsTimer_ = new QTimer(this);
    connect(statsTimer_, &QTimer::timeout, this, [this] {
        if (!stats_->isVisible() || !mine()) return;
        stats_->setText(ctx_.session->statsText() + QString("\nApp RAM: %1 MB | CPU: %2%").arg(SysInfo::workingSetMB(), 0, 'f', 0).arg(SysInfo::cpuPercent(), 0, 'f', 0));
    });
    statsTimer_->start(1000);
    connect(toggle, &QPushButton::clicked, this, [this, toggle] { descOpen_ = !descOpen_; toggle->setText(descOpen_ ? "Show less" : "Show more"); setDescription(descFull_); });
    connect(desc_, &QLabel::linkActivated, this, [this](const QString& l) { if (l.startsWith("seek:")) ctx_.session->mpv()->seekAbs(l.mid(5).toDouble()); });
    connect(comments_, &CommentsWidget::seekRequested, this, [this](double s) { ctx_.session->mpv()->seekAbs(s); });
    connect(related_, &VideoGrid::playRequested, this, [this](const VideoInfo& v) { openVideo(v); });
    connect(ctx_.session, &PlaybackSession::loading, this, &WatchPage::onLoading);
    connect(ctx_.session, &PlaybackSession::resolvedMeta, this, &WatchPage::onMeta);
    connect(ctx_.session, &PlaybackSession::failed, this, &WatchPage::onFailed);
    connect(ctx_.session, &PlaybackSession::ended, this, &WatchPage::onEnded);
    connect(ctx_.api, &YouTubeApiClient::finished, this, &WatchPage::onSearchDone);
    connect(ctx_.playlists, &PlaylistService::changed, this, &WatchPage::refreshButtons);
    connect(ctx_.subs, &SubscriptionService::changed, this, &WatchPage::refreshButtons);
    connect(ctx_.auth, &AuthService::stateChanged, this, [this] { remotePlaylists_.clear(); refreshButtons(); });

    connect(subscribe_, &QPushButton::clicked, this, [this] {
        if (channelId_.isEmpty()) return;
        const bool was = ctx_.subs->isSubscribed(channelId_);
        if (was) ctx_.subs->unsubscribe(channelId_); else ctx_.subs->subscribe(channelId_, cur_.channel);
        if (ctx_.account->available()) {
            auto done = [this](const QString& err) { if (!err.isEmpty()) status_->setText("YouTube: " + ErrorHandler::friendly(err)); };
            if (was) ctx_.account->unsubscribe(channelId_, done); else ctx_.account->subscribe(channelId_, done);
        }
    });
    connect(like_, &QPushButton::clicked, this, [this] {
        const QString n = PlaylistService::favorites();
        const bool liked = !ctx_.playlists->contains(n, cur_.id);
        if (liked) ctx_.playlists->add(n, cur_); else ctx_.playlists->removeVideo(n, cur_.id);
        disliked_ = false;
        if (ctx_.account->available()) ctx_.account->rate(cur_.id, liked ? "like" : "none", [this](const QString& e) { if (!e.isEmpty()) status_->setText("YouTube: " + ErrorHandler::friendly(e)); });
        refreshButtons();
    });
    connect(dislike_, &QPushButton::clicked, this, [this] {
        if (!ctx_.account->available()) { status_->setText("Sign in (Settings > Account) to dislike videos on YouTube."); refreshButtons(); return; }
        disliked_ = !disliked_;
        ctx_.playlists->removeVideo(PlaylistService::favorites(), cur_.id);
        ctx_.account->rate(cur_.id, disliked_ ? "dislike" : "none", [this](const QString& e) { if (!e.isEmpty()) status_->setText("YouTube: " + ErrorHandler::friendly(e)); });
        refreshButtons();
    });
    connect(share_, &QPushButton::clicked, this, [this] { QApplication::clipboard()->setText(cur_.url()); status_->setText("Link copied to clipboard."); });
    connect(more_, &QPushButton::clicked, this, [this] { QDesktopServices::openUrl(QUrl(cur_.url())); });
    connect(save_, &QPushButton::clicked, this, [this] {
        QMenu m(this);
        auto* pl = ctx_.playlists;
        m.addAction(pl->contains(PlaylistService::watchLater(), cur_.id) ? "✓ Watch Later" : "Watch Later", this, [this, pl] {
            if (pl->contains(PlaylistService::watchLater(), cur_.id)) pl->removeVideo(PlaylistService::watchLater(), cur_.id); else pl->add(PlaylistService::watchLater(), cur_);
        });
        m.addSeparator();
        for (const QString& n : pl->names()) {
            if (pl->isReserved(n)) continue;
            m.addAction((pl->contains(n, cur_.id) ? "✓ " : "") + n, this, [this, pl, n] { if (pl->contains(n, cur_.id)) pl->removeVideo(n, cur_.id); else pl->add(n, cur_); });
        }
        m.addAction("New playlist…", this, [this, pl] {
            bool ok = false; const QString n = QInputDialog::getText(this, "New playlist", "Name:", QLineEdit::Normal, QString(), &ok);
            if (ok && pl->create(n)) pl->add(n.trimmed(), cur_);
        });
        if (ctx_.account->available()) {
            m.addSeparator();
            if (remotePlaylists_.isEmpty()) {
                m.addAction("Load my YouTube playlists…", this, [this] {
                    ctx_.account->playlists([this](const QList<PlaylistInfo>& l, const QString& err) { remotePlaylists_ = l; status_->setText(err.isEmpty() ? "YouTube playlists loaded - press Save again." : ErrorHandler::friendly(err)); });
                });
            }
            for (const PlaylistInfo& pi : remotePlaylists_)
                m.addAction("☁ " + pi.title, this, [this, pi] {
                    ctx_.account->addToPlaylist(pi.id, cur_.id, [this, pi](const QString& e) { status_->setText(e.isEmpty() ? "Added to " + pi.title : ErrorHandler::friendly(e)); });
                });
        }
        m.exec(save_->mapToGlobal(QPoint(0, save_->height())));
    });
    refreshButtons();
}
bool WatchPage::mine() const { return ctx_.session->stage() == stage_ && stage_->isVisible(); }

void WatchPage::layoutStage() {
    if (fs_) {   // fullscreen / mini: the video takes all the space above the controls
        stage_->setMinimumHeight(112); stage_->setMaximumHeight(QWIDGETSIZE_MAX);
        stage_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        left_->setStretchFactor(stage_, 1); left_->invalidate();
        return;
    }
    left_->setStretchFactor(stage_, 0);
    const int w = stage_->width() > 0 ? stage_->width() : width() - 430;
    const int h = qBound(180, w * 9 / 16, int(height() * 0.62));
    stage_->setFixedHeight(h);
}
void WatchPage::resizeEvent(QResizeEvent* e) { QWidget::resizeEvent(e); layoutStage(); }

void WatchPage::refreshButtons() {
    const bool has = cur_.isValid();
    for (auto* b : {like_, dislike_, share_, save_, more_}) b->setEnabled(has);
    subscribe_->setEnabled(!channelId_.isEmpty());
    const bool subd = ctx_.subs->isSubscribed(channelId_);
    subscribe_->setChecked(subd); subscribe_->setText(subd ? "Subscribed ✓" : "Subscribe");
    const bool liked = has && ctx_.playlists->contains(PlaylistService::favorites(), cur_.id);
    like_->setChecked(liked); like_->setText(liked ? "👍 Liked" : "👍 Like");
    dislike_->setChecked(disliked_);
}
void WatchPage::onActivated() {
    ctx_.session->setShortsMode(false);
    ctx_.session->setStage(stage_);
    stats_->setVisible(AppSettings::instance().flag("showStats"));
    layoutStage();
}
void WatchPage::onDeactivated(bool keepPlaying) { if (!keepPlaying && mine()) ctx_.session->stop(); }
void WatchPage::setFullscreenLayout(bool fs) {
    fs_ = fs;
    scroll_->setVisible(!fs); side_->setVisible(!fs);
    layout()->setContentsMargins(fs ? 0 : 16, fs ? 0 : 12, fs ? 0 : 16, 0);
    layoutStage();
}
void WatchPage::openVideo(const VideoInfo& v, const QList<VideoInfo>& context) {
    queue_.clear(); qIdx_ = -1; repeat_ = false;
    for (int i = 0; i < context.size(); ++i) if (context[i].id == v.id) { queue_ = context; qIdx_ = i; break; }
    cur_ = v;
    playCurrent();
}
void WatchPage::playQueue(const QList<VideoInfo>& list, int index, bool shuffle, bool repeat) {
    if (list.isEmpty()) return;
    queue_ = list; repeat_ = repeat; qIdx_ = qBound(0, index, list.size() - 1);
    if (shuffle) {
        std::mt19937 rng(std::random_device{}());
        const VideoInfo first = queue_.takeAt(qIdx_);
        std::shuffle(queue_.begin(), queue_.end(), rng);
        queue_.prepend(first); qIdx_ = 0;
    }
    cur_ = queue_[qIdx_];
    playCurrent();
}
void WatchPage::setDescription(const QString& text) {
    descFull_ = text;
    QString t = text;
    if (!descOpen_ && t.size() > 240) t = t.left(240) + "…";
    desc_->setText(text.isEmpty() ? QString("<span style='color:#aaa'>No description.</span>") : TextUtil::linkify(t));
}
void WatchPage::playCurrent() {
    ctx_.session->setShortsMode(false);
    ctx_.session->setStage(stage_);
    channelId_ = cur_.channelId; disliked_ = false;
    title_->setText(cur_.title); channel_->setText(cur_.channel); status_->clear();
    avatar_->setText(cur_.channel.left(1).toUpper());
    avatar_->setStyleSheet(QString("background:%1;border-radius:20px;font-size:17px;font-weight:600;color:white;").arg(QColor::fromHsv(qAbs(qHash(cur_.channel)) % 360, 120, 150).name()));
    descOpen_ = false; setDescription(QString());
    controls_->setNextVisible(queue_.size() > 1);
    refreshButtons();
    const double resume = (AppSettings::instance().flag("resumePlayback") && !cur_.live) ? ctx_.history->position(cur_.id) : 0;
    ctx_.session->play(cur_, resume > 5 ? resume - 2 : 0);
    if (relReq_ >= 0) ctx_.api->cancel(relReq_);
    related_->clear();
    relReq_ = ctx_.api->search(cur_.title.left(60), YouTubeApiClient::Videos);
    comments_->setVideo(cur_.id);
}
void WatchPage::onLoading(const VideoInfo& v) { if (mine() && v.id == cur_.id) status_->setText("Loading…"); }
void WatchPage::onMeta(const ResolvedStream& s) {
    if (!mine() || s.id != cur_.id) return;
    if (!s.title.isEmpty()) { title_->setText(s.title); cur_.title = s.title; }
    if (!s.channel.isEmpty()) { channel_->setText(s.channel); cur_.channel = s.channel; avatar_->setText(s.channel.left(1).toUpper()); }
    if (!s.channelId.isEmpty()) { channelId_ = s.channelId; cur_.channelId = s.channelId; }
    if (cur_.seconds == 0) cur_.seconds = s.duration;
    cur_.live = s.live;
    setDescription(s.description);
    status_->clear();
    refreshButtons();
}
void WatchPage::onFailed(const QString& msg) {
    if (!mine()) return;
    status_->setText(ErrorHandler::friendly(msg));
    QTimer::singleShot(0, this, [this, msg] { if (ErrorHandler::showRetry(this, "Playback Error", msg)) ctx_.session->retry(); });
}
void WatchPage::onEnded() { if (mine()) playNextInQueue(true); }
void WatchPage::playNextInQueue(bool fromEnd) {
    if (!queue_.isEmpty()) {
        if (qIdx_ + 1 < queue_.size()) ++qIdx_;
        else if (repeat_) qIdx_ = 0;
        else if (fromEnd) qIdx_ = -2;
        else return;
        if (qIdx_ >= 0) { cur_ = queue_[qIdx_]; playCurrent(); return; }
        qIdx_ = queue_.size() - 1;
    }
    if (fromEnd && AppSettings::instance().flag("autoplay") && related_->count() > 0) {
        queue_.clear(); qIdx_ = -1;
        cur_ = related_->videoAt(0); playCurrent();
    }
}
void WatchPage::onSearchDone(int id, const QList<VideoInfo>& items, const QString&) {
    if (id != relReq_) return;
    relReq_ = -1;
    QList<VideoInfo> f;
    for (const VideoInfo& v : items) if (v.id != cur_.id) f << v;
    related_->setVideos(f);
}
