#pragma once
#include <QObject>
#include <QCache>
#include <QPixmap>
#include <QQueue>
#include <QSet>
#include <QNetworkAccessManager>
class CacheManager;

class ThumbnailService : public QObject {
    Q_OBJECT
public:
    ThumbnailService(CacheManager* cache, QObject* p = nullptr);
    QPixmap cached(const QString& url) const;
    void request(const QString& url);   // emits ready() when available
    void cancelPending();               // drop queued (not yet started) downloads
signals:
    void ready(const QString& url, const QPixmap& pix);
private:
    void pump();
    CacheManager* cache_;
    QNetworkAccessManager nam_;
    mutable QCache<QString, QPixmap> mem_;
    QQueue<QString> queue_;
    QSet<QString> inflight_;
    int active_ = 0;
    int stores_ = 0;
};
