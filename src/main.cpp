#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QTimer>
#include <QSaveFile>
#include <QSslSocket>
#include "ui/MainWindow.h"
#include "ui/Theme.h"
#include "playback/PlaybackSession.h"
#include "services/AppSettings.h"
#include "services/PlaylistService.h"
#include "services/HistoryService.h"
#include "services/SubscriptionService.h"
#include "services/CacheManager.h"
#include "youtube/ThumbnailService.h"
#include "youtube/YouTubeApiClient.h"
#include "youtube/StreamResolver.h"
#include "youtube/AccountApi.h"
#include "youtube/CommentsService.h"
#include "services/AuthService.h"
#include "utilities/Logger.h"

int runSelfTest(int argc, char** argv);   // tests/selftest.cpp (only when built with -DLITETUBE_SELFTEST)

static QString pickDataDir() {
    const QString portable = QCoreApplication::applicationDirPath() + "/data";
    QDir().mkpath(portable);
    QFile probe(portable + "/.w");
    if (probe.open(QIODevice::WriteOnly)) { probe.remove(); return portable; }
    const QString fallback = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(fallback);
    return fallback;
}

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    app.setApplicationName("LiteTube");
    app.setApplicationVersion(LITETUBE_VERSION);
    app.setStyle("Fusion");
    if (!QSslSocket::supportsSsl()) qWarning("OpenSSL libraries not found: HTTPS (thumbnails, API, sign-in) will not work");
    app.setStyleSheet(Theme::stylesheet());
    app.setWindowIcon(Theme::appIcon());

    QString dir = pickDataDir();
    for (int i = 1; i + 1 < argc; ++i) if (QByteArray(argv[i]) == "--data-dir") { dir = argv[i + 1]; QDir().mkpath(dir); }
    AppSettings& st = AppSettings::instance();
    st.init(dir);
    Logger::init(dir + "/logs", st.flag("logging"));
    LOGI("LiteTube " LITETUBE_VERSION " starting");
    // make the yt-dlp next to the exe the default if present
    const QString local = QCoreApplication::applicationDirPath() + "/yt-dlp.exe";
    if (st.str("ytdlpPath") == "yt-dlp" && QFileInfo::exists(local)) st.set("ytdlpPath", local);

    qRegisterMetaType<VideoInfo>("VideoInfo");
    qRegisterMetaType<ResolvedStream>("ResolvedStream");
    qRegisterMetaType<QList<VideoInfo>>("QList<VideoInfo>");

    CacheManager cache(dir + "/thumbs");
    cache.trim();
    ThumbnailService thumbs(&cache);
    PlaylistService playlists(dir + "/playlists.json");
    HistoryService history(dir + "/history.json");
    SubscriptionService subs(dir + "/subscriptions.json");
    YouTubeApiClient api;
    StreamResolver resolver;
    AppContext ctx;
    ctx.cache = &cache; ctx.thumbs = &thumbs; ctx.playlists = &playlists; ctx.history = &history; ctx.subs = &subs;
    ctx.api = &api; ctx.resolver = &resolver;
    AuthService auth;
    AccountApi account(&auth);
    CommentsService comments(&account);
    ctx.auth = &auth; ctx.account = &account; ctx.comments = &comments;
    PlaybackSession session(ctx);
    ctx.session = &session;

#ifdef LITETUBE_SELFTEST
    if (app.arguments().contains("--selftest")) return runSelfTest(argc, argv);
#endif
    MainWindow w(ctx);
    w.show();
    const int rc = app.exec();
    playlists.flush(); history.flush(); subs.flush();
    return rc;
}
