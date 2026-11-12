#pragma once
#include <QWidget>
#include "youtube/CommentsService.h"
class QVBoxLayout; class QLabel; class QComboBox; class QPushButton; class QLineEdit; class AccountApi;

class CommentsWidget : public QWidget {
    Q_OBJECT
public:
    CommentsWidget(CommentsService* svc, AccountApi* account, QWidget* p = nullptr);
    void setVideo(const QString& videoId);
    void clear();
signals:
    void seekRequested(double seconds);
private slots:
    void onLoaded(int id, const QList<Comment>& c, const QString& next);
    void onFailed(int id, const QString& msg);
private:
    void request(bool more);
    void addComment(const Comment& c);
    CommentsService* svc_;
    AccountApi* account_;
    QString video_, next_;
    int req_ = -1;
    QLabel* header_; QLabel* status_;
    QComboBox* sort_; QPushButton* more_; QLineEdit* input_; QPushButton* post_;
    QVBoxLayout* list_;
};
