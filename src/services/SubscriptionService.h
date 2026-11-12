#pragma once
#include <QObject>
#include <QTimer>
#include <QList>
#include <QPair>

class SubscriptionService : public QObject {
    Q_OBJECT
public:
    explicit SubscriptionService(const QString& file, QObject* p = nullptr);
    QList<QPair<QString, QString>> list() const { return subs_; }   // channelId, name
    bool isSubscribed(const QString& channelId) const;
    void subscribe(const QString& channelId, const QString& name);
    void unsubscribe(const QString& channelId);
    void flush();
signals:
    void changed();
private:
    QString file_;
    QList<QPair<QString, QString>> subs_;
};
