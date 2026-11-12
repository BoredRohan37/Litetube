#include "ErrorHandler.h"
#include "Logger.h"
#include <QMessageBox>
#include <QPushButton>

namespace ErrorHandler {
QString friendly(const QString& raw) {
    const QString r = raw.toLower();
    auto has = [&](const char* s) { return r.contains(QLatin1String(s)); };
    if (has("quota")) return "The YouTube API daily quota is used up. LiteTube will use yt-dlp for search until it resets.";
    if (has("keyinvalid") || has("api key not valid")) return "The YouTube API key is not valid. Check it in Settings > Advanced, or clear it to use yt-dlp only.";
    if (has("yt-dlp not found") || has("failed to start")) return "yt-dlp could not be started. Put yt-dlp.exe next to LiteTube.exe or set its path in Settings > Advanced.";
    if (has("private video") || has("video unavailable") || has("has been removed") || has("not available") || has("this video is")) return "This video is unavailable or may have been removed.";
    if (has("sign in") || has("confirm you") || has("age")) return "YouTube asks for sign-in or age confirmation for this video, which LiteTube cannot provide.";
    if (has("403") || has("expired")) return "The stream link expired. Press Retry to get a fresh one.";
    if (has("timed out") || has("timeout")) return "The connection timed out. Check your internet connection and retry.";
    if (has("resolve") || has("network") || has("unable to download") || has("connection") || has("host")) return "Could not reach YouTube. Check your internet connection and retry.";
    if (has("direct3d") || has("video output") || has("vo:")) return "Direct3D video output failed to start. Try another video output in Settings > Video.";
    if (has("unsupported") || has("format")) return "This video's format is not supported on this PC. Try a lower quality.";
    return raw.isEmpty() ? QStringLiteral("Something went wrong.") : raw.left(300);
}

bool showRetry(QWidget* parent, const QString& title, const QString& raw) {
    LOGW(title + ": " + raw);
    QMessageBox box(QMessageBox::Warning, title, friendly(raw), QMessageBox::NoButton, parent);
    box.addButton(QObject::tr("OK"), QMessageBox::RejectRole);
    QPushButton* retry = box.addButton(QObject::tr("Retry"), QMessageBox::AcceptRole);
    box.exec();
    return box.clickedButton() == retry;
}
}
