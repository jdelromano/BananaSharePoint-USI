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
    void getFileContent(QString site_id, QString item_id, QString file_name, bool open);
    void updateFileContent(QByteArray new_text, struct openFile * current_open_file);
    void checkVersion(QString site_id, QString item_id, QString version);
    QString files_path;
private:
    QOAuth2AuthorizationCodeFlow * microsoft;
    QOAuthOobReplyHandler * replyHandler;
signals:
    void teamsListReceived(QList<QPair<QString, QString> > list_id_name);
    void channelsListReceived(QMap<QString, QString> channels, QString team_id);
    void getChannels(QString id);
    void loggedIn();
    void fileContentReceived(QString file_name, QByteArray fileContent, QString site_id, QString item_id, QString version, bool open);
    void filesListReceived(QList<fileInfos> list_file_infos);
    void versionChecked(bool res);
    void openFile(QString fileName, QString site_id, QString item_id, QString version);
private slots:
    void saveFileLocal(QString fileName, QString fileContent, QString site_id, QString item_id, QString version, bool open);
};

#endif // AUTHENTICATOR_H
