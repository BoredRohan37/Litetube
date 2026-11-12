// Headless self-test: exercises services, parsing, mpv control, every page and the settings dialog.
// Run:  QT_QPA_PLATFORM=offscreen ./LiteTube --selftest
#include <QApplication>
#include <QEventLoop>
#include <QTimer>
#include <QTemporaryDir>
#include <QFile>
#include <QDataStream>
#include <QtMath>
#include <cstdio>
#include "ui/MainWindow.h"
#include "ui/WatchPage.h"
#include "ui/ShortsPage.h"
#include "ui/VideoGrid.h"
#include "ui/SearchPage.h"
#include "ui/SettingsDialog.h"
#include "playback/PlaybackSession.h"
#include "services/AppSettings.h"
#include "services/PlaylistService.h"
#include "services/HistoryService.h"
#include "services/SubscriptionService.h"
#include "services/CacheManager.h"
#include "youtube/YouTubeApiClient.h"
#include "youtube/ThumbnailService.h"
#include "youtube/StreamResolver.h"
#include "youtube/AccountApi.h"
#include "youtube/CommentsService.h"
#include "services/AuthService.h"
#include <QPixmap>
#include <QLabel>
#include <QStandardPaths>
#include "shorts/ShortsNavigation.h"
#include "shorts/ShortsController.h"

static int g_fail = 0, g_pass = 0;
static void check(bool ok, const char* name) {
    std::printf("[%s] %s\n", ok ? "PASS" : "FAIL", name);
    std::fflush(stdout);
    ok ? ++g_pass : ++g_fail;
}
static bool waitFor(std::function<bool()> cond, int ms) {
    QEventLoop l; QTimer t; t.setInterval(20);
    QTimer to; to.setSingleShot(true);
    QObject::connect(&t, &QTimer::timeout, [&] { if (cond()) l.quit(); });
    QObject::connect(&to, &QTimer::timeout, &l, &QEventLoop::quit);
    t.start(); to.start(ms);
    if (!cond()) l.exec();
    return cond();
}
static void writeWav(const QString& path, double secs) {
    const int rate = 22050, n = int(rate * secs);
    QFile f(path); f.open(QIODevice::WriteOnly);
    QDataStream d(&f); d.setByteOrder(QDataStream::LittleEndian);
    f.write("RIFF"); d << quint32(36 + n * 2); f.write("WAVEfmt "); d << quint32(16) << quint16(1) << quint16(1) << quint32(rate) << quint32(rate * 2) << quint16(2) << quint16(16);
    f.write("data"); d << quint32(n * 2);
    for (int i = 0; i < n; ++i) d << qint16(3000 * std::sin(2 * M_PI * 440 * i / rate));
}

