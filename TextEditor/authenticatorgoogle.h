#ifndef AUTHENTICATORGOOGLE_H
#define AUTHENTICATORGOOGLE_H

#include "abstractauthenticator.h"
#include <QObject>

class AuthenticatorGoogle : public AbstractAuthenticator
{
    Q_OBJECT
public:
    explicit AuthenticatorGoogle(QObject *parent = nullptr, bool secret = true);
    void getTeamsList();
    void getChannelsList(QString id);
    void getFilesFolder(QString team_id, QString channel_id);
    void getFilesFolderContent(QString drive_id, QString item_id);
    void getFileContent(QString site_id, QString item_id, QString file_name, bool open);
    void updateFileContent(QByteArray new_text, struct openFile * current_open_file);
    void checkVersion(QString site_id, QString item_id, QString version);
};

#endif // AUTHENTICATORGOOGLE_H
