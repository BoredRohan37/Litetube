#include "ThumbnailService.h"
#include "services/CacheManager.h"
#include "services/AppSettings.h"
#include <QNetworkReply>
#include <QTimer>
#include <QSslSocket>

ThumbnailService::ThumbnailService(CacheManager* c, QObject* p) : QObject(p), cache_(c), mem_(80) {}

QPixmap ThumbnailService::cached(const QString& url) const {
    if (QPixmap* p = mem_.object(url)) return *p;
    return QPixmap();
}
void ThumbnailService::request(const QString& url) {
    if (url.isEmpty() || inflight_.contains(url)) return;
    if (QPixmap* p = mem_.object(url)) { QPixmap c = *p; QTimer::singleShot(0, this, [this, url, c] { emit ready(url, c); }); return; }
    QByteArray data;
    if (cache_->load(url, data)) {
        QPixmap pm;
        if (pm.loadFromData(data)) {
            mem_.insert(url, new QPixmap(pm));
            QTimer::singleShot(0, this, [this, url, pm] { emit ready(url, pm); });
            return;
        }
    }
    inflight_.insert(url);
    queue_.enqueue(url);
    pump();
}
void ThumbnailService::cancelPending() {
    for (const QString& u : queue_) inflight_.remove(u);
    queue_.clear();
}
void ThumbnailService::pump() {
    while (active_ < 4 && !queue_.isEmpty()) {
        const QString url = queue_.dequeue();
        QString fetch = url;
        if (!QSslSocket::supportsSsl() && fetch.startsWith("https://i.ytimg.com")) fetch.replace(0, 5, "http");   // thumbnails are also served over plain http
        QNetworkRequest rq{QUrl(fetch)};
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        rq.setTransferTimeout(AppSettings::instance().num("netTimeout") * 1000);
#endif
        ++active_;
        QNetworkReply* r = nam_.get(rq);
        connect(r, &QNetworkReply::finished, this, [this, r, url] {
            --active_;
            inflight_.remove(url);
            if (r->error() == QNetworkReply::NoError) {
                const QByteArray d = r->readAll();
                QPixmap pm;
                if (pm.loadFromData(d)) {
                    cache_->store(url, d);
                    if (++stores_ % 50 == 0) cache_->trim();
                    mem_.insert(url, new QPixmap(pm));
                    emit ready(url, pm);
                }
            }
            r->deleteLater();
            pump();
        });
    }
}
