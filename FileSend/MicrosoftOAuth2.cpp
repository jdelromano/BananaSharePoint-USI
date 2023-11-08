// MicrosoftOAuth2.cpp
#include "MicrosoftOAuth2.h"
#include "Microsoft_secrets.h"

MicrosoftOAuth2::MicrosoftOAuth2(QObject* parent) : OAuth2Base(parent), oauth2(this)
{
    // Create a QOAuthHttpServerReplyHandler to handle the OAuth2 replies. Listens on port 3000 for incoming HTTP requests.
    auto authReply = new QOAuthHttpServerReplyHandler(Microsoft_secrets::port, this);

    // Configure the OAuth2 settings for Microsoft authentication
    // Set the authorization and access token URLs, as well as the desired scope.
    oauth2.setReplyHandler(authReply);
    oauth2.setAuthorizationUrl(QUrl(Microsoft_secrets::auth_uri));
    oauth2.setAccessTokenUrl(QUrl(Microsoft_secrets::token_uri));
    oauth2.setScope(Microsoft_secrets::scope);

    // Connect a slot to the statusChanged signal of the OAuth2 object.
    connect(&oauth2, &QOAuth2AuthorizationCodeFlow::statusChanged, this, [=](QAbstractOAuth::Status status) {
        if (status == QAbstractOAuth::Status::Granted) {
            emit authenticationSuccess();
        }
        else if (status == QAbstractOAuth::Status::TemporaryCredentialsReceived) {
            getAccessToken();
        }
        else if (status == QAbstractOAuth::Status::RefreshingToken) {
            // Handle token refresh if needed.
        }
        else if (status == QAbstractOAuth::Status::Unauthenticated) {
            // Handle unauthenticated state.
        }
        // Add more conditions as needed for different states.
    });

    // Configure a function to modify the OAuth2 parameters before sending a request.
    oauth2.setModifyParametersFunction([&](QAbstractOAuth::Stage stage, QMultiMap<QString, QVariant>* parameters) {
        parameters->insert("client_id", Microsoft_secrets::client_id);
        parameters->insert("client_secret", Microsoft_secrets::client_secret);
    });

    // Connect the authorizeWithBrowser signal of the OAuth2 object to QDesktopServices::openUrl,
    // which will open the authorization URL in the user's default web browser.
    connect(&oauth2, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, &QDesktopServices::openUrl);

    connect(&oauth2, &QAbstractOAuth2::authorizationCallbackReceived, this, &MicrosoftOAuth2::handleAuthorizationCallback);
    connect(&m_manager, &QNetworkAccessManager::finished, this, &MicrosoftOAuth2::onManagerFinished);
}

void MicrosoftOAuth2::getAccessToken() {
    QNetworkRequest request(QUrl(Microsoft_secrets::token_uri));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QByteArray postData;
    postData.append("grant_type=client_credentials");
    postData.append("&client_id=" + Microsoft_secrets::client_id);
    postData.append("&client_secret=" +Microsoft_secrets::client_secret);
    postData.append("&scope=" + Microsoft_secrets::scope);

    m_manager.post(request, postData);
}

void MicrosoftOAuth2::onManagerFinished(QNetworkReply* reply) {
    // Handle network replies specific to Microsoft OAuth2 here.
    // You can keep the relevant code from the original Authenticator class.
    // This method should handle the responses for Microsoft-specific URLs.
    // For example, the URL comparison and response parsing logic.

    // You can also emit signals here to indicate success or failure.
    // For example, emit authenticationSuccess() or authenticationError() signals.
    // Make sure to handle potential errors and parse the response accordingly.
    if (reply->error()) {
        qDebug() << "Error: " << reply->error();
        emit authenticationError(reply->errorString());
    } else {
        QByteArray data = reply->readAll();
        QString responseString = QString::fromUtf8(data);

        // Parse the JSON response
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseString.toUtf8());

        if (jsonResponse.isObject()) {
            QJsonObject jsonObject = jsonResponse.object();
            if (jsonObject.contains("access_token") && jsonObject["access_token"].isString()) {
                accessToken = jsonObject["access_token"].toString();
                emit authenticationSuccess();
            }
        }
    }

    reply->deleteLater();
}

//  putRequest if neededfor AccessToken. (If needed only)
