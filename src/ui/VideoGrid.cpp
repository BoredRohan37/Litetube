#include "VideoGrid.h"
#include "services/PlaylistService.h"
#include "services/SubscriptionService.h"
#include "youtube/ThumbnailService.h"
#include <QStyledItemDelegate>
#include <QPainter>
#include <QMenu>
#include <QScrollBar>
#include <QApplication>
#include <QClipboard>
#include <QInputDialog>
#include <QMetaObject>
#include <QKeyEvent>

static const int RoleVideo = Qt::UserRole + 3;

namespace {
class CardDelegate : public QStyledItemDelegate {
public:
    CardDelegate(VideoGrid* g) : QStyledItemDelegate(g), g_(g) {}
    QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const override {
        if (g_->style() == VideoGrid::Rows) return QSize(qMax(300, g_->viewport()->width() - 4), 112);
        const int w = g_->cardWidth();
        return QSize(w, (w - 16) * 9 / 16 + 96);
    }
    void paint(QPainter* p, const QStyleOptionViewItem& o, const QModelIndex& idx) const override {
        const VideoInfo v = idx.data(RoleVideo).value<VideoInfo>();
        QRect r = o.rect.adjusted(4, 4, -4, -4);
        const bool hover = o.state & (QStyle::State_MouseOver | QStyle::State_Selected);
        p->save();
        if (hover) { p->setBrush(QColor("#1f1f1f")); p->setPen(Qt::NoPen); p->drawRoundedRect(r, 10, 10); }
        const bool rows = g_->style() == VideoGrid::Rows;
        QRect th = rows ? QRect(r.left() + 4, r.top() + 4, 168, 94) : QRect(r.left() + 4, r.top() + 4, r.width() - 8, (r.width() - 8) * 9 / 16);
        QPixmap pm = g_->ctx().thumbs->cached(v.thumbUrl());
        p->setClipRegion(QRegion(th));
        if (pm.isNull()) { p->fillRect(th, QColor("#272727")); g_->ctx().thumbs->request(v.thumbUrl()); }
        else {
            const double sx = double(pm.width()) / th.width(), sy = double(pm.height()) / th.height(), k = qMin(sx, sy);
            const QSize src(int(th.width() * k), int(th.height() * k));
            p->drawPixmap(th, pm, QRect((pm.width() - src.width()) / 2, (pm.height() - src.height()) / 2, src.width(), src.height()));
        }
        p->setClipping(false);
        const QString d = v.durationText();
        QFont f = p->font(); f.setPixelSize(11); f.setBold(true); p->setFont(f);
        if (!d.isEmpty()) {
            const int w = p->fontMetrics().horizontalAdvance(d) + 10;
            const QRect b(th.right() - w - 5, th.bottom() - 21, w, 17);
            p->setPen(Qt::NoPen); p->setBrush(v.live ? QColor("#cc0000") : QColor(0, 0, 0, 205)); p->drawRoundedRect(b, 4, 4);
            p->setPen(Qt::white); p->drawText(b, Qt::AlignCenter, d);
        }
        QRect tx;
        if (rows) tx = QRect(th.right() + 12, r.top() + 6, r.right() - th.right() - 18, r.height() - 10);
        else {
            const QRect av(r.left() + 4, th.bottom() + 12, 36, 36);
            p->setRenderHint(QPainter::Antialiasing);
            p->setPen(Qt::NoPen); p->setBrush(QColor::fromHsv(qAbs(qHash(v.channel)) % 360, 120, 150)); p->drawEllipse(av);
            p->setPen(Qt::white); f.setPixelSize(15); p->setFont(f);
            p->drawText(av, Qt::AlignCenter, v.channel.left(1).toUpper());
            tx = QRect(av.right() + 10, th.bottom() + 9, r.right() - av.right() - 14, r.bottom() - th.bottom() - 10);
        }
        f.setPixelSize(14); f.setBold(true); p->setFont(f); p->setPen(QColor("#f1f1f1"));
        const QFontMetrics fm(f);
        QString title = v.title;
        const QRect trect(tx.left(), tx.top(), tx.width(), fm.lineSpacing() * 2);
        p->drawText(trect, Qt::TextWordWrap | Qt::AlignTop, fm.elidedText(title, Qt::ElideRight, tx.width() * 2 - 24));
        f.setPixelSize(12); f.setBold(false); p->setFont(f); p->setPen(QColor("#aaaaaa"));
        const QFontMetrics fm2(f);
        int y = trect.bottom() + 3;
        p->drawText(QRect(tx.left(), y, tx.width(), fm2.height()), Qt::AlignLeft, fm2.elidedText(v.channel, Qt::ElideRight, tx.width()));
        QStringList meta; if (v.viewCount >= 0) meta << Format::views(v.viewCount); if (!v.published.isEmpty()) meta << v.published;
        p->drawText(QRect(tx.left(), y + fm2.height() + 1, tx.width(), fm2.height()), Qt::AlignLeft, fm2.elidedText(meta.join(" · "), Qt::ElideRight, tx.width()));
        p->restore();
    }
private:
    VideoGrid* g_;
};
}

