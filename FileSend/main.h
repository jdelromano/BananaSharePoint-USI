#ifndef MAIN_H
#define MAIN_H

#include <QApplication>
#include <QDebug>
#include "MicrosoftOAuth2.h"
#include "GoogleOAuth2.h"

class MainApplication : public QApplication {
    Q_OBJECT

public:
    MainApplication(int &argc, char **argv);

private slots:
    void microsoftAuthenticationComplete();
    void googleAuthenticationComplete();
};

#endif // MAIN_H
