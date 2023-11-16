#ifndef OAUTH2BASE_H
#define OAUTH2BASE_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>

class OAuth2Base : public QObject
{
    Q_OBJECT

public:
    OAuth2Base(QObject *parent = nullptr);
    virtual ~OAuth2Base();

    // Methods to initiate authentication, handle the callback, and obtain access tokens.
    virtual void authenticate() = 0;
    QString getAccessToken() const;

signals:
/*   void authenticationSuccess();
   void authenticationError(const QString &error)*/;

protected:
    // OAuth2 configuration variables such as client ID, client secret, redirect URI, etc.
    QString clientId;
    QString clientSecret;
    QString redirectUri;
    QString accessToken;

    // Methods for making HTTP requests (e.g., to exchange code for an access token).
    QNetworkAccessManager *networkManager;

private slots:
    // Slot to handle network replies.
};

#endif // OAUTH2BASE_H
