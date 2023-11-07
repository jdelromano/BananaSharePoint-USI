
#include "GoogleAPIIntegration.h"
#include "QJsonDocument"
#include "QJsonObject"

GoogleAPIIntegration::GoogleAPIIntegration(QObject *parent) : QObject(parent) {
    googleApiWrapper = new GoogleAPIWrapper(this);
    googleOAuth2 = new GoogleOAuth2(this);
    // Configure and connect signals/slots as needed
    configureGoogleAPI();
}

GoogleAPIIntegration::~GoogleAPIIntegration() {
    delete googleApiWrapper;
    delete googleOAuth2;
}

void GoogleAPIIntegration::configureGoogleAPI() {
    // Set up Google API Wrapper
    googleApiWrapper->setGoogleOAuth2(googleOAuth2);

    // Set up Google OAuth2
    googleOAuth2->setClientId("your_google_client_id");
    googleOAuth2->setClientSecret("your_google_client_secret");
    googleOAuth2->setRedirectUri("your_google_redirect_uri");

    connect(googleOAuth2, &OAuth2Base::authenticationSuccess, [this]() {
        // Handle Google authentication success.
        googleApiWrapper->go();
    });

    connect(googleOAuth2, &OAuth2Base::authenticationError, [this](const QString& error) {
        // Handle Google authentication error.
    });

    // Additional signal/slot connections as needed
}

void GoogleAPIIntegration::authenticateWithGoogle() {
    googleOAuth2->authenticate();
}

void GoogleAPIIntegration::getUserEmail() {
    googleApiWrapper->getUserEmail();
}

void GoogleAPIIntegration::getFilesList() {
    googleApiWrapper->getFilesList();
}

void GoogleAPIIntegration::readFiles() {
    googleApiWrapper->readFiles();
}

void GoogleAPIIntegration::getFile(QString id) {
    googleApiWrapper->getFile(id);
}

void GoogleAPIIntegration::saveFile(QString id, QByteArray new_text) {
    googleApiWrapper->saveFile(id, new_text);
}
