// MicrosoftOAuth2.h
#ifndef MICROSOFTOAUTH2_H
#define MICROSOFTOAUTH2_H

#include <QObject>
#include "OAuth2Base.h"
#include "qoauth2authorizationcodeflow.h"
#include "qoauthhttpserverreplyhandler.h"
#include "helperstructures.h"
#include "QComboBox"
#include "QVBoxLayout"

class MicrosoftOAuth2 : public OAuth2Base
{
    Q_OBJECT

public:
    explicit MicrosoftOAuth2(QObject *parent = nullptr);
    ~MicrosoftOAuth2();
    void authenticate() override;
    void handleCallback() override;
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

#endif // MICROSOFTOAUTH2_H
