#pragma once
#include <QString>
#include <QDateTime>
namespace Format {
inline QString duration(int s) {
    if (s <= 0) return QString();
    const int h = s / 3600, m = (s % 3600) / 60, sec = s % 60;
    return h ? QString("%1:%2:%3").arg(h).arg(m, 2, 10, QChar('0')).arg(sec, 2, 10, QChar('0'))
             : QString("%1:%2").arg(m).arg(sec, 2, 10, QChar('0'));
}
inline QString clockTime(double sec) {
    int s = qMax(0, int(sec));
    const int h = s / 3600, m = (s % 3600) / 60;
    s %= 60;
    return h ? QString("%1:%2:%3").arg(h).arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0'))
             : QString("%1:%2").arg(m).arg(s, 2, 10, QChar('0'));
}
inline QString views(qint64 v) {
    if (v < 0) return QString();
    if (v >= 1000000000) return QString("%1B views").arg(v / 1e9, 0, 'f', 1);
    if (v >= 1000000) return QString("%1M views").arg(v / 1e6, 0, 'f', v >= 10000000 ? 0 : 1);
    if (v >= 1000) return QString("%1K views").arg(v / 1e3, 0, 'f', v >= 10000 ? 0 : 1);
    return QString("%1 views").arg(v);
}
inline QString ago(const QDateTime& t) {
    if (!t.isValid()) return QString();
    const qint64 s = t.secsTo(QDateTime::currentDateTime());
    if (s < 3600) return QString("%1 min ago").arg(qMax<qint64>(1, s / 60));
    if (s < 86400) return QString("%1 hours ago").arg(s / 3600);
    if (s < 86400 * 30) return QString("%1 days ago").arg(s / 86400);
    if (s < 86400 * 365) return QString("%1 months ago").arg(s / (86400 * 30));
    return QString("%1 years ago").arg(s / (86400 * 365));
}
}
