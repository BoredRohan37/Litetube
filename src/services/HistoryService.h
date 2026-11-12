#pragma once
#include <QObject>
#include <QTimer>
#include <QDateTime>
#include "youtube/VideoInfo.h"

struct WatchEntry { VideoInfo v; QDateTime at; double pos = 0; };

class HistoryService : public QObject {
    Q_OBJECT
public:
    explicit HistoryService(const QString& file, QObject* p = nullptr);
    void addWatch(const VideoInfo& v, double pos = 0);
    void updatePosition(const QString& id, double pos);
    double position(const QString& id) const;
    QList<WatchEntry> watch() const { return watch_; }
    void removeWatch(const QString& id);
    void clearWatch();
    void addSearch(const QString& q);
    QStringList searches() const { return search_; }
    void clearSearches();
    void flush();
signals:
    void changed();
private:
    void touch();
    QString file_;
    QList<WatchEntry> watch_;
    QStringList search_;
    QTimer save_;
};
