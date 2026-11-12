#include "SysInfo.h"
#include <QElapsedTimer>
#include <QThread>
#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
static double procSeconds() {
    FILETIME c, e, k, u;
    if (!GetProcessTimes(GetCurrentProcess(), &c, &e, &k, &u)) return 0;
    auto v = [](FILETIME f) { return ((ULONGLONG(f.dwHighDateTime) << 32) | f.dwLowDateTime) / 1e7; };
    return v(k) + v(u);
}
double SysInfo::workingSetMB() {
    PROCESS_MEMORY_COUNTERS pmc;
    if (!GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) return 0;
    return pmc.WorkingSetSize / 1048576.0;
}
#else
#include <unistd.h>
#include <QFile>
double SysInfo::workingSetMB() {
    QFile f("/proc/self/statm");
    if (!f.open(QIODevice::ReadOnly)) return 0;
    const QList<QByteArray> p = f.readAll().split(' ');
    return p.size() > 1 ? p[1].toLongLong() * sysconf(_SC_PAGESIZE) / 1048576.0 : 0;
}
static double procSeconds() {
    QFile f("/proc/self/stat");
    if (!f.open(QIODevice::ReadOnly)) return 0;
    const QByteArray d = f.readAll();
    const QList<QByteArray> p = d.mid(d.lastIndexOf(')') + 2).split(' ');
    return p.size() > 13 ? (p[11].toLongLong() + p[12].toLongLong()) / double(sysconf(_SC_CLK_TCK)) : 0;
}
#endif
double SysInfo::cpuPercent() {
    static QElapsedTimer wall;
    static double last = 0;
    const double now = procSeconds();
    if (!wall.isValid()) { wall.start(); last = now; return 0; }
    const double dt = wall.restart() / 1000.0;
    const double used = now - last;
    last = now;
    const int cores = qMax(1, QThread::idealThreadCount());
    return dt > 0 ? qBound(0.0, used / dt * 100.0 / cores, 100.0) : 0;
}
