#pragma once
#include <QObject>
#include <QSettings>
#include <QVariantMap>
#include <memory>

class AppSettings : public QObject {
    Q_OBJECT
public:
    static AppSettings& instance();
    void init(const QString& dataDir);
    QString dataDir() const { return dir_; }
    QVariant value(const QString& k) const { return s_ ? s_->value(k, defs_.value(k)) : defs_.value(k); }
    QString str(const QString& k) const { return value(k).toString(); }
    int num(const QString& k) const { return value(k).toInt(); }
    double dbl(const QString& k) const { return value(k).toDouble(); }
    bool flag(const QString& k) const { return value(k).toBool(); }
    void set(const QString& k, const QVariant& v);
    void resetAll();
signals:
    void changed(const QString& key);
private:
    AppSettings() = default;
    QString dir_;
    std::unique_ptr<QSettings> s_;
    QVariantMap defs_;
};