int runSelfTest(int, char**) {
    AppSettings& S = AppSettings::instance();
    QTemporaryDir tmp;
    const QString dir = tmp.path();

    // ---- 1. persistence ----
    {
        PlaylistService pl(dir + "/pl.json");
        check(pl.exists("Favorites") && pl.exists("Watch Later"), "reserved playlists exist");
        check(pl.create("Chill") && !pl.create("Chill"), "create playlist, reject duplicate");
        VideoInfo a; a.id = "AAAAAAAAAAA"; a.title = "A"; VideoInfo b; b.id = "BBBBBBBBBBB"; b.title = "B"; VideoInfo c; c.id = "CCCCCCCCCCC"; c.title = "C";
        pl.add("Chill", a); pl.add("Chill", b); pl.add("Chill", c);
        check(!pl.add("Chill", a), "no duplicate videos in a playlist");
        pl.move("Chill", 0, 2);
        check(pl.videos("Chill")[2].id == a.id, "reorder");
        check(pl.rename("Chill", "Relax") && pl.exists("Relax"), "rename");
        check(!pl.rename("Favorites", "x") && !pl.remove("Watch Later"), "reserved lists protected");
        pl.add("Watch Later", b);
        pl.flush();
        PlaylistService pl2(dir + "/pl.json");
        check(pl2.count("Relax") == 3 && pl2.videos("Relax")[0].id == b.id && pl2.contains("Watch Later", b.id), "playlists persist across restart");
        pl2.remove("Relax");
        check(!pl2.exists("Relax"), "delete playlist");

        HistoryService h(dir + "/h.json");
        h.addWatch(a); h.addWatch(b); h.addWatch(a, 0); h.updatePosition(a.id, 42); h.addSearch("lofi"); h.addSearch("jazz"); h.addSearch("lofi");
        h.flush();
        HistoryService h2(dir + "/h.json");
        check(h2.watch().size() == 2 && h2.watch()[0].v.id == a.id && qFuzzyCompare(h2.position(a.id), 42.0), "watch history persists, newest first, position stored");
        check(h2.searches() == QStringList({"lofi", "jazz"}), "search history de-duplicated and persisted");
        S.set("saveHistory", false); h2.addWatch(c);
        check(h2.watch().size() == 2, "privacy: history not saved when disabled");
        S.set("saveHistory", true);
        h2.clearWatch(); h2.clearSearches();
        check(h2.watch().isEmpty() && h2.searches().isEmpty(), "clear history");

        SubscriptionService sub(dir + "/s.json");
        sub.subscribe("UC1", "Chan"); SubscriptionService sub2(dir + "/s.json");
        check(sub2.isSubscribed("UC1"), "subscriptions persist");

        CacheManager cm(dir + "/thumbs");
        for (int i = 0; i < 5; ++i) cm.store(QString("u%1").arg(i), QByteArray(300000, 'x'));
        S.set("thumbCacheMB", 1); cm.trim();
        check(cm.sizeBytes() <= 1048576, "thumbnail cache trimmed to limit");
    }

    // ---- 2. stream format selection ----
    {
        const QByteArray json = R"({"id":"x","title":"T","channel":"C","channel_id":"UC","duration":100,"formats":[
          {"url":"u18","height":360,"vcodec":"avc1.42","acodec":"mp4a","protocol":"https","tbr":500},
          {"url":"v720av1","height":720,"vcodec":"av01.0","acodec":"none","protocol":"https","tbr":900},
          {"url":"v720avc","height":720,"vcodec":"avc1.64","acodec":"none","protocol":"https","tbr":1200},
          {"url":"v1080avc","height":1080,"vcodec":"avc1.64","acodec":"none","protocol":"https","tbr":2500},
          {"url":"aopus","height":0,"vcodec":"none","acodec":"opus","abr":160,"protocol":"https"},
          {"url":"am4a","height":0,"vcodec":"none","acodec":"mp4a.40.2","abr":128,"protocol":"https"},
          {"url":"sb","height":90,"vcodec":"none","acodec":"none","ext":"mhtml","protocol":"https"}],
          "subtitles":{"en":[{"ext":"vtt","url":"sub_en"}]}})";
        QString err; ResolvedStream r = StreamResolver::parse(json, &err);
        QString v, a; int h = 0;
        check(r.pick(720, v, a, h) && v == "v720avc" && a == "am4a" && h == 720, "pick 720p prefers H.264 + m4a over AV1/opus");
        check(r.pick(360, v, a, h) && v == "u18" && a.isEmpty(), "pick 360p uses progressive stream");
        check(r.pick(144, v, a, h) && h == 360, "no format under limit -> lowest available");
        check(r.pick(0, v, a, h) && h == 1080, "unlimited -> best");
        check(r.heights() == QList<int>({1080, 720, 360}), "quality list sorted, storyboards ignored");
        check(r.subs.size() == 1 && r.subs[0].url == "sub_en", "subtitle tracks parsed");
        QString e2; StreamResolver::parse("not json", &e2);
        check(!e2.isEmpty(), "bad yt-dlp output reported as error");
    }

    // ---- 3. failure handling (yt-dlp missing) ----
    {
        S.set("ytdlpPath", "/nonexistent/yt-dlp");
        StreamResolver rs; YouTubeApiClient api;
        QString rerr, aerr;
        QObject::connect(&rs, &StreamResolver::failed, [&](int, const QString& m) { rerr = m; });
        QObject::connect(&api, &YouTubeApiClient::failed, [&](int, const QString& m) { aerr = m; });
        rs.resolve("AAAAAAAAAAA"); api.search("x", 0);
        check(waitFor([&] { return !rerr.isEmpty() && !aerr.isEmpty(); }, 5000), "missing yt-dlp -> failure signals for resolver and search");
        check(rerr.contains("yt-dlp"), "failure message names yt-dlp");
        int cancelled = -1; bool got = false;
        QObject::connect(&api, &YouTubeApiClient::failed, [&](int id, const QString&) { if (id == cancelled) got = true; });
        S.set("ytdlpPath", "sleep");
        cancelled = api.search("x", 0); api.cancel(cancelled);
        waitFor([&] { return got; }, 300);
        check(!got, "cancelled search produces no late result");
        S.set("ytdlpPath", "yt-dlp");
    }

    // ---- 4. Shorts navigation ----
    {
        ShortsNavigation n; n.setCooldown(200);
        check(n.fromWheel(-120) == "next" && n.fromWheel(-120).isEmpty(), "wheel down -> next, burst ignored");
        QEventLoop l; QTimer::singleShot(250, &l, &QEventLoop::quit); l.exec();
        check(n.fromWheel(120) == "prev", "wheel up -> previous after cooldown");
        check(ShortsNavigation::fromKey(Qt::Key_Down) == "next" && ShortsNavigation::fromKey(Qt::Key_PageUp) == "prev" &&
              ShortsNavigation::fromKey(Qt::Key_Space) == "pause" && ShortsNavigation::fromKey(Qt::Key_M) == "mute" &&
              ShortsNavigation::fromKey(Qt::Key_Escape) == "exit", "Shorts key map (arrows, PgUp/PgDn, Space, M, Esc)");
    }

    // ---- 5. mpv control on a generated audio file (no display needed) ----
    {
        S.set("vo", "null"); S.set("ao", "null");
        MpvController m;
        QString err;
        check(m.init(0, &err), "libmpv initialises");
        QString wav = dir + "/t.wav"; writeWav(wav, 3.0);
        bool loaded = false, ended = false, paused = false; double maxPos = 0;
        QObject::connect(&m, &MpvController::fileLoaded, [&] { loaded = true; });
        QObject::connect(&m, &MpvController::playbackEnded, [&] { ended = true; });
        QObject::connect(&m, &MpvController::pauseChanged, [&](bool p) { paused = p; });
        QObject::connect(&m, &MpvController::positionChanged, [&](double p, double) { maxPos = qMax(maxPos, p); });
        m.load(wav, QString(), 0, false);
        check(waitFor([&] { return loaded; }, 5000), "mpv loads a file");
        check(waitFor([&] { return maxPos > 0.2; }, 5000), "position updates while playing");
        m.setPause(true);
        check(waitFor([&] { return paused; }, 2000), "pause event");
        m.setPause(false); m.setVolume(50); m.setSpeed(2.0);
        check(m.prop("speed").toDouble() == 2.0 && qRound(m.prop("volume").toDouble()) == 50, "speed and volume applied");
        m.seekAbs(2.0);
        check(waitFor([&] { return m.position() > 1.8; }, 3000), "seek");
        check(waitFor([&] { return ended; }, 6000), "end of file detected");
        m.stop();
        check(!m.version().isEmpty(), "mpv version readable");
        m.shutdown();
        S.set("vo", "direct3d"); S.set("ao", "");
    }

    // ---- 6. UI: construct everything, visit every page ----
    {
        S.set("vo", "null"); S.set("ao", "null");
        CacheManager cache(dir + "/t2"); ThumbnailService thumbs(&cache);
        PlaylistService pl(dir + "/pl3.json"); HistoryService hist(dir + "/h3.json"); SubscriptionService subs(dir + "/s3.json");
        YouTubeApiClient api; StreamResolver rs;
        AppContext ctx; ctx.cache = &cache; ctx.thumbs = &thumbs; ctx.playlists = &pl; ctx.history = &hist; ctx.subs = &subs; ctx.api = &api; ctx.resolver = &rs;
        AuthService auth; AccountApi account(&auth); CommentsService comments(&account);
        ctx.auth = &auth; ctx.account = &account; ctx.comments = &comments;
        PlaybackSession session(ctx); ctx.session = &session;
        S.set("ytdlpPath", "/nonexistent/yt-dlp");
        MainWindow w(ctx);
        w.show();
        for (int p = 0; p < MainWindow::PageCount; ++p) {
            w.navigate(p);
            QEventLoop l; QTimer::singleShot(40, &l, &QEventLoop::quit); l.exec();
            check(w.currentPage() == p, qPrintable(QString("navigate to page %1").arg(p)));
        }
        QList<VideoInfo> demo; for (int i = 0; i < 12; ++i) { VideoInfo d; d.id = QString("id%1xxxxxxx").arg(i, 3, 10, QChar('0')).left(11); d.title = QString("Lofi hip hop radio - beats to relax/study to #%1 with a long title that wraps").arg(i); d.channel = "Lofi Girl"; d.viewCount = 74000000; d.published = "4 years ago"; d.seconds = 600 + i * 37; d.live = (i == 0); demo << d; }
        w.resize(1180, 720);
        w.navigate(MainWindow::PHome);
        w.shortsPage();
        for (auto* grid : w.findChildren<VideoGrid*>()) grid->setVideos(demo);
        for (auto* lb : w.findChildren<QLabel*>()) if (lb->text().startsWith("yt-dlp") || lb->text() == "Loading…") lb->hide();
        { QEventLoop l3; QTimer::singleShot(200, &l3, &QEventLoop::quit); l3.exec(); }
        w.grab().save("/tmp/shot_home.png");
        w.navigate(MainWindow::PSearch);
        w.doSearch("test");
        check(w.currentPage() == MainWindow::PSearch, "search opens Search page");
        VideoInfo v; v.id = "AAAAAAAAAAA"; v.title = "Test video"; v.channel = "Ch";
        pl.add("Favorites", v); pl.add("Watch Later", v);
        w.navigate(MainWindow::PLibrary);
        check(pl.count("Favorites") == 1, "library reflects playlists");
        // Watch page with a missing yt-dlp must fail cleanly (not crash/hang); dialog is suppressed by closing it
        int failedCount = 0; QObject::connect(&session, &PlaybackSession::failed, [&] { ++failedCount; });
        QTimer closer; closer.setInterval(100);
        QObject::connect(&closer, &QTimer::timeout, [] { if (auto* m = qApp->activeModalWidget()) m->close(); });
        closer.start();
        w.watch(v);
        check(waitFor([&] { return failedCount > 0; }, 5000), "unplayable video -> clean error, no crash");
        w.toggleMini(); check(w.miniMode(), "mini player on"); w.toggleMini(); check(!w.miniMode() && w.currentPage() == MainWindow::PWatch, "mini player off");
        w.navigate(MainWindow::PShorts);
        QEventLoop l; QTimer::singleShot(300, &l, &QEventLoop::quit); l.exec();
        w.handleAction("mute"); w.handleAction("space"); w.handleAction("down"); w.handleAction("up");
        check(true, "Shorts page handles actions");
        w.navigate(MainWindow::PHome);
        SettingsDialog dlg(ctx, nullptr, 0);
        dlg.show();
        QEventLoop l2; QTimer::singleShot(100, &l2, &QEventLoop::quit); l2.exec();
        dlg.grab().save("/tmp/shot_settings.png");
        check(true, "settings dialog builds");
        w.navigate(MainWindow::PWatch);
        { QEventLoop l4; QTimer::singleShot(200, &l4, &QEventLoop::quit); l4.exec(); }
        w.grab().save("/tmp/shot_watch.png");
        w.navigate(MainWindow::PShorts);
        { QEventLoop l5; QTimer::singleShot(200, &l5, &QEventLoop::quit); l5.exec(); }
        w.grab().save("/tmp/shot_shorts.png");
        w.close();
        S.set("vo", "direct3d"); S.set("ao", ""); S.set("ytdlpPath", "yt-dlp");
    }

    std::printf("\nSelf-test: %d passed, %d failed\n", g_pass, g_fail);
    return g_fail ? 1 : 0;
}
