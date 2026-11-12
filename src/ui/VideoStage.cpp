#include "VideoStage.h"
#include <QLabel>
#include <QVBoxLayout>

VideoStage::VideoStage(QWidget* p) : QWidget(p) {
    auto* l = new QVBoxLayout(this);
    l->setContentsMargins(0, 0, 0, 0);
    msg_ = new QLabel(this);
    msg_->setAlignment(Qt::AlignCenter);
    msg_->setWordWrap(true);
    msg_->setStyleSheet("background:#000;color:#ddd;font-size:15px;padding:12px;");
    l->addWidget(msg_);
    setMinimumSize(200, 112);
}
void VideoStage::showMessage(const QString& text, const QPixmap& poster) {
    wantVideo_ = false;
    if (!poster.isNull()) { msg_->setPixmap(poster.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)); msg_->setToolTip(text); }
    else { msg_->setPixmap(QPixmap()); msg_->setText(text); }
    emit geometryChanged();
}
void VideoStage::showVideo() { wantVideo_ = true; emit geometryChanged(); }
