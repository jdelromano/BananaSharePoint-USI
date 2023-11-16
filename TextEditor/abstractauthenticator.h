#ifndef ABSTRACTAUTHENTICATOR_H
#define ABSTRACTAUTHENTICATOR_H

#include "helperstructures.h"
#include "qoauth2authorizationcodeflow.h"
#include "qoauthoobreplyhandler.h"
#include "QFile"
#include <QObject>

class AbstractAuthenticator : public QObject
{
    Q_OBJECT

public:
    explicit AbstractAuthenticator(QObject *parent = nullptr, bool secret = false);
    QString files_path;
    QOAuth2AuthorizationCodeFlow * microsoft;
    QOAuthOobReplyHandler * replyHandler;
    void startLogin();
    virtual void getTeamsList() = 0;
    virtual void getFileContent(QString site_id, QString item_id, QString file_name, bool open) = 0;
    virtual void updateFileContent(QByteArray new_text, struct openFile * current_open_file) = 0;
    virtual void checkVersion(QString site_id, QString item_id, QString version) = 0;

signals:
    void teamsListReceived(QList<QPair<QString, QString> > list_id_name);
    void channelsListReceived(QMap<QString, QString> channels, QString team_id);
    void getChannels(QString id);
    void loggedIn();
    void fileContentReceived(QString file_name, QByteArray fileContent, QString site_id, QString item_id, QString version, bool open);
    void filesListReceived(QList<fileInfos> list_file_infos);
    void googleFilesListReceived(QList<fileInfos> list_file_infos);
    void versionChecked(bool res);
    void openFile(QString fileName, QString site_id, QString item_id, QString version);

public slots:
    void saveFileLocal(QString fileName, QString fileContent, QString site_id, QString item_id, QString version, bool open);

};

#endif // ABSTRACTAUTHENTICATOR_H
