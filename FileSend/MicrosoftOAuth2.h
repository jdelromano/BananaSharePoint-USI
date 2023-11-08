// MicrosoftOAuth2.h
#ifndef MICROSOFTOAUTH2_H
#define MICROSOFTOAUTH2_H

#include "OAuth2Base.h"

class MicrosoftOAuth2 : public OAuth2Base {
    Q_OBJECT

public:
    MicrosoftOAuth2(QObject* parent = nullptr);
    ~MicrosoftOAuth2();

    // Override the authenticate and handleCallback methods for Microsoft-specific behavior.

private slots:
    // Slot to handle network replies specific to Microsoft OAuth2.
    void onManagerFinished(QNetworkReply* reply);

private:
    // Private member functions for Microsoft-specific behavior
    void getAccessToken();
    void putRequest();

private:
    QOAuth2AuthorizationCodeFlow oauth2;

};

#endif // MICROSOFTOAUTH2_H
