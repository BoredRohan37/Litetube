#include "ShortsController.h"
#include "youtube/YouTubeApiClient.h"
#include "youtube/ThumbnailService.h"

ShortsController::ShortsController(AppContext& ctx, QObject* p) : QObject(p), ctx_(ctx) {
    connect(ctx_.api, &YouTubeApiClient::finished, this, &ShortsController::onFinished);
    connect(ctx_.api, &YouTubeApiClient::failed, this, &ShortsController::onFailed);
}
void ShortsController::cancel() {
    if (req_ >= 0) { ctx_.api->cancel(req_); req_ = -1; emit listLoading(false); }
}
void ShortsController::start(const QString& topic) {
    cancel();
    list_.clear(); seen_.clear(); token_.clear(); idx_ = -1; wantNext_ = false; skipRun_ = 0;
    topic_ = topic;
    loadMore();
}
void ShortsController::loadMore() {
    if (req_ >= 0) return;
    req_ = ctx_.api->shorts(topic_, token_);
    emit listLoading(true);
}
void ShortsController::onFinished(int id, const QList<VideoInfo>& items, const QString& token) {
    if (id != req_) return;
    req_ = -1; emit listLoading(false);
    token_ = token;
    for (const VideoInfo& v : items) if (!seen_.contains(v.id)) { seen_.insert(v.id); list_ << v; }
    if (list_.isEmpty()) { emit listError("No Shorts found for this topic."); return; }
    if (idx_ < 0) goTo(0);
    else if (wantNext_ && idx_ + 1 < list_.size()) { wantNext_ = false; goTo(idx_ + 1); }
}
void ShortsController::onFailed(int id, const QString& msg) {
    if (id != req_) return;
    req_ = -1; emit listLoading(false);
    emit listError(msg);
}
void ShortsController::goTo(int i) {
    idx_ = i;
    emit currentChanged(list_[i], i);
    if (i + 1 < list_.size()) ctx_.thumbs->request(list_[i + 1].thumbUrl());   // only the next item's thumbnail
    if (i >= list_.size() - 3 && !token_.isEmpty()) loadMore();
}
bool ShortsController::next() {
    if (idx_ + 1 < list_.size()) { goTo(idx_ + 1); return true; }
    if (!token_.isEmpty() || req_ >= 0) { wantNext_ = true; loadMore(); }
    return false;
}
bool ShortsController::prev() {
    if (idx_ > 0) { goTo(idx_ - 1); return true; }
    return false;
}
void ShortsController::skipUnavailable() {
    if (++skipRun_ > 3) { skipRun_ = 0; emit listError("Several Shorts in a row could not be played. Check your connection and try again."); return; }
    next();
}
