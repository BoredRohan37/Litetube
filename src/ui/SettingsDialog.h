#pragma once
#include <QDialog>
#include <functional>
#include <QList>
#include "services/AppContext.h"
class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    SettingsDialog(AppContext& ctx, QWidget* p = nullptr, int tab = 0);
private:
    void build(int tab);
    void load();
    void save();
    AppContext& ctx_;
    QList<std::function<void()>> loaders_, savers_;
};
