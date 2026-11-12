#pragma once
#include "FeedPage.h"
// Home (mode 0) and Trending (mode 1)
class HomePage : public FeedPage {
    Q_OBJECT
public:
    enum Mode { Home, Trending };
    HomePage(AppContext& ctx, Mode m, QWidget* p = nullptr);
protected:
    int startRequest(int chip, const QString& token) override;
private:
    Mode mode_;
    QStringList chips_;
};
