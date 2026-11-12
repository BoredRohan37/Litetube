#include "AboutPage.h"
#include "playback/PlaybackSession.h"
#include "services/AppSettings.h"
#include "utilities/Logger.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QProcess>
#include <QSysInfo>
#include <QCoreApplication>
#include <mpv/client.h>

AboutPage::AboutPage(PlaybackSession* s, QWidget* p) : QWidget(p), s_(s) {
    auto* v = new QVBoxLayout(this);
    v->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    auto* logo = new QLabel("<span style='font-size:34px;font-weight:bold'>Lite<span style='color:#e01010'>Tube</span></span>");
    logo->setAlignment(Qt::AlignCenter);
    auto* ver = new QLabel(QString("Version %1").arg(LITETUBE_VERSION)); ver->setAlignment(Qt::AlignCenter);
    auto* d = new QLabel("A lightweight YouTube streaming app for older Windows PCs.\nPlayback uses libmpv with Direct3D output. Streams are resolved with yt-dlp.");
    d->setAlignment(Qt::AlignCenter); d->setObjectName("muted");
    deps_ = new QLabel; deps_->setAlignment(Qt::AlignCenter); deps_->setTextInteractionFlags(Qt::TextSelectableByMouse);
    auto* lic = new QLabel("Open source notices: Qt 5 (LGPL v3), mpv/libmpv (LGPL v2.1+ build), FFmpeg (LGPL), yt-dlp (Unlicense).\nSee the licenses folder next to the executable.\nLiteTube is not affiliated with or endorsed by YouTube or Google.");
    lic->setAlignment(Qt::AlignCenter); lic->setObjectName("muted"); lic->setWordWrap(true);
    v->addSpacing(30); v->addWidget(logo); v->addWidget(ver); v->addWidget(d); v->addSpacing(12); v->addWidget(deps_); v->addSpacing(12); v->addWidget(lic);
    refresh();
}
void AboutPage::refresh() {
    const unsigned long api = mpv_client_api_version();
    const QString base = QString("Built with Qt %1 (running %2)\nlibmpv client API %3.%4  |  %5\nOS: %6\nData folder: %7")
        .arg(QT_VERSION_STR, qVersion()).arg(api >> 16).arg(api & 0xffff).arg(s_->mpv()->version().isEmpty() ? "mpv not started yet" : s_->mpv()->version())
        .arg(QSysInfo::prettyProductName(), AppSettings::instance().dataDir());
    deps_->setText(base + "\nyt-dlp: checking…");
    auto* p = new QProcess(this);
    connect(p, &QProcess::errorOccurred, this, [this, base, p](QProcess::ProcessError) { deps_->setText(base + "\nyt-dlp: not found"); p->deleteLater(); });
    connect(p, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this, base, p](int code, QProcess::ExitStatus) {
        deps_->setText(base + "\nyt-dlp: " + (code == 0 ? QString::fromUtf8(p->readAllStandardOutput()).trimmed() : QStringLiteral("error")));
        p->deleteLater();
    });
    p->start(AppSettings::instance().str("ytdlpPath"), {"--version"});
}
