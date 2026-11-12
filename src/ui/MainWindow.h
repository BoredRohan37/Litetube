#pragma once
#include <QMainWindow>
#include "services/AppContext.h"
#include "youtube/VideoInfo.h"
class QListWidget; class QStackedWidget; class QLineEdit; class QStringListModel; class QToolButton; class QPushButton;
class HomePage; class SearchPage; class ShortsPage; class WatchPage; class PlaylistPage; class HistoryPage;
class SubscriptionsPage; class AboutPage;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    enum Page { PHome, PShorts, PSubs, PTrending, PLibrary, PHistory, PWatch, PSearch, PAbout, PageCount };
    explicit MainWindow(AppContext& ctx, QWidget* p = nullptr);
    void navigate(int page);
    int currentPage() const;
    void openSettings(int tab = 0);
    void doSearch(const QString& q);
    void watch(const VideoInfo& v, const QList<VideoInfo>& context = {});
    void toggleMini();
    void toggleFullscreen();
    void handleAction(const QString& action);
    bool miniMode() const { return mini_; }
    WatchPage* watchPage() { return watch_; }
    ShortsPage* shortsPage() { return shorts_; }
    SearchPage* searchPage() { return search_; }
    PlaylistPage* libraryPage() { return library_; }
protected:
    void closeEvent(QCloseEvent* e) override;
    bool eventFilter(QObject* o, QEvent* e) override;
private:
    void setGuideCollapsed(bool c, bool remember = true);
    void refreshAccountButton();
    void applyChrome();
    AppContext& ctx_;
    QListWidget* nav_;
    QStackedWidget* stack_;
    QWidget *side_, *top_;
    QLineEdit* searchEdit_;
    QStringListModel* completions_;
    QToolButton *menuBtn_, *settingsBtn_;
    QPushButton* accountBtn_;
    HomePage *home_, *trending_;
    SearchPage* search_;
    ShortsPage* shorts_;
    WatchPage* watch_;
    PlaylistPage* library_;
    HistoryPage* history_;
    SubscriptionsPage* subs_;
    AboutPage* about_;
    int prevPage_ = PHome;
    bool userCollapsed_ = false;
    bool fullscreen_ = false, mini_ = false, wasMaximized_ = false, guideCollapsed_ = false;
    QRect normalGeometry_;
};
