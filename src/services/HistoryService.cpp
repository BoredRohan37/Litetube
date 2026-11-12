#include "HistoryService.h"
#include "JsonStore.h"
#include "AppSettings.h"
#include <QJsonArray>

HistoryService::HistoryService(const QString& file, QObject* p) : QObject(p), file_(file) {
    save_.setSingleShot(true); save_.setInterval(1500);
    connect(&save_, &QTimer::timeout, this, &HistoryService::flush);
    const QJsonObject o = JsonStore::read(file_);
    for (const QJsonValue& v : o["watch"].toArray()) {
        const QJsonObject e = v.toObject();
        WatchEntry w; w.v = VideoInfo::fromJson(e["video"].toObject());
        w.at = QDateTime::fromString(e["at"].toString(), Qt::ISODate); w.pos = e["pos"].toDouble();
        if (w.v.isValid()) watch_ << w;
    }
    for (const QJsonValue& v : o["search"].toArray()) search_ << v.toString();
}
void HistoryService::flush() {
    save_.stop();
    QJsonArray w, s;
    for (const WatchEntry& e : watch_) w.append(QJsonObject{{"video", e.v.toJson()}, {"at", e.at.toString(Qt::ISODate)}, {"pos", e.pos}});
    for (const QString& q : search_) s.append(q);
    JsonStore::write(file_, QJsonObject{{"watch", w}, {"search", s}});
}
void HistoryService::touch() { save_.start(); emit changed(); }
void HistoryService::addWatch(const VideoInfo& v, double pos) {
    if (!AppSettings::instance().flag("saveHistory") || !v.isValid()) return;
    double keep = pos;
    for (int i = 0; i < watch_.size(); ++i) if (watch_[i].v.id == v.id) { if (pos <= 0) keep = watch_[i].pos; watch_.removeAt(i); break; }
    watch_.prepend({v, QDateTime::currentDateTime(), keep});
    while (watch_.size() > 500) watch_.removeLast();
    touch();
}
void HistoryService::updatePosition(const QString& id, double pos) {
    for (WatchEntry& e : watch_) if (e.v.id == id) { e.pos = pos; save_.start(); return; }
}
double HistoryService::position(const QString& id) const {
    for (const WatchEntry& e : watch_) if (e.v.id == id) return e.pos;
    return 0;
}
void HistoryService::removeWatch(const QString& id) {
    for (int i = 0; i < watch_.size(); ++i) if (watch_[i].v.id == id) { watch_.removeAt(i); touch(); return; }
}
void HistoryService::clearWatch() { watch_.clear(); touch(); }
void HistoryService::addSearch(const QString& q0) {
    const QString q = q0.trimmed();
    if (q.isEmpty() || !AppSettings::instance().flag("saveSearchHistory")) return;
    search_.removeAll(q); search_.prepend(q);
    while (search_.size() > 50) search_.removeLast();
    touch();
}
void HistoryService::clearSearches() { search_.clear(); touch(); }
