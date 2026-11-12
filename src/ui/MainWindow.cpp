#include "MainWindow.h"
#include "HomePage.h"
#include "SearchPage.h"
#include "ShortsPage.h"
#include "WatchPage.h"
#include "PlaylistPage.h"
#include "HistoryPage.h"
#include "SubscriptionsPage.h"
#include "AboutPage.h"
#include "SettingsDialog.h"
#include "Theme.h"
#include "VideoStage.h"
#include "playback/PlaybackSession.h"
#include "services/AppSettings.h"
#include "services/AuthService.h"
#include "services/PlaylistService.h"
#include "services/HistoryService.h"
#include "services/SubscriptionService.h"
#include "utilities/Logger.h"
#include <QListWidget>
#include <QStackedWidget>
#include <QLineEdit>
#include <QCompleter>
#include <QStringListModel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <QMenu>
#include <QStatusBar>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QApplication>
#include <QComboBox>
#include <QTextEdit>
#include <QAbstractSpinBox>
#ifdef _WIN32
#include <windows.h>
#endif

namespace { enum NavRole { RolePage = Qt::UserRole, RoleAction, RoleText }; }

MainWindow::MainWindow(AppContext& ctx, QWidget* p) : QMainWindow(p), ctx_(ctx) {
    setWindowTitle("LiteTube");
    setWindowIcon(Theme::appIcon());
    resize(1180, 720);
    setMinimumSize(800, 520);
    statusBar()->setSizeGripEnabled(false);
    statusBar()->setStyleSheet("QStatusBar{background:#0f0f0f;color:#aaa;border:none;}");

    auto* central = new QWidget; setCentralWidget(central);
    ctx_.session->setHost(central);
    auto* col = new QVBoxLayout(central); col->setContentsMargins(0, 0, 0, 0); col->setSpacing(0);

    // ---- top bar ----
    top_ = new QWidget; top_->setObjectName("topbar"); top_->setFixedHeight(56);
    auto* th = new QHBoxLayout(top_); th->setContentsMargins(8, 0, 12, 0); th->setSpacing(6);
    menuBtn_ = new QToolButton; menuBtn_->setObjectName("icon"); menuBtn_->setText("☰"); menuBtn_->setToolTip("Guide");
    auto* logo = new QLabel("<span style='font-size:19px;font-weight:700'><span style='color:#ff0000'>▶</span> Lite<span style='color:#ff0000'>Tube</span></span>");
    searchEdit_ = new QLineEdit; searchEdit_->setPlaceholderText("Search"); searchEdit_->setClearButtonEnabled(true);
    searchEdit_->setFixedHeight(36); searchEdit_->setStyleSheet("QLineEdit{border-radius:18px;padding-left:16px;}");
    auto* go = new QPushButton("🔍"); go->setFixedSize(52, 36); go->setStyleSheet("QPushButton{border-radius:18px;}");
    settingsBtn_ = new QToolButton; settingsBtn_->setObjectName("icon"); settingsBtn_->setText("⚙"); settingsBtn_->setToolTip("Settings");
    accountBtn_ = new QPushButton("Sign in"); accountBtn_->setObjectName("flat");
    th->addWidget(menuBtn_); th->addWidget(logo); th->addStretch(1);
    th->addWidget(searchEdit_, 4); th->addWidget(go); th->addStretch(1);
    th->addWidget(settingsBtn_); th->addWidget(accountBtn_);
    col->addWidget(top_);
    completions_ = new QStringListModel(this);
    auto* comp = new QCompleter(completions_, this); comp->setCaseSensitivity(Qt::CaseInsensitive); searchEdit_->setCompleter(comp);

    // ---- guide + pages ----
    auto* body = new QHBoxLayout; body->setSpacing(0); body->setContentsMargins(0, 0, 0, 0);
    side_ = new QWidget; side_->setFixedWidth(220);
    auto* sv = new QVBoxLayout(side_); sv->setContentsMargins(0, 0, 0, 0);
    nav_ = new QListWidget; nav_->setObjectName("nav"); nav_->setIconSize(QSize(1, 1));
    struct Item { const char* text; int page; int action; };   // action: 2 = Watch Later, 3 = Playlists, 4 = Liked
    const Item items[] = {{"🏠  Home", PHome, 0}, {"⚡  Shorts", PShorts, 0}, {"📺  Subscriptions", PSubs, 0},
                          {"", -1, 0},
                          {"📁  Library", PLibrary, 0}, {"🕘  History", PHistory, 0}, {"⏱  Watch later", PLibrary, 2}, {"🎵  Playlists", PLibrary, 3}, {"👍  Liked videos", PLibrary, 4},
                          {"", -1, 0},
                          {"🔥  Trending", PTrending, 0}, {"ℹ  About", PAbout, 0}};
    for (const Item& it : items) {
        auto* li = new QListWidgetItem(QString::fromUtf8(it.text), nav_);
        if (it.page < 0) { li->setFlags(Qt::NoItemFlags); li->setSizeHint(QSize(10, 12)); }
        li->setData(RolePage, it.page); li->setData(RoleAction, it.action); li->setData(RoleText, QString::fromUtf8(it.text));
    }
    sv->addWidget(nav_);
    body->addWidget(side_);
    stack_ = new QStackedWidget;
    body->addWidget(stack_, 1);
    col->addLayout(body, 1);

    home_ = new HomePage(ctx, HomePage::Home);
    trending_ = new HomePage(ctx, HomePage::Trending);
    search_ = new SearchPage(ctx);
    shorts_ = new ShortsPage(ctx);
    watch_ = new WatchPage(ctx);
    library_ = new PlaylistPage(ctx);
    history_ = new HistoryPage(ctx);
    subs_ = new SubscriptionsPage(ctx);
    about_ = new AboutPage(ctx.session);
    QWidget* pages[PageCount] = {home_, shorts_, subs_, trending_, library_, history_, watch_, search_, about_};
    for (QWidget* w : pages) stack_->addWidget(w);

    connect(menuBtn_, &QToolButton::clicked, this, [this] { setGuideCollapsed(!guideCollapsed_); });
    connect(settingsBtn_, &QToolButton::clicked, this, [this] { openSettings(0); });
    connect(accountBtn_, &QPushButton::clicked, this, [this] {
        if (ctx_.auth->signedIn()) {
            QMenu m(this); m.addAction("Signed in as " + ctx_.auth->name())->setEnabled(false); m.addSeparator();
            m.addAction("Account settings…", this, [this] { openSettings(8); });
            m.addAction("Sign out", this, [this] { ctx_.auth->signOut(); });
            m.exec(accountBtn_->mapToGlobal(QPoint(0, accountBtn_->height())));
        } else if (!ctx_.auth->configured()) openSettings(8);
        else ctx_.auth->signIn();
    });
    connect(ctx_.auth, &AuthService::stateChanged, this, &MainWindow::refreshAccountButton);
    connect(ctx_.auth, &AuthService::message, this, [this](const QString& m) { statusBar()->showMessage(m, 9000); });
    connect(nav_, &QListWidget::itemClicked, this, [this](QListWidgetItem* it) {
        const int action = it->data(RoleAction).toInt(), page = it->data(RolePage).toInt();
        if (page < 0) return;
        navigate(page);
        if (action == 2) library_->selectList(PlaylistService::watchLater());
        else if (action == 3) library_->selectList(QString());
        else if (action == 4) library_->selectList(ctx_.account->available() ? QStringLiteral("yt:liked") : PlaylistService::favorites());
        else if (page == PLibrary) library_->selectList(PlaylistService::favorites());
    });
    connect(go, &QPushButton::clicked, this, [this] { doSearch(searchEdit_->text()); });
    connect(searchEdit_, &QLineEdit::returnPressed, this, [this] { doSearch(searchEdit_->text()); });
    auto updateCompl = [this] { completions_->setStringList(ctx_.history->searches()); };
    connect(ctx_.history, &HistoryService::changed, this, updateCompl);
    updateCompl();

    auto playCtx = [this](const VideoInfo& v, const QList<VideoInfo>& c) { watch(v, c); };
    connect(home_, &FeedPage::playRequested, this, playCtx);
    connect(trending_, &FeedPage::playRequested, this, playCtx);
    connect(search_, &FeedPage::playRequested, this, playCtx);
    connect(subs_, &SubscriptionsPage::playRequested, this, playCtx);
    connect(history_, &HistoryPage::playRequested, this, [this](const VideoInfo& v) { watch(v); });
    connect(history_, &HistoryPage::searchRequested, this, &MainWindow::doSearch);
    connect(library_, &PlaylistPage::playQueueRequested, this, [this](const QList<VideoInfo>& l, int i, bool sh, bool rp) {
        navigate(PWatch); watch_->playQueue(l, i, sh, rp);
    });
    connect(watch_, &WatchPage::toggleMini, this, &MainWindow::toggleMini);
    connect(watch_, &WatchPage::toggleFullscreen, this, &MainWindow::toggleFullscreen);
    connect(shorts_, &ShortsPage::exitRequested, this, [this] { navigate(prevPage_ == PShorts ? PHome : prevPage_); });
    connect(ctx_.session, &PlaybackSession::inputAction, this, &MainWindow::handleAction);
    qApp->installEventFilter(this);

    userCollapsed_ = AppSettings::instance().flag("guideCollapsed");
    setGuideCollapsed(userCollapsed_, false);
    refreshAccountButton();
    static const QHash<QString, int> startMap{{"Home", PHome}, {"Trending", PTrending}, {"Shorts", PShorts}, {"Subscriptions", PSubs}, {"Library", PLibrary}, {"History", PHistory}};
    if (AppSettings::instance().flag("rememberWindow")) restoreGeometry(AppSettings::instance().value("geometry").toByteArray());
    navigate(startMap.value(AppSettings::instance().str("startupPage"), PHome));
}

