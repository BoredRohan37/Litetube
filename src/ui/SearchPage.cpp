#include "SearchPage.h"
#include "youtube/YouTubeApiClient.h"
#include "services/HistoryService.h"

SearchPage::SearchPage(AppContext& ctx, QWidget* p) : FeedPage(ctx, {"All", "Shorts", "Videos", "Live"}, true, p) {}
void SearchPage::search(const QString& q) {
    q_ = q.trimmed();
    if (q_.isEmpty()) return;
    ctx_.history->addSearch(q_);
    reload();
}
int SearchPage::startRequest(int chip, const QString& token) {
    if (q_.isEmpty()) return -1;
    static const int map[] = {YouTubeApiClient::All, YouTubeApiClient::Shorts, YouTubeApiClient::Videos, YouTubeApiClient::Live};
    return ctx_.api->search(q_, map[chip], token);
}
