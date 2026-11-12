#pragma once
#include <QObject>
#include <QHash>
#include <QProcess>
#include <QNetworkAccessManager>
#include <QList>
class AccountApi; class QNetworkReply;

struct Comment {
    QString author, text, when;
    int likes = 0, replyCount = 0;
    QList<Comment> replies;
};

// Top-level comments for a video: official API when a key / sign-in exists, otherwise yt-dlp (slower).
class CommentsService : public QObject {
    Q_OBJECT
public:
    explicit CommentsService(AccountApi* account, QObject* p = nullptr);
    int load(const QString& videoId, bool newest, const QString& pageToken = QString());
    void cancel(int id);
signals:
    void loaded(int id, const QList<Comment>& comments, const QString& nextToken);
    void failed(int id, const QString& message);
private:
    void parseApi(int id, const QJsonObject& o);
    AccountApi* account_;
    QNetworkAccessManager nam_;
    int seq_ = 0;
    QHash<int, QProcess*> procs_;
    QHash<int, QNetworkReply*> replies_;
    QSet<int> live_;
};
