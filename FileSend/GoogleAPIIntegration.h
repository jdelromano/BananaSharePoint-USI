#ifndef GOOGLEAPIINTEGRATION_H
#define GOOGLEAPIINTEGRATION_H

#include <QObject>

// Include necessary headers for your project
class GoogleAPIIntegration : public OAuth2Base {
    Q_OBJECT

public:
    GoogleAPIIntegration(QObject* parent = nullptr);
    ~GoogleAPIIntegration();

    void authenticateWithGoogle();
    void getUserEmail();
    void getFilesList();
    void readFiles();
    void getFile(QString id);
    void saveFile(QString id, QByteArray new_text);

private:
    // Include instances of GoogleAPIWrapper and GoogleOAuth2
};

#endif // GOOGLEAPIINTEGRATION_H
