#include "GoogleOAuth2.h"
#include "Google_secrets.h"
#include <QUrlQuery>

GoogleOAuth2::GoogleOAuth2(QObject* parent) : OAuth2Base(parent)
{
    this->google = new QOAuth2AuthorizationCodeFlow;
    this->google->setScope("https://mail.google.com/ https://www.googleapis.com/auth/drive");

    connect(this->google, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, &QDesktopServices::openUrl);

    const QUrl authUri=Google_secrets::auth_uri;
    const QString clientId =Google_secrets::client_id;
    const QUrl tokenUri= Google_secrets::token_uri;
    const QString clientSecret = Google_secrets::client_secret;
    const QUrl redirectUri= Google_secrets::redirect_uris;
    const auto port = Google_secrets::port; // Get the port

    this->google->setAuthorizationUrl(authUri);
    this->google->setClientIdentifier(clientId);
    this->google->setAccessTokenUrl(tokenUri);
    this->google->setClientIdentifierSharedKey(clientSecret);

    //qDebug() << "fields: \n" << authUri << "\n" << clientId << "\n" << tokenUri << "\n" << clientSecret << "\n port: " << port;

    this->google->setModifyParametersFunction([](QAbstractOAuth::Stage stage, QMultiMap<QString, QVariant> * parameters) {
        // Percent-decode the "code" parameter so Google can match it
        if (stage == QAbstractOAuth::Stage::RequestingAccessToken) {
            QByteArray code = parameters->value("code").toByteArray();
            //qDebug() << "code after decode: " << QUrl::fromPercentEncoding(code);
            parameters->replace("code", QUrl::fromPercentEncoding(code));
        }
    });

    this->replyHandler = new QOAuthHttpServerReplyHandler(port, this);
    this->google->setReplyHandler(replyHandler);

    connect(this, &GoogleOAuth2::gotToken, this, &GoogleOAuth2::getFilesList);


    connect(this->google, &QOAuth2AuthorizationCodeFlow::granted, [=](){
        const QString token = this->google->token();
        qDebug() << "Google token: "<< token;
        //qDebug() << "got token";
        QDateTime date = this->google->expirationAt();
        qDebug() << "expire date: " << date;
        emit gotToken();
    });
}

void GoogleOAuth2::authenticate() {
    // Construct the authorization URL
    QUrl authUrl(Google_secrets::auth_uri);

    QUrlQuery urlQuery;
    urlQuery.addQueryItem("client_id", Google_secrets::client_id);
    urlQuery.addQueryItem("redirect_uri", Google_secrets::redirect_uris);
    urlQuery.addQueryItem("response_type", "code");
    urlQuery.addQueryItem("scope", "https://mail.google.com/ https://www.googleapis.com/auth/drive");

    authUrl.setQuery(urlQuery.query());

    // Open the default web browser for user authentication
    emit authorizeWithBrowser(authUrl);
}

void GoogleOAuth2::handleCallback(const QUrl& callbackUrl) {
    // Extract code from the callback URL
    QString code = QUrlQuery(callbackUrl).queryItemValue("code");

    if (!code.isEmpty()) {
        // Perform token exchange logic using the obtained code
        // ...

        // Once the token exchange is complete, emit signals accordingly
        emit authenticationSuccess();
    } else {
        emit authenticationError("Failed to obtain authorization code");
    }
}
