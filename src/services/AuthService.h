#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QTcpServer>
#include <functional>

// Google OAuth 2.0 for installed apps (system browser + loopback + PKCE). The user supplies their own client ID.
class AuthService : public QObject {
    Q_OBJECT
public:
    explicit AuthService(QObject* p = nullptr);
    bool configured() const;
    bool signedIn() const;
    QString name() const;
    void signIn();
    void signOut();
    // Calls back with a valid access token, refreshing it when needed. err is empty on success.
    void withToken(std::function<void(const QString& token, const QString& err)> cb);
    QNetworkAccessManager* nam() { return &nam_; }
signals:
    void stateChanged();
    void message(const QString& text);
private:
    void exchange(const QString& code);
    void tokenRequest(const QByteArray& body, std::function<void(bool, const QString&)> done);
    void fetchProfile();
    QNetworkAccessManager nam_;
    QTcpServer server_;
    QString verifier_, redirect_, access_;
    qint64 expiresAt_ = 0;
    QList<std::function<void(const QString&, const QString&)>> waiting_;
    bool refreshing_ = false;
};
