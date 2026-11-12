#pragma once
#include <QJsonObject>
#include <QJsonDocument>
#include <QSaveFile>
#include <QFile>
namespace JsonStore {
inline QJsonObject read(const QString& path) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) return QJsonObject();
    return QJsonDocument::fromJson(f.readAll()).object();
}
inline bool write(const QString& path, const QJsonObject& o) {
    QSaveFile f(path);
    if (!f.open(QIODevice::WriteOnly)) return false;
    f.write(QJsonDocument(o).toJson(QJsonDocument::Compact));
    return f.commit();
}
}
