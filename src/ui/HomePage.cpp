#include "HomePage.h"
#include "youtube/YouTubeApiClient.h"
#include "services/HistoryService.h"
#include "services/AppSettings.h"

static QStringList chipsFor(HomePage::Mode m) {
    return m == HomePage::Home ? QStringList{"All", "Music", "Live", "Gaming", "Mixes", "Action", "Comedy"}
                               : QStringList{"Now", "Music", "Gaming", "Sports", "News", "Movies"};
}
HomePage::HomePage(AppContext& ctx, Mode m, QWidget* p) : FeedPage(ctx, chipsFor(m), false, p), mode_(m), chips_(chipsFor(m)) {}

int HomePage::startRequest(int chip, const QString& token) {
    const QString name = chips_.value(chip);
    if (mode_ == Trending) return ctx_.api->trending(name, token);
    if (chip == 0) {
        // "All": most popular; with watch history present, lean on the last channel watched instead (no account personalisation exists in the public API)
        if (!AppSettings::instance().str("cookiesBrowser").isEmpty() && token.isEmpty()) return ctx_.api->cookieFeed("ytrec");   // real personalised Home
        const auto h = ctx_.history->watch();
        if (!h.isEmpty() && !h.first().v.channel.isEmpty() && token.isEmpty() && !ctx_.api->hasApiKey())
            return ctx_.api->search(h.first().v.channel, YouTubeApiClient::Videos, token);
        return ctx_.api->trending("Now", token);
    }
    if (name == "Live") return ctx_.api->search("live", YouTubeApiClient::Live, token);
    if (name == "Mixes") return ctx_.api->search("mix playlist music", YouTubeApiClient::Videos, token);
    return ctx_.api->search(name, YouTubeApiClient::Videos, token);
}
