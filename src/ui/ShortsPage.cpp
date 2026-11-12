#include "ShortsPage.h"
#include "VideoStage.h"
#include "playback/PlaybackSession.h"
#include "services/PlaylistService.h"
#include "services/SubscriptionService.h"
#include "services/AppSettings.h"
#include "youtube/ThumbnailService.h"
#include "utilities/ErrorHandler.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QPushButton>
#include <QComboBox>
#include <QSlider>
#include <QWheelEvent>
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QUrl>

static QToolButton* big(const QString& t, const QString& tip) {
    auto* b = new QToolButton; b->setText(t); b->setToolTip(tip); b->setFixedSize(54, 54);
    b->setStyleSheet("QToolButton{border-radius:27px;background:#262626;font-size:18px;} QToolButton:hover{background:#3a3a3a;}");
    return b;
}

ShortsPage::ShortsPage(AppContext& ctx, QWidget* p) : QWidget(p), ctx_(ctx), ctrl_(ctx) {
    auto* root = new QVBoxLayout(this);
    auto* top = new QHBoxLayout;
    auto* h = new QLabel("Shorts"); h->setObjectName("h1");
    topic_ = new QComboBox;
    topic_->addItems({"Trending", "Funny", "Music", "Gaming", "Animals", "Sports", "Food", "Science"});
    top->addWidget(h); top->addStretch(1); top->addWidget(new QLabel("Topic:")); top->addWidget(topic_);
    root->addLayout(top);

    auto* mid = new QHBoxLayout;
    mid->addStretch(1);
    center_ = new QWidget;
    auto* cv = new QVBoxLayout(center_); cv->setContentsMargins(0, 0, 0, 0); cv->setSpacing(4);
    stage_ = new VideoStage;
    progress_ = new QSlider(Qt::Horizontal); progress_->setFixedHeight(10); progress_->setRange(0, 0);
    channel_ = new QLabel; channel_->setStyleSheet("font-weight:bold;");
    subscribe_ = new QPushButton("Subscribe"); subscribe_->setObjectName("primary");
    auto* chRow = new QHBoxLayout; chRow->addWidget(channel_, 1); chRow->addWidget(subscribe_);
    title_ = new QLabel; title_->setWordWrap(true); title_->setStyleSheet("font-size:14px;");
    desc_ = new QLabel; desc_->setWordWrap(true); desc_->setObjectName("muted");
    status_ = new QLabel; status_->setObjectName("muted"); status_->setWordWrap(true);
    cv->addWidget(stage_, 0, Qt::AlignHCenter); cv->addWidget(progress_); cv->addLayout(chRow); cv->addWidget(title_); cv->addWidget(desc_); cv->addWidget(status_);
    mid->addWidget(center_);

    auto* act = new QVBoxLayout;
    act->addStretch(1);
    prev_ = big("▲", "Previous Short (Up)"); next_ = big("▼", "Next Short (Down)");
    like_ = big("♥", "Like (adds to Favorites)"); comments_ = big("💬", "Comments (opens YouTube)");
    share_ = big("↗", "Copy link"); mute_ = big("🔊", "Mute (M)");
    for (auto* b : {prev_, next_, like_, comments_, share_, mute_}) { act->addWidget(b); act->addSpacing(6); }
    act->addStretch(1);
    mid->addLayout(act);
    mid->addStretch(1);
    root->addLayout(mid, 1);

    connect(&ctrl_, &ShortsController::currentChanged, this, &ShortsPage::onCurrent);
    connect(&ctrl_, &ShortsController::listLoading, this, [this](bool on) { if (on && ctrl_.count() == 0) stage_->showMessage("Loading Shorts…"); });
    connect(&ctrl_, &ShortsController::listError, this, [this](const QString& m) {
        stage_->showMessage(ErrorHandler::friendly(m));
        status_->setText(ErrorHandler::friendly(m));
    });
    connect(topic_, &QComboBox::currentTextChanged, this, [this](const QString& t) { if (active_) { ctx_.session->stop(); ctrl_.start(t == "Trending" ? QString() : t); } });
    connect(prev_, &QToolButton::clicked, this, [this] { applyAction("prev"); });
    connect(next_, &QToolButton::clicked, this, [this] { applyAction("next"); });
    connect(mute_, &QToolButton::clicked, this, [this] { applyAction("mute"); });
    connect(like_, &QToolButton::clicked, this, [this] {
        if (!cur_.isValid()) return;
        const QString n = PlaylistService::favorites();
        if (ctx_.playlists->contains(n, cur_.id)) ctx_.playlists->removeVideo(n, cur_.id); else ctx_.playlists->add(n, cur_);
        refreshButtons();
    });
    connect(comments_, &QToolButton::clicked, this, [this] { if (cur_.isValid()) QDesktopServices::openUrl(QUrl(cur_.url())); });
    connect(share_, &QToolButton::clicked, this, [this] { if (cur_.isValid()) { QApplication::clipboard()->setText(cur_.url()); status_->setText("Link copied to clipboard."); } });
    connect(subscribe_, &QPushButton::clicked, this, [this] {
        if (cur_.channelId.isEmpty()) return;
        if (ctx_.subs->isSubscribed(cur_.channelId)) ctx_.subs->unsubscribe(cur_.channelId); else ctx_.subs->subscribe(cur_.channelId, cur_.channel);
        refreshButtons();
    });
    connect(progress_, &QSlider::sliderMoved, this, [this](int v) { ctx_.session->mpv()->seekAbs(v / 10.0); });
    connect(ctx_.session, &PlaybackSession::position, this, [this](double pos, double dur) {
        if (!active_) return;
        if (dur > 0 && progress_->maximum() != int(dur * 10)) progress_->setRange(0, int(dur * 10));
        if (!progress_->isSliderDown()) progress_->setValue(int(pos * 10));
    });
    connect(ctx_.session, &PlaybackSession::resolvedMeta, this, [this](const ResolvedStream& s) {
        if (!active_ || s.id != cur_.id) return;
        if (!s.channelId.isEmpty()) cur_.channelId = s.channelId;
        desc_->setText(s.description.left(180));
        refreshButtons();
    });
    connect(ctx_.session, &PlaybackSession::failed, this, [this](const QString& m) {
        if (!active_ || !ctx_.session->shortsMode()) return;
        status_->setText("This Short can't be played (" + ErrorHandler::friendly(m) + ") – skipping…");
        QTimer::singleShot(900, this, [this] { if (active_) ctrl_.skipUnavailable(); });
    });
    connect(ctx_.session, &PlaybackSession::ended, this, [this] {
        if (active_ && ctx_.session->shortsMode() && AppSettings::instance().str("shortsEndAction") == "next") applyAction("next");
    });
}
void ShortsPage::relayout() {
    const int h = qMax(260, height() - 215);
    const int w = h * 9 / 16;
    stage_->setFixedSize(w, h);
    center_->setFixedWidth(qMax(w, 300));
}
void ShortsPage::resizeEvent(QResizeEvent* e) { QWidget::resizeEvent(e); relayout(); }

