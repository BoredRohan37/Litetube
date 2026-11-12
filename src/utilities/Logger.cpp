#include "Logger.h"
#include <QFile>
#include <QDateTime>
#include <QDir>
#include <QMutex>
#include <QMutexLocker>
#include <QTextStream>
#include <QtGlobal>

static QMutex g_mx;
static QString g_path;
static bool g_on = true;

static void qtHandler(QtMsgType t, const QMessageLogContext&, const QString& m) {
    const char* lv = t == QtDebugMsg ? "DEBUG" : t == QtInfoMsg ? "INFO" : t == QtWarningMsg ? "WARN" : "ERROR";
    Logger::log(lv, m);
}

void Logger::init(const QString& dir, bool enabled) {
    QDir().mkpath(dir);
    g_path = dir + "/litetube.log";
    g_on = enabled;
    QFile f(g_path);
    if (f.size() > 1024 * 1024) { QFile::remove(g_path + ".old"); f.rename(g_path + ".old"); }
    qInstallMessageHandler(qtHandler);
}
void Logger::setEnabled(bool on) { g_on = on; }
QString Logger::path() { return g_path; }
void Logger::log(const char* level, const QString& msg) {
    if (!g_on || g_path.isEmpty()) return;
    QMutexLocker l(&g_mx);
    QFile f(g_path);
    if (!f.open(QIODevice::Append | QIODevice::Text)) return;
    QTextStream(&f) << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz") << " [" << level << "] " << msg << "\n";
}
