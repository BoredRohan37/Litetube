#include "SettingsDialog.h"
#include "services/AppSettings.h"
#include "services/CacheManager.h"
#include "services/HistoryService.h"
#include "services/AuthService.h"
#include "utilities/Logger.h"
#include <QListWidget>
#include <QStackedWidget>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#include <QScrollArea>

SettingsDialog::SettingsDialog(AppContext& ctx, QWidget* p, int tab) : QDialog(p), ctx_(ctx) {
    setWindowTitle("Settings");
    resize(780, 520);
    build(tab);
    load();
}

void SettingsDialog::build(int tab) {
    auto* root = new QVBoxLayout(this);
    auto* body = new QHBoxLayout;
    auto* nav = new QListWidget; nav->setObjectName("nav"); nav->setFixedWidth(170);
    auto* stack = new QStackedWidget;
    body->addWidget(nav); body->addWidget(stack, 1);
    root->addLayout(body, 1);

    auto page = [&](const QString& name, const QString& hint = QString()) {
        nav->addItem(name);
        auto* sa = new QScrollArea; sa->setWidgetResizable(true); sa->setFrameShape(QFrame::NoFrame);
        auto* w = new QWidget; auto* v = new QVBoxLayout(w); v->setContentsMargins(18, 8, 18, 8);
        auto* h = new QLabel(name); h->setObjectName("h1"); v->addWidget(h);
        if (!hint.isEmpty()) { auto* l = new QLabel(hint); l->setObjectName("muted"); l->setWordWrap(true); v->addWidget(l); }
        auto* fw = new QWidget; auto* f = new QFormLayout(fw); f->setContentsMargins(0, 10, 0, 0); f->setVerticalSpacing(10);
        f->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        v->addWidget(fw); v->addStretch(1);
        sa->setWidget(w); stack->addWidget(sa);
        return f;
    };
    auto check = [&](QFormLayout* f, const QString& label, const QString& key) {
        auto* c = new QCheckBox(label); f->addRow(c);
        loaders_ << [=] { c->setChecked(AppSettings::instance().flag(key)); };
        savers_ << [=] { AppSettings::instance().set(key, c->isChecked()); };
    };
    auto combo = [&](QFormLayout* f, const QString& label, const QString& key, const QList<QPair<QString, QVariant>>& items) {
        auto* c = new QComboBox;
        for (const auto& it : items) c->addItem(it.first, it.second);
        f->addRow(label, c);
        loaders_ << [=] { const int i = c->findData(AppSettings::instance().value(key)); c->setCurrentIndex(i < 0 ? 0 : i); };
        savers_ << [=] { AppSettings::instance().set(key, c->currentData()); };
    };
    auto spin = [&](QFormLayout* f, const QString& label, const QString& key, int lo, int hi, const QString& suffix) {
        auto* s = new QSpinBox; s->setRange(lo, hi); s->setSuffix(suffix); f->addRow(label, s);
        loaders_ << [=] { s->setValue(AppSettings::instance().num(key)); };
        savers_ << [=] { AppSettings::instance().set(key, s->value()); };
    };
    auto dspin = [&](QFormLayout* f, const QString& label, const QString& key, double lo, double hi) {
        auto* s = new QDoubleSpinBox; s->setRange(lo, hi); s->setSingleStep(0.2); s->setDecimals(1); f->addRow(label, s);
        loaders_ << [=] { s->setValue(AppSettings::instance().dbl(key)); };
        savers_ << [=] { AppSettings::instance().set(key, s->value()); };
    };
    auto line = [&](QFormLayout* f, const QString& label, const QString& key, bool password = false, const QString& ph = QString()) {
        auto* e = new QLineEdit; if (password) e->setEchoMode(QLineEdit::Password); e->setPlaceholderText(ph); f->addRow(label, e);
        loaders_ << [=] { e->setText(AppSettings::instance().str(key)); };
        savers_ << [=] { AppSettings::instance().set(key, e->text().trimmed()); };
    };

    auto* g = page("General");
    combo(g, "Startup page", "startupPage", {{"Home", "Home"}, {"Trending", "Trending"}, {"Shorts", "Shorts"}, {"Subscriptions", "Subscriptions"}, {"Library", "Library"}, {"History", "History"}});
    check(g, "Remember window size and position", "rememberWindow");
    check(g, "Resume videos where I stopped", "resumePlayback");

    auto* pb = page("Playback");
    combo(pb, "Default quality", "quality", {{"144p", 144}, {"240p", 240}, {"360p", 360}, {"480p", 480}, {"720p", 720}, {"1080p", 1080}});
    combo(pb, "Default speed", "speed", {{"0.5x", 0.5}, {"0.75x", 0.75}, {"1.0x", 1.0}, {"1.25x", 1.25}, {"1.5x", 1.5}, {"2.0x", 2.0}});
    check(pb, "Autoplay the next related video", "autoplay");
    spin(pb, "Buffering timeout", "bufferTimeout", 10, 180, " s");
    spin(pb, "Stream buffer (RAM)", "cacheMB", 8, 256, " MB");

    auto* vd = page("Video", "Video output, decoder and compatibility changes take effect after restarting LiteTube.");
    combo(vd, "Video output", "vo", {{"Direct3D (recommended)", "direct3d"}, {"GPU (auto API)", "gpu"}, {"Automatic", "auto"}});
    check(vd, "Direct3D compatibility mode (old GPUs such as Intel GMA)", "d3dCompat");
    combo(vd, "Hardware decoding", "hwdec", {{"Off (software, safest)", "no"}, {"Auto (safe)", "auto-safe"}, {"DXVA2 (copy-back)", "dxva2-copy"}, {"D3D11 (copy-back)", "d3d11va-copy"}});
    check(vd, "Fast decoding (lower CPU, slight quality loss)", "fastDecode");

    auto* au = page("Audio");
    line(au, "Preferred audio language", "audioLang", false, "e.g. en");
    spin(au, "Default volume", "volume", 0, 100, " %");
    check(au, "Start muted", "muted");

    auto* sb = page("Subtitles", "Default style is similar to MX Player: bold white text with a black outline and soft shadow, no box.");
    check(sb, "Show subtitles when available", "subsEnabled");
    line(sb, "Preferred language", "subLang", false, "e.g. en");
    line(sb, "Font", "subFont", false, "Arial");
    spin(sb, "Size", "subSize", 20, 90, "");
    spin(sb, "Scale", "subScale", 50, 250, " %");
    dspin(sb, "Outline thickness", "subOutline", 0, 6);
    spin(sb, "Vertical position (100 = bottom)", "subPos", 50, 100, " %");
    line(sb, "Text colour", "subColor", false, "#FFFFFF");
    check(sb, "Dark box behind text", "subBox");

    auto* sh = page("Shorts");
    combo(sh, "When a Short ends", "shortsEndAction", {{"Loop it", "loop"}, {"Go to next Short", "next"}});
    spin(sh, "Scroll delay (higher = less sensitive)", "shortsCooldown", 150, 1500, " ms");
    check(sh, "Start Shorts muted", "shortsMuted");

    auto* nt = page("Network");
    spin(nt, "Network timeout", "netTimeout", 5, 120, " s");
    spin(nt, "Retries", "netRetries", 0, 6, "");
    spin(nt, "Results per page", "maxResults", 10, 40, "");
    line(nt, "Region code (for trending)", "region", false, "US");

    auto* pr = page("Privacy");
    check(pr, "Save watch history", "saveHistory");
    check(pr, "Save search history", "saveSearchHistory");
    auto* clrW = new QPushButton("Clear watch history"); auto* clrS = new QPushButton("Clear search history"); auto* clrC = new QPushButton("Clear thumbnail cache");
    pr->addRow(clrW); pr->addRow(clrS); pr->addRow(clrC);
    connect(clrW, &QPushButton::clicked, this, [this] { ctx_.history->clearWatch(); });
    connect(clrS, &QPushButton::clicked, this, [this] { ctx_.history->clearSearches(); });
    connect(clrC, &QPushButton::clicked, this, [this] { ctx_.cache->clear(); QMessageBox::information(this, "Cache", "Thumbnail cache cleared."); });

    auto* ac = page("Account", "Sign in with your Google account to use your YouTube subscriptions, playlists and liked videos, to like/dislike/subscribe and to comment. "
                               "You need your own free OAuth client ID (Desktop app) - see docs\\LOGIN_SETUP.md. Your password is never seen by LiteTube.");
    auto* status = new QLabel; status->setWordWrap(true);
    ac->addRow("Status", status);
    line(ac, "OAuth client ID", "clientId", false, "xxxx.apps.googleusercontent.com");
    line(ac, "OAuth client secret", "clientSecret", true, "from the same credential");
    auto* inBtn = new QPushButton("Sign in with Google"); inBtn->setObjectName("red");
    auto* outBtn = new QPushButton("Sign out");
    auto* row = new QHBoxLayout; row->addWidget(inBtn); row->addWidget(outBtn); row->addStretch(1);
    ac->addRow(row);
    auto* hint = new QLabel("The official API does not provide personalised Home recommendations, Watch Later or watch history. "
                            "For those, you can optionally let yt-dlp read your logged-in browser's cookies (unofficial; may conflict with YouTube's terms; Firefox works best).");
    hint->setWordWrap(true); hint->setObjectName("muted"); ac->addRow(hint);
    combo(ac, "Personalised feeds via browser cookies", "cookiesBrowser", {{"Off", ""}, {"Firefox", "firefox"}, {"Chrome", "chrome"}, {"Edge", "edge"}, {"Brave", "brave"}, {"Opera", "opera"}});
    auto refresh = [=] {
        status->setText(ctx_.auth->signedIn() ? "Signed in as " + ctx_.auth->name() : (ctx_.auth->configured() ? QStringLiteral("Not signed in") : QStringLiteral("Not set up - enter a client ID first")));
        inBtn->setEnabled(!ctx_.auth->signedIn()); outBtn->setEnabled(ctx_.auth->signedIn());
    };
    loaders_ << refresh;
    connect(ctx_.auth, &AuthService::stateChanged, this, refresh);
    connect(inBtn, &QPushButton::clicked, this, [this] { save(); ctx_.auth->signIn(); });
    connect(outBtn, &QPushButton::clicked, this, [this] { ctx_.auth->signOut(); });

    auto* ad = page("Advanced");
    line(ad, "yt-dlp path", "ytdlpPath");
    line(ad, "YouTube Data API key (optional)", "apiKey", true, "faster search & comments");
    spin(ad, "Thumbnail cache limit", "thumbCacheMB", 10, 1000, " MB");
    check(ad, "Write diagnostic log", "logging");
    auto* openLog = new QPushButton("Open log folder");
    ad->addRow(openLog);
    connect(openLog, &QPushButton::clicked, this, [] { QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(Logger::path()).absolutePath())); });

    connect(nav, &QListWidget::currentRowChanged, stack, &QStackedWidget::setCurrentIndex);
    nav->setCurrentRow(qBound(0, tab, nav->count() - 1));

    auto* btns = new QHBoxLayout;
    auto* reset = new QPushButton("Reset to default"); auto* ok = new QPushButton("OK"); ok->setObjectName("primary"); auto* cancel = new QPushButton("Cancel");
    btns->addWidget(reset); btns->addStretch(1); btns->addWidget(cancel); btns->addWidget(ok);
    root->addLayout(btns);
    connect(reset, &QPushButton::clicked, this, [this] {
        const QString id = AppSettings::instance().str("clientId"), sec = AppSettings::instance().str("clientSecret"), rt = AppSettings::instance().str("authRefresh"), nm = AppSettings::instance().str("authName");
        AppSettings::instance().resetAll();
        AppSettings::instance().set("clientId", id); AppSettings::instance().set("clientSecret", sec);
        AppSettings::instance().set("authRefresh", rt); AppSettings::instance().set("authName", nm);   // keep the sign-in
        load();
    });
    connect(ok, &QPushButton::clicked, this, [this] { save(); accept(); });
    connect(cancel, &QPushButton::clicked, this, &QDialog::reject);
}
void SettingsDialog::load() { for (auto& f : loaders_) f(); }
void SettingsDialog::save() {
    for (auto& f : savers_) f();
    Logger::setEnabled(AppSettings::instance().flag("logging"));
    if (ctx_.session) {}
}
