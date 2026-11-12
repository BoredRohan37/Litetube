#include "AppSettings.h"

AppSettings& AppSettings::instance() { static AppSettings a; return a; }

void AppSettings::init(const QString& dataDir) {
    dir_ = dataDir;
    defs_ = {
        {"startupPage", "Home"}, {"rememberWindow", true}, {"resumePlayback", true},
        {"quality", 480}, {"autoplay", true}, {"speed", 1.0}, {"volume", 80}, {"muted", false},
        {"vo", "direct3d"}, {"hwdec", "no"}, {"fastDecode", true}, {"d3dCompat", true}, {"cacheMB", 32}, {"ao", ""},
        {"audioLang", ""}, {"subsEnabled", true}, {"subLang", "en"}, {"subScale", 100}, {"subFont", "Arial"}, {"subSize", 46}, {"subColor", "#FFFFFF"}, {"subOutline", 2.4}, {"subBox", false}, {"subPos", 94},
        {"clientId", ""}, {"clientSecret", ""}, {"authRefresh", ""}, {"authName", ""}, {"cookiesBrowser", ""}, {"guideCollapsed", false},
        {"shortsEndAction", "loop"}, {"shortsCooldown", 450}, {"shortsMuted", false},
        {"thumbCacheMB", 100}, {"netTimeout", 20}, {"netRetries", 2}, {"bufferTimeout", 40},
        {"maxResults", 20}, {"region", "US"},
        {"saveHistory", true}, {"saveSearchHistory", true}, {"logging", true},
        {"ytdlpPath", "yt-dlp"}, {"apiKey", ""}, {"showStats", false}
    };
    s_.reset(new QSettings(dir_ + "/settings.ini", QSettings::IniFormat));
}
void AppSettings::set(const QString& k, const QVariant& v) {
    if (!s_) return;
    s_->setValue(k, v);
    emit changed(k);
}
void AppSettings::resetAll() {
    if (!s_) return;
    for (const QString& k : defs_.keys()) s_->remove(k);
    s_->sync();
    emit changed(QString());
}
