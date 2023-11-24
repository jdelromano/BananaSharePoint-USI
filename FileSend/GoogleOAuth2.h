#ifndef GOOGLEOAUTH2_H
#define GOOGLEOAUTH2_H
#include "OAuth2Base.h"
#include <QObject>
#include "qgridlayout.h"
#include "qobject.h"
#include <QOAuth2AuthorizationCodeFlow>
#include "qoauth2authorizationcodeflow.h"
#include "qtextedit.h"
#include <QDesktopServices>
#include <QDesktopServices>
#include <QJsonDocument>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QOAuthHttpServerReplyHandler>
#include <QNetworkReply>

class GoogleOAuth2 : public OAuth2Base {
    Q_OBJECT

public:
    GoogleOAuth2(QObject* parent = nullptr);
    virtual ~GoogleOAuth2();
    void go();
    void getFile(QString id);
    void saveFile(QString id, QByteArray new_text);
    QMap<QString, QString> files;
    QOAuthOobReplyHandler *getReplyHandler() const;
    void authenticate() override;
    void handleCallback(const QUrl& callbackUrl) override;

public slots:
    void getUserEmail();
    void getFilesList();
    void readFiles();
private slots:
    // Slot to handle network replies specific to Google OAuth2
    void handleGoogleNetworkReply(QNetworkReply* reply);

signals:
    void getFilesDone();
    void gotToken();
    void getFileContentDone(QNetworkReply * reply, QString id);
    void createFileChannel(QString id);
    void authorizeWithBrowser(const QUrl& url);
private:
    QOAuth2AuthorizationCodeFlow * google;
    QOAuthOobReplyHandler * replyHandler;
};

#endif // GOOGLEOAUTH2_H
