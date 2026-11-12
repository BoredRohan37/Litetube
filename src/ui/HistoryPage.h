#pragma once
#include <QWidget>
#include "services/AppContext.h"
#include "youtube/VideoInfo.h"
class VideoGrid; class QListWidget; class QTabWidget;
class HistoryPage : public QWidget {
    Q_OBJECT
public:
    explicit HistoryPage(AppContext& ctx, QWidget* p = nullptr);
    void refresh();
signals:
    void playRequested(const VideoInfo& v);
    void searchRequested(const QString& q);
private:
    AppContext& ctx_;
    VideoGrid* grid_;
    QListWidget* searches_;
};
