#include "SubscriptionService.h"
#include "JsonStore.h"
#include <QJsonArray>

SubscriptionService::SubscriptionService(const QString& file, QObject* p) : QObject(p), file_(file) {
    for (const QJsonValue& v : JsonStore::read(file_)["channels"].toArray())
        subs_ << qMakePair(v.toObject()["id"].toString(), v.toObject()["name"].toString());
}
void SubscriptionService::flush() {
    QJsonArray a;
    for (const auto& s : subs_) a.append(QJsonObject{{"id", s.first}, {"name", s.second}});
    JsonStore::write(file_, QJsonObject{{"channels", a}});
}
bool SubscriptionService::isSubscribed(const QString& id) const {
    for (const auto& s : subs_) if (s.first == id) return true;
    return false;
}
void SubscriptionService::subscribe(const QString& id, const QString& name) {
    if (id.isEmpty() || isSubscribed(id)) return;
    subs_ << qMakePair(id, name); flush(); emit changed();
}
void SubscriptionService::unsubscribe(const QString& id) {
    for (int i = 0; i < subs_.size(); ++i) if (subs_[i].first == id) { subs_.removeAt(i); flush(); emit changed(); return; }
}