VideoGrid::VideoGrid(AppContext& ctx, Style style, QWidget* p) : QListWidget(p), ctx_(ctx), style_(style) {
    setContextMenuPolicy(Qt::CustomContextMenu);
    setMouseTracking(true);
    setMovement(QListView::Static);
    setResizeMode(QListView::Adjust);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    verticalScrollBar()->setSingleStep(30);
    setFrameShape(QFrame::NoFrame);
    setSpacing(0);
    setUniformItemSizes(true);
    setItemDelegate(new CardDelegate(this));
    if (style == Cards) setViewMode(QListView::IconMode); else setViewMode(QListView::ListMode);
    // activations are queued: handlers may clear/refill this very list (that was a crash)
    auto fire = [this](QListWidgetItem* it) {
        const VideoInfo v = it->data(RoleVideo).value<VideoInfo>();
        QMetaObject::invokeMethod(this, [this, v] { emit playRequested(v); }, Qt::QueuedConnection);
    };
    connect(this, &QListWidget::itemClicked, this, fire);
    connect(this, &QWidget::customContextMenuRequested, this, &VideoGrid::showMenu);
    connect(verticalScrollBar(), &QScrollBar::valueChanged, this, [this](int v) { if (count() && v >= verticalScrollBar()->maximum() - 120) emit loadMoreRequested(); });
    connect(ctx_.thumbs, &ThumbnailService::ready, this, [this] { viewport()->update(); });
    relayout();
}
void VideoGrid::keyPressEvent(QKeyEvent* e) {
    if ((e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) && currentItem()) { const VideoInfo v = selectedVideo(); QMetaObject::invokeMethod(this, [this, v] { emit playRequested(v); }, Qt::QueuedConnection); return; }
    QListWidget::keyPressEvent(e);
}
void VideoGrid::relayout() {
    if (style_ == Cards) {
        const int w = viewport()->width();
        const int cols = qMax(1, w / 300);
        cw_ = qMax(240, w / cols - 1);
        setGridSize(QSize(cw_, (cw_ - 16) * 9 / 16 + 96));
    }
    doItemsLayout();
}
void VideoGrid::resizeEvent(QResizeEvent* e) { QListWidget::resizeEvent(e); relayout(); }

void VideoGrid::addOne(const VideoInfo& v) {
    auto* it = new QListWidgetItem(this);
    it->setData(RoleVideo, QVariant::fromValue(v));
    it->setToolTip(v.title);
    if (style_ == Cards) it->setSizeHint(QSize(cw_, (cw_ - 16) * 9 / 16 + 96)); else it->setSizeHint(QSize(300, 112));
}
void VideoGrid::setVideos(const QList<VideoInfo>& v) {
    const QSignalBlocker b(this);
    clear();
    ctx_.thumbs->cancelPending();
    append(v);
    verticalScrollBar()->setValue(0);
}
void VideoGrid::append(const QList<VideoInfo>& v) { for (const VideoInfo& x : v) addOne(x); relayout(); }
QList<VideoInfo> VideoGrid::videos() const {
    QList<VideoInfo> l;
    for (int i = 0; i < count(); ++i) l << videoAt(i);
    return l;
}
VideoInfo VideoGrid::videoAt(int r) const { return r >= 0 && r < count() ? item(r)->data(RoleVideo).value<VideoInfo>() : VideoInfo(); }
VideoInfo VideoGrid::selectedVideo() const { return videoAt(currentRow()); }

void VideoGrid::showMenu(const QPoint& pos) {
    QListWidgetItem* it = itemAt(pos);
    if (!it) return;
    const int rw = row(it);
    const VideoInfo v = videoAt(rw);
    QMenu m(this);
    m.addAction("Play", this, [this, v] { emit playRequested(v); });
    m.addSeparator();
    PlaylistService* pl = ctx_.playlists;
    m.addAction(pl->contains(PlaylistService::watchLater(), v.id) ? "Remove from Watch Later" : "Save to Watch Later", this, [pl, v] {
        if (pl->contains(PlaylistService::watchLater(), v.id)) pl->removeVideo(PlaylistService::watchLater(), v.id); else pl->add(PlaylistService::watchLater(), v);
    });
    m.addAction(pl->contains(PlaylistService::favorites(), v.id) ? "Remove from Liked" : "Add to Liked", this, [pl, v] {
        if (pl->contains(PlaylistService::favorites(), v.id)) pl->removeVideo(PlaylistService::favorites(), v.id); else pl->add(PlaylistService::favorites(), v);
    });
    QMenu* sub = m.addMenu("Save to playlist");
    for (const QString& n : pl->names()) {
        if (pl->isReserved(n)) continue;
        sub->addAction(n, this, [pl, n, v] { pl->add(n, v); });
    }
    sub->addAction("New playlist…", this, [this, pl, v] {
        bool ok = false;
        const QString n = QInputDialog::getText(this, "New playlist", "Name:", QLineEdit::Normal, QString(), &ok);
        if (ok && pl->create(n)) pl->add(n.trimmed(), v);
    });
    if (!v.channelId.isEmpty()) {
        SubscriptionService* ss = ctx_.subs;
        m.addAction(ss->isSubscribed(v.channelId) ? "Unsubscribe from " + v.channel : "Subscribe to " + v.channel, this, [ss, v] {
            if (ss->isSubscribed(v.channelId)) ss->unsubscribe(v.channelId); else ss->subscribe(v.channelId, v.channel);
        });
    }
    m.addAction("Copy link", this, [v] { QApplication::clipboard()->setText(v.url()); });
    if (removable_) { m.addSeparator(); m.addAction("Remove from this list", this, [this, rw] { emit removeRequested(rw); }); }
    m.exec(viewport()->mapToGlobal(pos));
}
