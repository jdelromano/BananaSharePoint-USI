#ifndef AUTHENTICATOR_H
#define AUTHENTICATOR_H

#include <QObject>
#include "qoauth2authorizationcodeflow.h"
#include "qoauthhttpserverreplyhandler.h"
#include "helperstructures.h"
#include "QComboBox"
#include "QVBoxLayout"

class Authenticator : public QObject
{
    Q_OBJECT

public:
    explicit Authenticator(QObject *parent = nullptr);
    void startLogin();
    void getTeamsList();
    void getChannelsList(QString id);
    void getFilesFolder(QString team_id, QString channel_id);
    void getFilesFolderContent(QString drive_id, QString item_id);
    void getFileContent(QString site_id, QString item_id);
    void updateFileContent(QString site_id, QString item_id, QByteArray new_text);
private:
    QOAuth2AuthorizationCodeFlow * microsoft;
    QOAuthOobReplyHandler * replyHandler;
signals:
    void teamsListReceived(QList<QPair<QString, QString> > list_id_name);
    void channelsListReceived(QMap<QString, QString> channels, QString team_id);
    void getChannels(QString id);
    void loggedIn();
    void fileContentReceived(QByteArray fileContent, QString site_id, QString item_id);
    void filesListReceived(QList<fileInfos> list_file_infos);
};

#endif // AUTHENTICATOR_H