int MainWindow::currentPage() const { return stack_->currentIndex(); }

void MainWindow::setGuideCollapsed(bool c, bool remember) {
    guideCollapsed_ = c;
    if (remember) userCollapsed_ = c;
    side_->setFixedWidth(c ? 68 : 220);
    for (int i = 0; i < nav_->count(); ++i) {
        QListWidgetItem* it = nav_->item(i);
        const QString full = it->data(RoleText).toString();
        if (full.isEmpty()) continue;
        it->setText(c ? full.section(' ', 0, 0) : full);
        it->setToolTip(c ? full.mid(full.indexOf(' ')).trimmed() : QString());
        it->setTextAlignment(c ? Qt::AlignCenter : (Qt::AlignLeft | Qt::AlignVCenter));
    }
    if (remember) AppSettings::instance().set("guideCollapsed", c);
}
void MainWindow::refreshAccountButton() {
    if (ctx_.auth->signedIn()) accountBtn_->setText("👤 " + (ctx_.auth->name().isEmpty() ? QStringLiteral("Account") : ctx_.auth->name().left(18)));
    else accountBtn_->setText("👤 Sign in");
}

void MainWindow::navigate(int page) {
    if (page < 0 || page >= PageCount) return;
    if (mini_) toggleMini();
    const int old = stack_->currentIndex();
    if (old != page) {
        if (old == PShorts) shorts_->onDeactivated();
        if (old == PWatch) watch_->onDeactivated(false);
        if (old != PWatch && old != PShorts) prevPage_ = old;
        if (fullscreen_ && page != PWatch) toggleFullscreen();
    }
    stack_->setCurrentIndex(page);
    // the Watch page gets the full width, like YouTube's overlay guide
    if (page == PWatch) { if (!guideCollapsed_) setGuideCollapsed(true, false); }
    else if (guideCollapsed_ && !userCollapsed_) setGuideCollapsed(false, false);
    const bool wasNav = (old != page);
    for (int i = 0; i < nav_->count(); ++i)
        if (nav_->item(i)->data(RolePage).toInt() == page && nav_->item(i)->data(RoleAction).toInt() == 0) { nav_->setCurrentRow(i); break; }
    if (page == PHome) home_->ensureLoaded();
    else if (page == PTrending) trending_->ensureLoaded();
    else if (page == PShorts && wasNav) shorts_->onActivated();
    else if (page == PWatch && wasNav) watch_->onActivated();
    else if (page == PLibrary) library_->selectList(QString());
    else if (page == PAbout) about_->refresh();
    else if (page == PSubs) subs_->refresh();
}

