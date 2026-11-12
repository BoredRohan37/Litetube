#pragma once
#include <QListWidget>
#include "services/AppContext.h"
#include "youtube/VideoInfo.h"

// Lightweight YouTube-style video list: one custom-painted delegate, thumbnails fetched only when painted.
class VideoGrid : public QListWidget {
    Q_OBJECT
public:
    enum Style { Cards, Rows };
    VideoGrid(AppContext& ctx, Style style, QWidget* p = nullptr);
    void setVideos(const QList<VideoInfo>& v);
    void append(const QList<VideoInfo>& v);
    QList<VideoInfo> videos() const;
    VideoInfo videoAt(int row) const;
    VideoInfo selectedVideo() const;
    void setRemovable(bool on) { removable_ = on; }
    Style style() const { return style_; }
    AppContext& ctx() { return ctx_; }
    int cardWidth() const { return cw_; }
signals:
    void playRequested(const VideoInfo& v);
    void loadMoreRequested();
    void removeRequested(int row);
protected:
    void resizeEvent(QResizeEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;
private slots:
    void showMenu(const QPoint& pos);
private:
    void relayout();
    void addOne(const VideoInfo& v);
    AppContext& ctx_;
    Style style_;
    bool removable_ = false;
    int cw_ = 300;
};
