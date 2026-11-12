#pragma once
#include <QString>
#include <QByteArray>
class CacheManager {
public:
    explicit CacheManager(const QString& dir);
    QString pathFor(const QString& url) const;
    bool load(const QString& url, QByteArray& out) const;
    void store(const QString& url, const QByteArray& data);
    void trim();            // enforce thumbCacheMB, oldest first
    void clear();
    qint64 sizeBytes() const;
    QString dir() const { return dir_; }
private:
    QString dir_;
};
