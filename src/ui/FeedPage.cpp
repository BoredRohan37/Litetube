#include "FeedPage.h"
#include "VideoGrid.h"
#include "youtube/YouTubeApiClient.h"
#include "utilities/ErrorHandler.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QButtonGroup>

FeedPage::FeedPage(AppContext& ctx, const QStringList& chips, bool rows, QWidget* p) : QWidget(p), ctx_(ctx) {
    auto* v = new QVBoxLayout(this);
    v->setContentsMargins(10, 8, 10, 0);
    chipLay_ = new QHBoxLayout; chipLay_->setSpacing(6);
    v->addLayout(chipLay_);
    status_ = new QLabel; status_->setObjectName("muted"); status_->setAlignment(Qt::AlignCenter); status_->setWordWrap(true); status_->hide();
    retry_ = new QPushButton("Retry"); retry_->setObjectName("primary"); retry_->hide();
    grid_ = new VideoGrid(ctx, rows ? VideoGrid::Rows : VideoGrid::Cards);
    v->addWidget(status_); v->addWidget(retry_, 0, Qt::AlignHCenter); v->addWidget(grid_, 1);
    group_ = new QButtonGroup(this); group_->setExclusive(true);
    setChips(chips);
    connect(retry_, &QPushButton::clicked, this, [this] { reload(); });
    connect(grid_, &VideoGrid::playRequested, this, [this](const VideoInfo& vi) { emit playRequested(vi, grid_->videos()); });
    connect(grid_, &VideoGrid::loadMoreRequested, this, [this] { if (req_ < 0 && !token_.isEmpty()) loadPage(true); });
    connect(ctx_.api, &YouTubeApiClient::finished, this, &FeedPage::onFinished);
    connect(ctx_.api, &YouTubeApiClient::failed, this, &FeedPage::onFailed);
}
void FeedPage::setChips(const QStringList& chips) {
    for (QAbstractButton* b : group_->buttons()) { group_->removeButton(b); b->deleteLater(); }
    for (int i = 0; i < chips.size(); ++i) {
        auto* b = new QPushButton(chips[i]); b->setObjectName("chip"); b->setCheckable(true); b->setChecked(i == 0);
        group_->addButton(b, i); chipLay_->insertWidget(i, b);
        connect(b, &QPushButton::clicked, this, [this, i] { chip_ = i; reload(); });
    }
    if (chipLay_->count() == chips.size()) chipLay_->addStretch(1);
    chip_ = 0;
}
void FeedPage::setStatus(const QString& s, bool retry) {
    status_->setText(s); status_->setVisible(!s.isEmpty()); retry_->setVisible(retry);
}
void FeedPage::ensureLoaded() { if (!loaded_) reload(); }
void FeedPage::reload() {
    loaded_ = true;
    if (req_ >= 0) ctx_.api->cancel(req_);
    req_ = -1; token_.clear();
    grid_->clear();
    loadPage(false);
}
void FeedPage::loadPage(bool more) {
    appending_ = more;
    setStatus(more ? QString() : QStringLiteral("Loading…"));
    req_ = startRequest(chip_, more ? token_ : QString());
    if (req_ < 0) setStatus(QString());
}
void FeedPage::onFinished(int id, const QList<VideoInfo>& items, const QString& next) {
    if (id != req_) return;
    req_ = -1; token_ = next;
    if (appending_) grid_->append(items); else grid_->setVideos(items);
    setStatus(grid_->count() ? QString() : QStringLiteral("No results found."), grid_->count() == 0);
}
void FeedPage::onFailed(int id, const QString& msg) {
    if (id != req_) return;
    req_ = -1;
    setStatus(ErrorHandler::friendly(msg), true);
}
