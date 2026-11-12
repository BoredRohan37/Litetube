#pragma once
#include <QObject>
#include <QHash>
#include <QProcess>
#include <QList>

struct StreamFormat {
    int height = 0;
    QString url, vcodec, acodec;
    bool hasVideo = false, hasAudio = false;
    double tbr = 0, abr = 0;
};
struct SubTrack { QString lang, label, url; };

struct ResolvedStream {
    QString id, title, channel, channelId, description;
    int duration = 0;
    bool live = false;
    QList<StreamFormat> formats;
    QList<SubTrack> subs;
    qint64 resolvedAt = 0;
    QList<int> heights() const;
    // Chooses the best stream not taller than maxHeight (<=0: no limit). Prefers H.264 (cheap to decode on old CPUs).
    bool pick(int maxHeight, QString& video, QString& audio, int& height) const;
};

class StreamResolver : public QObject {
    Q_OBJECT
public:
    explicit StreamResolver(QObject* p = nullptr) : QObject(p) {}
    int resolve(const QString& videoId);
    void cancel(int reqId);
    void cancelAll();
    static ResolvedStream parse(const QByteArray& json, QString* err);
signals:
    void resolved(int reqId, const ResolvedStream& s);
    void failed(int reqId, const QString& message);
private:
    int seq_ = 0;
    QHash<int, QProcess*> procs_;
};
