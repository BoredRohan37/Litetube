#pragma once
#include <QString>
class Logger {
public:
    static void init(const QString& dir, bool enabled);
    static void setEnabled(bool on);
    static void log(const char* level, const QString& msg);
    static QString path();
};
#define LOGI(m) Logger::log("INFO", (m))
#define LOGW(m) Logger::log("WARN", (m))
#define LOGE(m) Logger::log("ERROR", (m))
