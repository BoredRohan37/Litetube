#include "AuthService.h"
#include "AppSettings.h"
#include "utilities/Logger.h"
#include <QCryptographicHash>
#include <QDesktopServices>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QTcpSocket>
#include <QRandomGenerator>
#include <QSslSocket>

static AppSettings& S() { return AppSettings::instance(); }
static QString b64url(const QByteArray& b) { return QString::fromLatin1(b.toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals)); }

AuthService::AuthService(QObject* p) : QObject(p) {}
bool AuthService::configured() const { return !S().str("clientId").trimmed().isEmpty(); }
bool AuthService::signedIn() const { return !S().str("authRefresh").isEmpty(); }
QString AuthService::name() const { return S().str("authName"); }

void AuthService::signIn() {
    if (!configured()) { emit message("Enter your Google OAuth client ID in Settings > Account first."); return; }
    if (!QSslSocket::supportsSsl()) { emit message("HTTPS support (OpenSSL DLLs) is missing, sign-in cannot work."); return; }
    server_.close();
    if (!server_.listen(QHostAddress::LocalHost, 0)) { emit message("Could not open a local port for the sign-in redirect."); return; }
    redirect_ = QString("http://127.0.0.1:%1").arg(server_.serverPort());
    QByteArray raw(48, 0);
    for (char& c : raw) c = char(QRandomGenerator::global()->bounded(256));
    verifier_ = b64url(raw);
    const QString challenge = b64url(QCryptographicHash::hash(verifier_.toLatin1(), QCryptographicHash::Sha256));
    QUrl url("https://accounts.google.com/o/oauth2/v2/auth");
    QUrlQuery q;
    q.addQueryItem("client_id", S().str("clientId").trimmed());
    q.addQueryItem("redirect_uri", redirect_);
    q.addQueryItem("response_type", "code");
    q.addQueryItem("scope", "https://www.googleapis.com/auth/youtube https://www.googleapis.com/auth/youtube.force-ssl");
    q.addQueryItem("code_challenge", challenge);
    q.addQueryItem("code_challenge_method", "S256");
    q.addQueryItem("access_type", "offline");
    q.addQueryItem("prompt", "consent");
    url.setQuery(q);
    connect(&server_, &QTcpServer::newConnection, this, [this] {
        QTcpSocket* sock = server_.nextPendingConnection();
        connect(sock, &QTcpSocket::readyRead, this, [this, sock] {
            const QByteArray req = sock->readAll();
            const QString line = QString::fromLatin1(req.left(req.indexOf("\r\n")));
            const QUrl u("http://x" + line.section(' ', 1, 1));
            const QUrlQuery q(u);
            const QString code = q.queryItemValue("code");
            const bool ok = !code.isEmpty();
            const QByteArray body = ok ? "<html><body style='font-family:sans-serif;background:#111;color:#eee;text-align:center;padding-top:80px'><h2>LiteTube is signed in.</h2>You can close this tab.</body></html>"
                                       : "<html><body style='font-family:sans-serif'>Sign-in was cancelled or failed. You can close this tab.</body></html>";
            sock->write("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\nContent-Length: " + QByteArray::number(body.size()) + "\r\n\r\n" + body);
            sock->disconnectFromHost();
            server_.close();
            if (ok) exchange(code); else emit message("Sign-in was cancelled.");
        });
    });
    QDesktopServices::openUrl(url);
    emit message("Finish signing in in your browser…");
}
void AuthService::signOut() {
    S().set("authRefresh", ""); S().set("authName", ""); access_.clear(); expiresAt_ = 0;
    emit stateChanged();
}
void AuthService::tokenRequest(const QByteArray& body, std::function<void(bool, const QString&)> done) {
    QNetworkRequest rq(QUrl("https://oauth2.googleapis.com/token"));
    rq.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QNetworkReply* r = nam_.post(rq, body);
    connect(r, &QNetworkReply::finished, this, [this, r, done] {
        r->deleteLater();
        const QJsonObject o = QJsonDocument::fromJson(r->readAll()).object();
        if (o.contains("access_token")) {
            access_ = o["access_token"].toString();
            expiresAt_ = QDateTime::currentSecsSinceEpoch() + o["expires_in"].toInt(3600) - 60;
            if (o.contains("refresh_token")) S().set("authRefresh", o["refresh_token"].toString());
            done(true, QString());
        } else done(false, o["error_description"].toString(o["error"].toString(r->errorString())));
    });
}
void AuthService::exchange(const QString& code) {
    QUrlQuery b;
    b.addQueryItem("client_id", S().str("clientId").trimmed());
    if (!S().str("clientSecret").trimmed().isEmpty()) b.addQueryItem("client_secret", S().str("clientSecret").trimmed());
    b.addQueryItem("code", code); b.addQueryItem("code_verifier", verifier_);
    b.addQueryItem("redirect_uri", redirect_); b.addQueryItem("grant_type", "authorization_code");
    tokenRequest(b.toString(QUrl::FullyEncoded).toUtf8(), [this](bool ok, const QString& err) {
        if (!ok) { LOGW("token exchange failed: " + err); emit message("Sign-in failed: " + err); return; }
        fetchProfile();
    });
}
void AuthService::fetchProfile() {
    withToken([this](const QString& tok, const QString& err) {
        if (!err.isEmpty()) { emit message(err); return; }
        QNetworkRequest rq(QUrl("https://www.googleapis.com/youtube/v3/channels?part=snippet&mine=true"));
        rq.setRawHeader("Authorization", "Bearer " + tok.toUtf8());
        QNetworkReply* r = nam_.get(rq);
        connect(r, &QNetworkReply::finished, this, [this, r] {
            r->deleteLater();
            const QJsonArray items = QJsonDocument::fromJson(r->readAll()).object()["items"].toArray();
            S().set("authName", items.isEmpty() ? QStringLiteral("Signed in") : items[0].toObject()["snippet"].toObject()["title"].toString());
            emit stateChanged();
            emit message("Signed in as " + name());
        });
    });
}
void AuthService::withToken(std::function<void(const QString&, const QString&)> cb) {
    if (!access_.isEmpty() && QDateTime::currentSecsSinceEpoch() < expiresAt_) { cb(access_, QString()); return; }
    if (!signedIn()) { cb(QString(), "Not signed in"); return; }
    waiting_ << cb;
    if (refreshing_) return;
    refreshing_ = true;
    QUrlQuery b;
    b.addQueryItem("client_id", S().str("clientId").trimmed());
    if (!S().str("clientSecret").trimmed().isEmpty()) b.addQueryItem("client_secret", S().str("clientSecret").trimmed());
    b.addQueryItem("refresh_token", S().str("authRefresh")); b.addQueryItem("grant_type", "refresh_token");
    tokenRequest(b.toString(QUrl::FullyEncoded).toUtf8(), [this](bool ok, const QString& err) {
        refreshing_ = false;
        auto list = waiting_; waiting_.clear();
        if (!ok && (err.contains("invalid_grant") || err.contains("revoked"))) signOut();
        for (auto& f : list) f(ok ? access_ : QString(), ok ? QString() : err);
    });
}
