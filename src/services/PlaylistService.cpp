#include "PlaylistService.h"
#include "JsonStore.h"
#include <QJsonArray>

PlaylistService::PlaylistService(const QString& file, QObject* p) : QObject(p), file_(file) {
    save_.setSingleShot(true);
    save_.setInterval(400);
    connect(&save_, &QTimer::timeout, this, &PlaylistService::flush);
    load();
}
void PlaylistService::load() {
    order_.clear(); data_.clear();
    const QJsonArray arr = JsonStore::read(file_)["playlists"].toArray();
    for (const QJsonValue& pv : arr) {
        const QJsonObject po = pv.toObject();
        const QString n = po["name"].toString();
        if (n.isEmpty() || data_.contains(n)) continue;
        QList<VideoInfo> vs;
        for (const QJsonValue& vv : po["videos"].toArray()) { VideoInfo v = VideoInfo::fromJson(vv.toObject()); if (v.isValid()) vs << v; }
        order_ << n; data_[n] = vs;
    }
    for (const QString& r : {favorites(), watchLater()})
        if (!data_.contains(r)) { order_.prepend(r); data_[r] = {}; }
    // keep reserved lists first
    order_.removeAll(watchLater()); order_.prepend(watchLater());
    order_.removeAll(favorites()); order_.prepend(favorites());
}
void PlaylistService::flush() {
    save_.stop();
    QJsonArray arr;
    for (const QString& n : order_) {
        QJsonArray vs;
        for (const VideoInfo& v : data_[n]) vs.append(v.toJson());
        arr.append(QJsonObject{{"name", n}, {"videos", vs}});
    }
    JsonStore::write(file_, QJsonObject{{"playlists", arr}});
}
void PlaylistService::touch() { save_.start(); emit changed(); }
bool PlaylistService::create(const QString& n0) {
    const QString n = n0.trimmed();
    if (n.isEmpty() || data_.contains(n)) return false;
    order_ << n; data_[n] = {}; touch(); return true;
}
bool PlaylistService::rename(const QString& from, const QString& to0) {
    const QString to = to0.trimmed();
    if (isReserved(from) || to.isEmpty() || data_.contains(to) || !data_.contains(from)) return false;
    order_.replace(order_.indexOf(from), to);
    data_[to] = data_.take(from); touch(); return true;
}
bool PlaylistService::remove(const QString& n) {
    if (isReserved(n) || !data_.contains(n)) return false;
    order_.removeAll(n); data_.remove(n); touch(); return true;
}
bool PlaylistService::contains(const QString& n, const QString& id) const {
    for (const VideoInfo& v : data_.value(n)) if (v.id == id) return true;
    return false;
}
bool PlaylistService::add(const QString& n, const VideoInfo& v) {
    if (!data_.contains(n) || !v.isValid() || contains(n, v.id)) return false;
    data_[n].append(v); touch(); return true;
}
void PlaylistService::removeVideo(const QString& n, const QString& id) {
    auto& l = data_[n];
    for (int i = 0; i < l.size(); ++i) if (l[i].id == id) { l.removeAt(i); touch(); return; }
}
void PlaylistService::removeAt(const QString& n, int row) {
    auto& l = data_[n];
    if (row >= 0 && row < l.size()) { l.removeAt(row); touch(); }
}
void PlaylistService::move(const QString& n, int from, int to) {
    auto& l = data_[n];
    if (from < 0 || to < 0 || from >= l.size() || to >= l.size() || from == to) return;
    l.move(from, to); touch();
}
