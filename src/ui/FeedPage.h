#pragma once
#include <QWidget>
#include <QStringList>
#include "services/AppContext.h"
#include "youtube/VideoInfo.h"
class VideoGrid; class QLabel; class QPushButton; class QHBoxLayout; class QButtonGroup;

// Shared base for Home, Trending, Search and channel feeds: chips, grid, paging, errors with Retry.
class FeedPage : public QWidget {
    Q_OBJECT
public:
    FeedPage(AppContext& ctx, const QStringList& chips, bool rows, QWidget* p = nullptr);
    void ensureLoaded();
    void reload();
signals:
    void playRequested(const VideoInfo& v, const QList<VideoInfo>& context);
protected:
    virtual int startRequest(int chip, const QString& token) = 0;   // returns request id
    void setChips(const QStringList& chips);
    int currentChip() const { return chip_; }
    void setStatus(const QString& s, bool retry = false);
    AppContext& ctx_;
    VideoGrid* grid_;
    QHBoxLayout* chipLay_;
private slots:
    void onFinished(int id, const QList<VideoInfo>& items, const QString& next);
    void onFailed(int id, const QString& msg);
private:
    void loadPage(bool more);
    QButtonGroup* group_;
    QLabel* status_;
    QPushButton* retry_;
    int req_ = -1, chip_ = 0;
    QString token_;
    bool loaded_ = false, appending_ = false;
};
