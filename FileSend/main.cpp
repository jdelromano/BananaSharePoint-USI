#include <QApplication>
#include "googleoauth2.h"
#include "microsoftoauth2.h"
#include "GoogleAPIIntegration.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    GoogleOAuth2 googleAuth;
    MicrosoftOAuth2 microsoftAuth;
    GoogleAPIIntegration googleIntegration; // Create an instance of GoogleAPIIntegration

    // Set OAuth2 configuration parameters for Google and Microsoft.
    googleAuth.setClientId("your_google_client_id");
    googleAuth.setClientSecret("your_google_client_secret");
    googleAuth.setRedirectUri("your_google_redirect_uri");

    microsoftAuth.setClientId("your_microsoft_client_id");
    microsoftAuth.setClientSecret("your_microsoft_client_secret");
    microsoftAuth.setRedirectUri("your_microsoft_redirect_uri");

    // Connect signals for authentication success and error.
    QObject::connect(&googleAuth, &OAuth2Base::authenticationSuccess, [&googleIntegration]() {
        // Handle Google authentication success using GoogleAPIIntegration.
        googleIntegration.getUserEmail(); // For example, call a method in GoogleAPIIntegration.
    });
    QObject::connect(&googleAuth, &OAuth2Base::authenticationError, [](const QString& error) {
        // Handle Google authentication error.
    });

    QObject::connect(&microsoftAuth, &OAuth2Base::authenticationSuccess, []() {
        // Handle Microsoft authentication success.
    });
    QObject::connect(&microsoftAuth, &OAuth2Base::authenticationError, [](const QString& error) {
        // Handle Microsoft authentication error.
    });

    // Start the authentication process.
    googleAuth.authenticate();
    microsoftAuth.authenticate();

    return a.exec();
}
