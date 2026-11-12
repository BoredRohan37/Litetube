#pragma once
#include <QString>
#include <QJsonObject>
#include <QMetaType>
#include "utilities/Format.h"

struct VideoInfo {
    QString id, title, channel, channelId, published, sortKey;
    qint64 viewCount = -1;
    int seconds = 0;
    bool live = false, isShort = false;

    bool isValid() const { return !id.isEmpty(); }
    QString thumbUrl() const { return "https://i.ytimg.com/vi/" + id + "/mqdefault.jpg"; }
    QString url() const { return "https://www.youtube.com/watch?v=" + id; }
    QString durationText() const { return live ? QStringLiteral("LIVE") : Format::duration(seconds); }
    QString subtitle() const {
        QStringList p;
        if (!channel.isEmpty()) p << channel;
        if (viewCount >= 0) p << Format::views(viewCount);
        if (!published.isEmpty()) p << published;
        return p.join(QStringLiteral("  ·  "));
    }
    QJsonObject toJson() const {
        QJsonObject o;
        o["id"] = id; o["title"] = title; o["channel"] = channel; o["channelId"] = channelId;
        o["published"] = published; o["views"] = double(viewCount); o["seconds"] = seconds;
        o["live"] = live; o["short"] = isShort;
        return o;
    }
    static VideoInfo fromJson(const QJsonObject& o) {
        VideoInfo v;
        v.id = o["id"].toString(); v.title = o["title"].toString(); v.channel = o["channel"].toString();
        v.channelId = o["channelId"].toString(); v.published = o["published"].toString();
        v.viewCount = qint64(o["views"].toDouble(-1)); v.seconds = o["seconds"].toInt();
        v.live = o["live"].toBool(); v.isShort = o["short"].toBool();
        return v;
    }
};
Q_DECLARE_METATYPE(VideoInfo)
