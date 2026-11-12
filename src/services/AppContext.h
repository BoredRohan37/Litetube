#pragma once
class PlaylistService; class HistoryService; class SubscriptionService; class CacheManager;
class ThumbnailService; class AuthService; class AccountApi; class CommentsService; class YouTubeApiClient; class StreamResolver; class PlaybackSession;
struct AppContext {
    PlaylistService* playlists = nullptr;
    HistoryService* history = nullptr;
    SubscriptionService* subs = nullptr;
    CacheManager* cache = nullptr;
    ThumbnailService* thumbs = nullptr;
    YouTubeApiClient* api = nullptr;
    StreamResolver* resolver = nullptr;
    PlaybackSession* session = nullptr;
    AuthService* auth = nullptr;
    AccountApi* account = nullptr;
    CommentsService* comments = nullptr;
};
