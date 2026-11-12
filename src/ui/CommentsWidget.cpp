#include "CommentsWidget.h"
#include "youtube/AccountApi.h"
#include "utilities/ErrorHandler.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include "TextUtil.h"

CommentsWidget::CommentsWidget(CommentsService* svc, AccountApi* account, QWidget* p) : QWidget(p), svc_(svc), account_(account) {
    auto* v = new QVBoxLayout(this); v->setContentsMargins(0, 8, 0, 0);
    auto* top = new QHBoxLayout;
    header_ = new QLabel("Comments"); header_->setObjectName("h2");
    sort_ = new QComboBox; sort_->addItems({"Top comments", "Newest first"});
    top->addWidget(header_); top->addStretch(1); top->addWidget(sort_);
    v->addLayout(top);
    auto* postRow = new QHBoxLayout;
    input_ = new QLineEdit; input_->setPlaceholderText("Add a comment…");
    post_ = new QPushButton("Comment"); post_->setObjectName("primary");
    postRow->addWidget(input_, 1); postRow->addWidget(post_);
    v->addLayout(postRow);
    status_ = new QLabel; status_->setObjectName("muted"); status_->setWordWrap(true);
    v->addWidget(status_);
    auto* holder = new QWidget; list_ = new QVBoxLayout(holder); list_->setContentsMargins(0, 0, 0, 0); list_->setSpacing(14);
    v->addWidget(holder);
    more_ = new QPushButton("Show more comments"); more_->hide();
    v->addWidget(more_, 0, Qt::AlignHCenter);
    connect(svc_, &CommentsService::loaded, this, &CommentsWidget::onLoaded);
    connect(svc_, &CommentsService::failed, this, &CommentsWidget::onFailed);
    connect(sort_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) { if (!video_.isEmpty()) { clear(); request(false); } });
    connect(more_, &QPushButton::clicked, this, [this] { request(true); });
    auto doPost = [this] {
        const QString t = input_->text().trimmed();
        if (t.isEmpty() || !account_->available()) return;
        post_->setEnabled(false);
        account_->postComment(video_, t, [this](const QString& err) {
            post_->setEnabled(true);
            if (err.isEmpty()) { input_->clear(); status_->setText("Comment posted."); } else status_->setText("Could not post: " + ErrorHandler::friendly(err));
        });
    };
    connect(post_, &QPushButton::clicked, this, doPost);
    connect(input_, &QLineEdit::returnPressed, this, doPost);
}
void CommentsWidget::clear() {
    if (req_ >= 0) svc_->cancel(req_);
    req_ = -1; next_.clear(); more_->hide(); status_->clear();
    while (QLayoutItem* it = list_->takeAt(0)) { delete it->widget(); delete it; }
}
void CommentsWidget::setVideo(const QString& id) {
    clear(); video_ = id;
    const bool can = account_->available();
    input_->setVisible(can); post_->setVisible(can);
    request(false);
}
void CommentsWidget::request(bool more) {
    status_->setText(more ? "Loading…" : "Loading comments…");
    more_->hide();
    req_ = svc_->load(video_, sort_->currentIndex() == 1, more ? next_ : QString());
}
void CommentsWidget::onLoaded(int id, const QList<Comment>& c, const QString& next) {
    if (id != req_) return;
    req_ = -1; next_ = next;
    status_->setText(c.isEmpty() && list_->count() == 0 ? "No comments." : QString());
    for (const Comment& k : c) addComment(k);
    more_->setVisible(!next.isEmpty());
}
void CommentsWidget::onFailed(int id, const QString& msg) {
    if (id != req_) return;
    req_ = -1;
    status_->setText(ErrorHandler::friendly(msg));
}
void CommentsWidget::addComment(const Comment& c) {
    auto* w = new QWidget; auto* l = new QVBoxLayout(w); l->setContentsMargins(0, 0, 0, 0); l->setSpacing(3);
    auto* head = new QLabel(QString("<b>%1</b> <span style='color:#aaa'> · %2</span>").arg(c.author.toHtmlEscaped(), c.when));
    auto* body = new QLabel(TextUtil::linkify(c.text)); body->setWordWrap(true); body->setTextFormat(Qt::RichText); body->setTextInteractionFlags(Qt::TextBrowserInteraction);
    connect(body, &QLabel::linkActivated, this, [this](const QString& l) { if (l.startsWith("seek:")) emit seekRequested(l.mid(5).toDouble()); });
    auto* foot = new QLabel(QString("<span style='color:#aaa'>👍 %1</span>").arg(c.likes));
    l->addWidget(head); l->addWidget(body); l->addWidget(foot);
    if (c.replyCount > 0) {
        auto* btn = new QPushButton(QString("▾ %1 replies").arg(c.replyCount)); btn->setObjectName("flat"); btn->setCursor(Qt::PointingHandCursor);
        auto* box = new QWidget; auto* bl = new QVBoxLayout(box); bl->setContentsMargins(24, 4, 0, 0);
        for (const Comment& r : c.replies) {
            auto* rl = new QLabel(QString("<b>%1</b> <span style='color:#aaa'>· %2</span><br>%3").arg(r.author.toHtmlEscaped(), r.when, TextUtil::linkify(r.text)));
            rl->setWordWrap(true); rl->setTextFormat(Qt::RichText); bl->addWidget(rl);
        }
        if (c.replies.size() < c.replyCount) { auto* m = new QLabel(QString("<span style='color:#aaa'>…and %1 more on YouTube</span>").arg(c.replyCount - c.replies.size())); bl->addWidget(m); }
        box->hide();
        connect(btn, &QPushButton::clicked, box, [box, btn, n = c.replyCount] { box->setVisible(!box->isVisible()); btn->setText(QString(box->isVisible() ? "▴ %1 replies" : "▾ %1 replies").arg(n)); });
        l->addWidget(btn, 0, Qt::AlignLeft); l->addWidget(box);
    }
    list_->addWidget(w);
}
