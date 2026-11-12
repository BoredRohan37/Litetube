#pragma once
#include <QObject>
#include <QSet>
#include "services/AppContext.h"
#include "youtube/VideoInfo.h"

// Owns the Shorts list: paging, de-duplication, thumbnail pre-fetch of the next item only, request cancellation.
class ShortsController : public QObject {
    Q_OBJECT
public:
    explicit ShortsController(AppContext& ctx, QObject* p = nullptr);
    void start(const QString& topic);
    bool next();
    bool prev();
    void skipUnavailable();                 // current one cannot be played: move on
    VideoInfo current() const { return idx_ >= 0 && idx_ < list_.size() ? list_[idx_] : VideoInfo(); }
    int index() const { return idx_; }
    int count() const { return list_.size(); }
    bool isLoading() const { return req_ >= 0; }
    void cancel();
signals:
    void currentChanged(const VideoInfo& v, int index);
    void listLoading(bool on);
    void listError(const QString& message);
private slots:
    void onFinished(int id, const QList<VideoInfo>& items, const QString& token);
    void onFailed(int id, const QString& msg);
private:
    void goTo(int i);
    void loadMore();
    AppContext& ctx_;
    QList<VideoInfo> list_;
    QSet<QString> seen_;
    QString topic_, token_;
    int idx_ = -1, req_ = -1;
    bool wantNext_ = false;
    int skipRun_ = 0;
};
