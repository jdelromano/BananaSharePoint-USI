#include "main.h"
#include "OAuth2Base.h"

MainApplication::MainApplication(int &argc, char **argv) : QApplication(argc, argv) {
    // Create instances of authenticators
    MicrosoftOAuth2 microsoftAuthenticator;
//    GoogleOAuth2 googleAuthenticator;

    // Connect signals to slots for authentication completion
    connect(&microsoftAuthenticator, &MicrosoftOAuth2::loggedIn, this, &MainApplication::microsoftAuthenticationComplete);
//    connect(&googleAuthenticator, &GoogleOAuth2::gotToken, this, &MainApplication::googleAuthenticationComplete);

    // Initiate authentication for both Microsoft and Google
    microsoftAuthenticator.authenticate();
//    googleAuthenticator.authenticate();
}

void MainApplication::microsoftAuthenticationComplete() {
    qDebug() << "Microsoft authentication complete!";
}

void MainApplication::googleAuthenticationComplete() {
    qDebug() << "Google authentication complete!";
}

int main(int argc, char **argv) {
    MainApplication app(argc, argv);
    return app.exec();
}
