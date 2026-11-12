#include "SubscriptionsPage.h"
#include "VideoGrid.h"
#include "services/SubscriptionService.h"
#include "youtube/YouTubeApiClient.h"
#include "utilities/ErrorHandler.h"
#include "services/AuthService.h"
#include "services/AppSettings.h"
#include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

SubscriptionsPage::SubscriptionsPage(AppContext& ctx, QWidget* p) : QWidget(p), ctx_(ctx) {
    auto* h = new QHBoxLayout(this);
    auto* left = new QVBoxLayout;
    auto* t = new QLabel("Subscriptions"); t->setObjectName("h1");
    auto* note = new QLabel("Signed out: subscriptions are stored on this PC only. Sign in (Settings > Account) to use your YouTube subscriptions."); note->setObjectName("muted"); note->setWordWrap(true);
    channels_ = new QListWidget; channels_->setFixedWidth(220);
    unsub_ = new QPushButton("Unsubscribe");
    left->addWidget(t); left->addWidget(note); left->addWidget(channels_, 1); left->addWidget(unsub_);
    auto* right = new QVBoxLayout;
    status_ = new QLabel; status_->setObjectName("muted"); status_->setWordWrap(true);
    grid_ = new VideoGrid(ctx, VideoGrid::Cards);
    right->addWidget(status_); right->addWidget(grid_, 1);
    h->addLayout(left); h->addLayout(right, 1);
    connect(ctx_.subs, &SubscriptionService::changed, this, &SubscriptionsPage::refresh);
    connect(channels_, &QListWidget::currentRowChanged, this, [this](int) { loadChannel(); });
    connect(unsub_, &QPushButton::clicked, this, [this] {
        if (auto* it = channels_->currentItem()) ctx_.subs->unsubscribe(it->data(Qt::UserRole).toString());
    });
    connect(grid_, &VideoGrid::playRequested, this, [this](const VideoInfo& v) { emit playRequested(v, grid_->videos()); });
    connect(ctx_.api, &YouTubeApiClient::finished, this, &SubscriptionsPage::onFinished);
    connect(ctx_.api, &YouTubeApiClient::failed, this, &SubscriptionsPage::onFailed);
    connect(ctx_.auth, &AuthService::stateChanged, this, &SubscriptionsPage::refresh);
    refresh();
}
void SubscriptionsPage::refresh() {
    acct_ = ctx_.account->available();
    if (acct_) { loadAccount(); return; }
    const QString cur = channels_->currentItem() ? channels_->currentItem()->data(Qt::UserRole).toString() : QString();
    channels_->blockSignals(true);
    channels_->clear();
    int sel = -1;
    for (const auto& s : ctx_.subs->list()) {
        auto* it = new QListWidgetItem(s.second.isEmpty() ? s.first : s.second, channels_);
        it->setData(Qt::UserRole, s.first);
        if (s.first == cur) sel = channels_->count() - 1;
    }
    channels_->blockSignals(false);
    unsub_->setEnabled(channels_->count() > 0);
    if (channels_->count() == 0) { grid_->clear(); status_->setText("No subscriptions yet. Use Subscribe on a Watch page, a Short, or the right-click menu of any video."); return; }
    channels_->setCurrentRow(sel >= 0 ? sel : 0);
    if (sel >= 0) return;
    loadChannel();
}
void SubscriptionsPage::loadAccount() {
    channels_->blockSignals(true); channels_->clear();
    auto* all = new QListWidgetItem("★  All subscriptions", channels_); all->setData(Qt::UserRole, QString());
    channels_->blockSignals(false);
    unsub_->setEnabled(false);
    grid_->clear(); status_->setText("Loading your YouTube subscriptions…");
    ctx_.account->subscriptions([this](const QList<ChannelInfo>& l, const QString& err) {
        if (!err.isEmpty()) { status_->setText(ErrorHandler::friendly(err)); return; }
        acctChannels_ = l;
        channels_->blockSignals(true);
        for (const ChannelInfo& c : l) { auto* it = new QListWidgetItem(c.title, channels_); it->setData(Qt::UserRole, c.id); }
        channels_->setCurrentRow(0);
        channels_->blockSignals(false);
        status_->setText("Loading latest uploads…");
        ctx_.account->subscriptionFeed(l, [this](const QList<VideoInfo>& v, const QString& e) {
            if (channels_->currentRow() > 0) return;
            grid_->setVideos(v);
            status_->setText(v.isEmpty() ? (e.isEmpty() ? QStringLiteral("No recent uploads.") : ErrorHandler::friendly(e)) : QString());
        });
    });
}
void SubscriptionsPage::loadChannel() {
    if (acct_ && channels_->currentRow() == 0) { loadAccount(); return; }
    if (req_ >= 0) ctx_.api->cancel(req_);
    req_ = -1;
    auto* it = channels_->currentItem();
    if (!it) return;
    grid_->clear();
    status_->setText("Loading recent uploads…");
    req_ = ctx_.api->channelVideos(it->data(Qt::UserRole).toString());
}
void SubscriptionsPage::onFinished(int id, const QList<VideoInfo>& items, const QString&) {
    if (id != req_) return;
    req_ = -1; grid_->setVideos(items);
    status_->setText(items.isEmpty() ? "No recent uploads found." : QString());
}
void SubscriptionsPage::onFailed(int id, const QString& msg) {
    if (id != req_) return;
    req_ = -1; status_->setText(ErrorHandler::friendly(msg));
}
