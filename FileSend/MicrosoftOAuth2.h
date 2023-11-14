// MicrosoftOAuth2.h
#ifndef MICROSOFTOAUTH2_H
#define MICROSOFTOAUTH2_H

#include "OAuth2Base.h"
#include <QObject>
#include "qoauth2authorizationcodeflow.h"
#include "qoauthhttpserverreplyhandler.h"

class MicrosoftOAuth2 : public OAuth2Base {
    Q_OBJECT

public:
    MicrosoftOAuth2(QObject* parent = nullptr);
    virtual ~MicrosoftOAuth2();
    void authenticate() override;
    void handleCallback(const QUrl& callbackUrl) override;
    void startLogin();
    void getTeamsList();
    void getChannelsList(QString id);
    void getFilesFolder(QString team_id, QString channel_id);
    void getFilesFolderContent(QString drive_id, QString item_id);
    void getFileContent(QString site_id, QString item_id, QString file_name, bool open);
    void updateFileContent(QByteArray new_text, struct openFile * current_open_file);
    void checkVersion(QString site_id, QString item_id, QString version);

private:
    QOAuth2AuthorizationCodeFlow * microsoft;
    QOAuthOobReplyHandler * replyHandler;

signals:
    void teamsListReceived(QList<QPair<QString, QString> > list_id_name);
    void channelsListReceived(QMap<QString, QString> channels, QString team_id);
    void getChannels(QString id);
    void loggedIn();
    void authorizeWithBrowser(const QUrl& url);
//    void fileContentReceived(QString file_name, QByteArray fileContent, QString site_id, QString item_id, QString version, bool open);
//    void filesListReceived(QList<fileInfos> list_file_infos);
//    void versionChecked(bool res);
//    void openFile(QString fileName, QString site_id, QString item_id, QString version);

private slots:
    void saveFileLocal(QString fileName, QString fileContent, QString site_id, QString item_id, QString version, bool open);
};

#endif // MICROSOFTOAUTH2_H
