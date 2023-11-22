#pragma once

#include <QObject>
#include "abstractauthenticator.h"
#include "helperstructures.h"
#include "QComboBox"
#include "QVBoxLayout"

class AuthenticatorMicrosoft : public AbstractAuthenticator
{
    Q_OBJECT

public:
    explicit AuthenticatorMicrosoft(QObject *parent);
    void getList();
    void getChannelsList(QString id);
    void getFilesFolder(QString team_id, QString channel_id);
    void getFilesFolderContent(QString drive_id, QString item_id);
    void getFileContent(QString site_id, QString item_id, QString file_name, bool open);
    void updateFileContent(QByteArray new_text, struct openFile * current_open_file);
    void checkVersion(QString site_id, QString item_id, QString version);
};