void MainWindow::openSettings(int tab) {
    SettingsDialog dlg(ctx_, this, tab);
    dlg.exec();
    refreshAccountButton();
}
void MainWindow::doSearch(const QString& q) {
    if (q.trimmed().isEmpty()) return;
    searchEdit_->setText(q.trimmed());
    navigate(PSearch);
    search_->search(q);
}
void MainWindow::watch(const VideoInfo& v, const QList<VideoInfo>& context) {
    if (!mini_) navigate(PWatch);
    watch_->openVideo(v, context);
}
void MainWindow::toggleMini() {
    if (!mini_) {
        if (currentPage() != PWatch) return;
        if (fullscreen_) toggleFullscreen();
        wasMaximized_ = isMaximized();
        if (wasMaximized_) showNormal();
        normalGeometry_ = geometry();
        mini_ = true;
        side_->hide(); top_->hide(); statusBar()->hide();
        watch_->setFullscreenLayout(true);
        setMinimumSize(360, 240);
        resize(520, 340);
#ifdef _WIN32
        SetWindowPos(reinterpret_cast<HWND>(winId()), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);   // always on top without re-creating the window
#endif
        setWindowTitle("LiteTube - Playing");
    } else {
        mini_ = false;
#ifdef _WIN32
        SetWindowPos(reinterpret_cast<HWND>(winId()), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
#endif
        side_->show(); top_->show(); statusBar()->show();
        watch_->setFullscreenLayout(false);
        setMinimumSize(800, 520);
        setGeometry(normalGeometry_);
        if (wasMaximized_) showMaximized();
        setWindowTitle("LiteTube");
    }
}
void MainWindow::toggleFullscreen() {
    if (currentPage() != PWatch || mini_) return;
    fullscreen_ = !fullscreen_;
    side_->setVisible(!fullscreen_); top_->setVisible(!fullscreen_);
    watch_->setFullscreenLayout(fullscreen_);
    if (fullscreen_) { wasMaximized_ = isMaximized(); showFullScreen(); }
    else { wasMaximized_ ? showMaximized() : showNormal(); }
}

void MainWindow::handleAction(const QString& a) {
    if (currentPage() == PShorts) { shorts_->handleAction(a); return; }
    MpvController* m = ctx_.session->mpv();
    if (a == "space" || a == "click") m->setPause(!m->paused());
    else if (a == "mute") { const bool nm = !m->muted(); m->setMute(nm); AppSettings::instance().set("muted", nm); }
    else if (a == "left") m->seekRel(-5);
    else if (a == "right") m->seekRel(5);
    else if (a == "up") { const int v = qMin(100, AppSettings::instance().num("volume") + 5); AppSettings::instance().set("volume", v); m->setVolume(v); }
    else if (a == "down") { const int v = qMax(0, AppSettings::instance().num("volume") - 5); AppSettings::instance().set("volume", v); m->setVolume(v); }
    else if (a == "fullscreen" || a == "dblclick") toggleFullscreen();
    else if (a == "esc") { if (mini_) toggleMini(); else if (fullscreen_) toggleFullscreen(); }
}

bool MainWindow::eventFilter(QObject*, QEvent* e) {
    if (e->type() != QEvent::KeyPress || !isActiveWindow()) return false;
    QWidget* f = QApplication::focusWidget();
    if (qobject_cast<QLineEdit*>(f) || qobject_cast<QTextEdit*>(f) || qobject_cast<QAbstractSpinBox*>(f) ||
        (qobject_cast<QComboBox*>(f) && qobject_cast<QComboBox*>(f)->isEditable()) || (f && f->inherits("QPlainTextEdit")))
        return false;
    const int page = currentPage();
    if (page != PShorts && page != PWatch) return false;
    auto* ke = static_cast<QKeyEvent*>(e);
    if (ke->modifiers() & (Qt::ControlModifier | Qt::AltModifier)) return false;
    QString a;
    switch (ke->key()) {
    case Qt::Key_Space: a = "space"; break;
    case Qt::Key_M: a = "mute"; break;
    case Qt::Key_F: a = "fullscreen"; break;
    case Qt::Key_Escape: a = "esc"; break;
    case Qt::Key_Left: a = "left"; break;
    case Qt::Key_Right: a = "right"; break;
    case Qt::Key_Up: a = "up"; break;
    case Qt::Key_Down: a = "down"; break;
    case Qt::Key_PageUp: a = "pgup"; break;
    case Qt::Key_PageDown: a = "pgdown"; break;
    default: return false;
    }
    if (page == PWatch && (a == "up" || a == "down" || a == "pgup" || a == "pgdown") && (qobject_cast<QListWidget*>(f) || f == nullptr || f->inherits("QScrollArea"))) return false;
    if (page == PWatch && (a == "pgup" || a == "pgdown")) return false;
    handleAction(a);
    return true;
}

void MainWindow::closeEvent(QCloseEvent* e) {
    if (AppSettings::instance().flag("rememberWindow") && !isFullScreen() && !mini_) AppSettings::instance().set("geometry", saveGeometry());
    ctx_.session->shutdown();
    ctx_.playlists->flush(); ctx_.history->flush(); ctx_.subs->flush();
    QMainWindow::closeEvent(e);
}
