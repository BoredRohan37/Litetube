#pragma once
#include <QString>
class QWidget;
namespace ErrorHandler {
QString friendly(const QString& raw);
bool showRetry(QWidget* parent, const QString& title, const QString& raw);
}
