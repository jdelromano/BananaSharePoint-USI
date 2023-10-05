#ifndef AUTHENTICATOR_H
#define AUTHENTICATOR_H

#include <QObject>
#include "qoauth2authorizationcodeflow.h"
#include "qoauthhttpserverreplyhandler.h"
#include "QComboBox"
#include "QVBoxLayout"

class Authenticator : public QObject
{
    Q_OBJECT

public:
    explicit Authenticator(QObject *parent = nullptr);
    void startLogin();
    void getTeamsList(QComboBox * selectTeam);
    void getChannelsList(QString id);
    void getFilesFolder(QString team_id, QString channel_id, QVBoxLayout* dockWidgetlayout);
    void getFilesFolderContent(QString drive_id, QString item_id, QVBoxLayout* dockWidgetlayout);
    void getFileContent(QString site_id, QString item_id);
    void updateFileContent(QString site_id, QString item_id, QByteArray new_text);
private:
    QOAuth2AuthorizationCodeFlow * microsoft;
    QOAuthOobReplyHandler * replyHandler;
signals:
    void teamsListReceived();
    void channelsListReceived(QMap<QString, QString> channels, QString team_id);
    void getChannels(QString id);
    void loggedIn();
    void fileContentReceived(QByteArray fileContent, QString site_id, QString item_id);
};

#endif // AUTHENTICATOR_H
