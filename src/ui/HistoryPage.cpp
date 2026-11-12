#include "HistoryPage.h"
#include "VideoGrid.h"
#include "services/HistoryService.h"
#include <QTabWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>

HistoryPage::HistoryPage(AppContext& ctx, QWidget* p) : QWidget(p), ctx_(ctx) {
    auto* v = new QVBoxLayout(this);
    auto* tabs = new QTabWidget;
    auto* w1 = new QWidget; auto* l1 = new QVBoxLayout(w1);
    auto* b1 = new QHBoxLayout; auto* clr = new QPushButton("Clear watch history"); b1->addWidget(clr); b1->addStretch(1);
    grid_ = new VideoGrid(ctx, VideoGrid::Rows); grid_->setRemovable(true);
    l1->addLayout(b1); l1->addWidget(grid_, 1);
    auto* w2 = new QWidget; auto* l2 = new QVBoxLayout(w2);
    auto* b2 = new QHBoxLayout; auto* clr2 = new QPushButton("Clear search history"); b2->addWidget(clr2); b2->addStretch(1);
    searches_ = new QListWidget;
    l2->addLayout(b2); l2->addWidget(searches_, 1);
    tabs->addTab(w1, "Watch history"); tabs->addTab(w2, "Search history");
    v->addWidget(tabs);
    connect(clr, &QPushButton::clicked, this, [this] { if (QMessageBox::question(this, "Clear history", "Clear the whole watch history?") == QMessageBox::Yes) ctx_.history->clearWatch(); });
    connect(clr2, &QPushButton::clicked, this, [this] { ctx_.history->clearSearches(); });
    connect(grid_, &VideoGrid::playRequested, this, &HistoryPage::playRequested);
    connect(grid_, &VideoGrid::removeRequested, this, [this](int r) { ctx_.history->removeWatch(grid_->videoAt(r).id); });
    connect(searches_, &QListWidget::itemActivated, this, [this](QListWidgetItem* it) { emit searchRequested(it->text()); });
    connect(ctx_.history, &HistoryService::changed, this, &HistoryPage::refresh);
    refresh();
}
void HistoryPage::refresh() {
    QList<VideoInfo> l;
    for (const WatchEntry& e : ctx_.history->watch()) { VideoInfo v = e.v; v.published = Format::ago(e.at); l << v; }
    grid_->setVideos(l);
    searches_->clear();
    searches_->addItems(ctx_.history->searches());
}
