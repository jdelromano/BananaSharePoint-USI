#ifndef AUTHENTICATOR_H
#define AUTHENTICATOR_H

#include <QObject>
#include "abstractauthenticator.h"
#include "qoauth2authorizationcodeflow.h"
#include "qoauthhttpserverreplyhandler.h"
#include "helperstructures.h"
#include "QComboBox"
#include "QVBoxLayout"

class Authenticator : public AbstractAuthenticator
{
    Q_OBJECT

public:
    explicit Authenticator(QObject *parent = nullptr, bool secret = false);
    void getTeamsList();
    void getChannelsList(QString id);
    void getFilesFolder(QString team_id, QString channel_id);
    void getFilesFolderContent(QString drive_id, QString item_id);
    void getFileContent(QString site_id, QString item_id, QString file_name, bool open);
    void updateFileContent(QByteArray new_text, struct openFile * current_open_file);
    void checkVersion(QString site_id, QString item_id, QString version);
private slots:
    void saveFileLocal(QString fileName, QString fileContent, QString site_id, QString item_id, QString version, bool open);
};

#endif // AUTHENTICATOR_H
