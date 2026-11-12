#pragma once
#include <QObject>
#include <QTimer>
#include <QMap>
#include "youtube/VideoInfo.h"

class PlaylistService : public QObject {
    Q_OBJECT
public:
    static QString favorites() { return QStringLiteral("Favorites"); }
    static QString watchLater() { return QStringLiteral("Watch Later"); }
    explicit PlaylistService(const QString& file, QObject* p = nullptr);
    QStringList names() const { return order_; }
    bool exists(const QString& n) const { return data_.contains(n); }
    bool isReserved(const QString& n) const { return n == favorites() || n == watchLater(); }
    bool create(const QString& n);
    bool rename(const QString& from, const QString& to);
    bool remove(const QString& n);
    QList<VideoInfo> videos(const QString& n) const { return data_.value(n); }
    int count(const QString& n) const { return data_.value(n).size(); }
    bool contains(const QString& n, const QString& id) const;
    bool add(const QString& n, const VideoInfo& v);
    void removeVideo(const QString& n, const QString& id);
    void removeAt(const QString& n, int row);
    void move(const QString& n, int from, int to);
    void flush();
signals:
    void changed();
private:
    void load();
    void touch();
    QString file_;
    QStringList order_;
    QMap<QString, QList<VideoInfo>> data_;
    QTimer save_;
};
