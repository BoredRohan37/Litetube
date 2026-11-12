#include "PlaylistPage.h"
#include "VideoGrid.h"
#include "services/PlaylistService.h"
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QInputDialog>
#include <QMessageBox>
#include "services/AuthService.h"
#include "youtube/AccountApi.h"
#include "utilities/ErrorHandler.h"

PlaylistPage::PlaylistPage(AppContext& ctx, QWidget* p) : QWidget(p), ctx_(ctx) {
    auto* h = new QHBoxLayout(this);
    auto* left = new QVBoxLayout;
    auto* t = new QLabel("Library"); t->setObjectName("h1");
    lists_ = new QListWidget; lists_->setFixedWidth(210);
    auto* newBtn = new QPushButton("＋ New playlist");
    left->addWidget(t); left->addWidget(lists_, 1); left->addWidget(newBtn);
    auto* right = new QVBoxLayout;
    title_ = new QLabel; title_->setObjectName("h1");
    auto* bar = new QHBoxLayout;
    play_ = new QPushButton("▶ Play all"); play_->setObjectName("primary");
    shuffle_ = new QPushButton("Shuffle"); repeat_ = new QPushButton("Repeat"); repeat_->setCheckable(true);
    up_ = new QPushButton("▲ Up"); down_ = new QPushButton("▼ Down");
    rename_ = new QPushButton("Rename"); del_ = new QPushButton("Delete list");
    for (auto* b : {play_, shuffle_, repeat_, up_, down_, rename_, del_}) bar->addWidget(b);
    bar->addStretch(1);
    grid_ = new VideoGrid(ctx, VideoGrid::Rows);
    grid_->setRemovable(true);
    right->addWidget(title_); right->addLayout(bar); right->addWidget(grid_, 1);
    h->addLayout(left); h->addLayout(right, 1);

    connect(ctx_.playlists, &PlaylistService::changed, this, [this] { if (!busy_) { refreshLists(); refreshVideos(); } });
    connect(lists_, &QListWidget::currentRowChanged, this, [this](int) { refreshVideos(); });
    connect(newBtn, &QPushButton::clicked, this, [this] {
        bool ok = false; const QString n = QInputDialog::getText(this, "New playlist", "Name:", QLineEdit::Normal, QString(), &ok);
        if (ok && ctx_.playlists->create(n)) selectList(n.trimmed());
    });
    connect(play_, &QPushButton::clicked, this, [this] { emit playQueueRequested(grid_->videos(), qMax(0, grid_->currentRow()), false, repeat_->isChecked()); });
    connect(shuffle_, &QPushButton::clicked, this, [this] { emit playQueueRequested(grid_->videos(), qMax(0, grid_->currentRow()), true, repeat_->isChecked()); });
    connect(grid_, &VideoGrid::playRequested, this, [this](const VideoInfo& v) {
        const auto l = grid_->videos(); int i = 0;
        for (; i < l.size(); ++i) if (l[i].id == v.id) break;
        emit playQueueRequested(l, i, false, repeat_->isChecked());
    });
    connect(grid_, &VideoGrid::removeRequested, this, [this](int row) { ctx_.playlists->removeAt(current(), row); });
    auto move = [this](int d) {
        const int r = grid_->currentRow();
        if (r < 0) return;
        ctx_.playlists->move(current(), r, r + d);
        grid_->setCurrentRow(qBound(0, r + d, grid_->count() - 1));
    };
    connect(up_, &QPushButton::clicked, this, [move] { move(-1); });
    connect(down_, &QPushButton::clicked, this, [move] { move(1); });
    connect(rename_, &QPushButton::clicked, this, [this] {
        bool ok = false; const QString n = QInputDialog::getText(this, "Rename playlist", "Name:", QLineEdit::Normal, current(), &ok);
        if (ok) { const QString old = current(); if (ctx_.playlists->rename(old, n)) selectList(n.trimmed()); }
    });
    connect(del_, &QPushButton::clicked, this, [this] {
        if (QMessageBox::question(this, "Delete playlist", "Delete \"" + current() + "\"?") == QMessageBox::Yes) ctx_.playlists->remove(current());
    });
    refreshLists();
}
bool PlaylistPage::isRemote() const { return current().startsWith("yt:"); }
void PlaylistPage::loadRemote(const QString& id) {
    title_->setText(id == "yt:liked" ? "Liked videos (YouTube)" : lists_->currentItem()->text());
    grid_->setVideos({});
    auto cb = [this](const QList<VideoInfo>& v, const QString&, const QString& err) {
        if (!err.isEmpty()) { title_->setText(title_->text() + " - " + ErrorHandler::friendly(err)); return; }
        grid_->setVideos(v);
    };
    if (id == "yt:liked") ctx_.account->likedVideos(QString(), cb); else ctx_.account->playlistVideos(id.mid(3), QString(), cb);
}
QString PlaylistPage::current() const { return lists_->currentItem() ? lists_->currentItem()->data(Qt::UserRole).toString() : QString(); }
void PlaylistPage::refreshLists() {
    const QString cur = current();
    busy_ = true;
    lists_->blockSignals(true);
    lists_->clear();
    for (const QString& n : ctx_.playlists->names()) {
        auto* it = new QListWidgetItem(QString("%1 (%2)").arg(n).arg(ctx_.playlists->count(n)), lists_);
        it->setData(Qt::UserRole, n);
    }
    if (ctx_.account->available()) {
        auto* h = new QListWidgetItem("— YouTube account —", lists_); h->setFlags(Qt::NoItemFlags);
        auto* liked = new QListWidgetItem("☁ Liked videos", lists_); liked->setData(Qt::UserRole, "yt:liked");
        static QList<PlaylistInfo> cache;   // refreshed when the list is rebuilt after sign-in
        for (const PlaylistInfo& pi : cache) { auto* it = new QListWidgetItem("☁ " + pi.title, lists_); it->setData(Qt::UserRole, "yt:" + pi.id); }
        if (cache.isEmpty()) ctx_.account->playlists([this](const QList<PlaylistInfo>& l, const QString&) {
            if (l.isEmpty()) return;
            bool have = false; for (int i = 0; i < lists_->count(); ++i) if (lists_->item(i)->data(Qt::UserRole).toString().startsWith("yt:") && lists_->item(i)->data(Qt::UserRole).toString() != "yt:liked") have = true;
            if (have) return;
            for (const PlaylistInfo& pi : l) { auto* it = new QListWidgetItem("☁ " + pi.title, lists_); it->setData(Qt::UserRole, "yt:" + pi.id); }
        });
    }
    lists_->blockSignals(false);
    int row = 0;
    for (int i = 0; i < lists_->count(); ++i) if (lists_->item(i)->data(Qt::UserRole).toString() == cur) row = i;
    lists_->blockSignals(true); lists_->setCurrentRow(row); lists_->blockSignals(false);
    busy_ = false;
}
void PlaylistPage::selectList(const QString& name) {
    refreshLists();
    for (int i = 0; i < lists_->count(); ++i) if (lists_->item(i)->data(Qt::UserRole).toString() == name) lists_->setCurrentRow(i);
    refreshVideos();
}
void PlaylistPage::refreshVideos() {
    const QString n = current();
    if (n.startsWith("yt:")) {
        loadRemote(n);
        for (auto* b : {rename_, del_, up_, down_}) b->setEnabled(false);
        for (auto* b : {play_, shuffle_}) b->setEnabled(true);
        grid_->setRemovable(false);
        return;
    }
    grid_->setRemovable(true);
    title_->setText(n);
    const int keep = grid_->currentRow();
    grid_->setVideos(ctx_.playlists->videos(n));
    if (keep >= 0 && keep < grid_->count()) grid_->setCurrentRow(keep);
    const bool reserved = ctx_.playlists->isReserved(n);
    rename_->setEnabled(!reserved); del_->setEnabled(!reserved);
    const bool any = grid_->count() > 0;
    for (auto* b : {play_, shuffle_, up_, down_}) b->setEnabled(any);
}
