#include "CacheManager.h"
#include "AppSettings.h"
#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <algorithm>

CacheManager::CacheManager(const QString& dir) : dir_(dir) { QDir().mkpath(dir_); }
QString CacheManager::pathFor(const QString& url) const {
    return dir_ + "/" + QCryptographicHash::hash(url.toUtf8(), QCryptographicHash::Sha1).toHex() + ".img";
}
bool CacheManager::load(const QString& url, QByteArray& out) const {
    QFile f(pathFor(url));
    if (!f.open(QIODevice::ReadOnly)) return false;
    out = f.readAll();
    return !out.isEmpty();
}
void CacheManager::store(const QString& url, const QByteArray& data) {
    QFile f(pathFor(url));
    if (f.open(QIODevice::WriteOnly)) f.write(data);
}
qint64 CacheManager::sizeBytes() const {
    qint64 t = 0;
    for (const QFileInfo& fi : QDir(dir_).entryInfoList(QDir::Files)) t += fi.size();
    return t;
}
void CacheManager::trim() {
    const qint64 limit = qint64(AppSettings::instance().num("thumbCacheMB")) * 1048576;
    QFileInfoList l = QDir(dir_).entryInfoList(QDir::Files);
    qint64 total = 0;
    for (const QFileInfo& fi : l) total += fi.size();
    if (total <= limit) return;
    std::sort(l.begin(), l.end(), [](const QFileInfo& a, const QFileInfo& b) { return a.lastModified() < b.lastModified(); });
    for (const QFileInfo& fi : l) {
        if (total <= limit * 8 / 10) break;
        total -= fi.size();
        QFile::remove(fi.absoluteFilePath());
    }
}
void CacheManager::clear() {
    for (const QFileInfo& fi : QDir(dir_).entryInfoList(QDir::Files)) QFile::remove(fi.absoluteFilePath());
}