void ShortsPage::onActivated() {
    active_ = true;
    nav_.setCooldown(AppSettings::instance().num("shortsCooldown"));
    ctx_.session->setShortsMode(true);
    ctx_.session->setStage(stage_);
    relayout();
    QTimer::singleShot(0, this, [this] { relayout(); });
    if (ctrl_.count() == 0) { const QString t = topic_->currentText(); ctrl_.start(t == "Trending" ? QString() : t); }
    else if (cur_.isValid()) ctx_.session->play(cur_);
}
void ShortsPage::onDeactivated() {
    active_ = false;
    ctrl_.cancel();
    ctx_.session->stop();
    ctx_.session->setShortsMode(false);
    ctx_.session->setStage(nullptr);
}
void ShortsPage::onCurrent(const VideoInfo& v, int) {
    cur_ = v;
    status_->clear(); desc_->clear(); progress_->setRange(0, 0);
    channel_->setText(v.channel.isEmpty() ? "" : "@" + v.channel);
    title_->setText(v.title);
    refreshButtons();
    if (!active_) return;
    // play() stops the previous Short and cancels its stream request first -> no overlapping audio
    ctx_.session->play(v);
}
void ShortsPage::refreshButtons() {
    const bool liked = cur_.isValid() && ctx_.playlists->contains(PlaylistService::favorites(), cur_.id);
    like_->setStyleSheet(QString("QToolButton{border-radius:27px;background:%1;font-size:18px;}").arg(liked ? "#cc0000" : "#262626"));
    subscribe_->setEnabled(!cur_.channelId.isEmpty());
    subscribe_->setText(ctx_.subs->isSubscribed(cur_.channelId) ? "Subscribed ✓" : "Subscribe");
    mute_->setText(ctx_.session->mpv()->muted() ? "🔇" : "🔊");
}
void ShortsPage::wheelEvent(QWheelEvent* e) {
    const QString a = nav_.fromWheel(e->angleDelta().y());
    if (!a.isEmpty()) applyAction(a);
    e->accept();
}
void ShortsPage::handleAction(const QString& action) {
    const QString a = nav_.fromAction(action);
    if (!a.isEmpty()) applyAction(a);
}
void ShortsPage::applyAction(const QString& a) {
    MpvController* m = ctx_.session->mpv();
    if (a == "next") ctrl_.next();
    else if (a == "prev") ctrl_.prev();
    else if (a == "pause") m->setPause(!m->paused());
    else if (a == "mute") {
        const bool nm = !m->muted(); m->setMute(nm); AppSettings::instance().set("shortsMuted", nm); refreshButtons();
    } else if (a == "exit") emit exitRequested();
}
