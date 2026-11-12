#pragma once
#include "FeedPage.h"
class SearchPage : public FeedPage {
    Q_OBJECT
public:
    explicit SearchPage(AppContext& ctx, QWidget* p = nullptr);
    void search(const QString& q);
    QString query() const { return q_; }
protected:
    int startRequest(int chip, const QString& token) override;
private:
    QString q_;
};
