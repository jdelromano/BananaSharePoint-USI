#pragma once

#include "helperstructures.h"
#include "qoauth2authorizationcodeflow.h"
#include "qoauthoobreplyhandler.h"
#include "QFile"
#include <QObject>
#include <QOAuth2AuthorizationCodeFlow>
#include <QOAuthHttpServerReplyHandler>
#include <QDesktopServices>
#include <QDebug>

class AbstractAuthenticator : public QObject
{
    Q_OBJECT

protected:
    QOAuth2AuthorizationCodeFlow *m_oAuth;
    QOAuthOobReplyHandler *replyHandler;
    //QString *m_filesPath; 

    //QOAuthOobReplyHandler *replyHandler;

public:
    explicit AbstractAuthenticator(QObject *parent, int port);
    QString m_filesPath;
    void setAuthParameters(const QUrl &authUri, const QString &clientId, const QUrl &tokenUri, const QUrl &redirectUri, int port, const QString &clientSecret);
    void startLogin();
    virtual void getList() = 0;
    virtual void getFileContent(QString site_id, QString item_id, QString file_name, bool open) = 0;
    virtual void updateFileContent(QByteArray new_text, struct openFile * current_open_file) = 0;
    virtual void checkVersion(QString site_id, QString item_id, QString version) = 0;

signals:
    void loggedIn();
    void fileContentReceived(QString file_name, QByteArray fileContent, QString site_id, QString item_id, QString version, bool open);
    void filesListReceived(QList<fileInfos> list_file_infos);
    void versionChecked(bool res);
    void openFile(QString fileName, QString site_id, QString item_id, QString version);
    //microsoft signals remove these out of here
    void teamsListReceived(QList<QPair<QString, QString>> list_id_name);
    void channelsListReceived(QMap<QString, QString> channels, QString team_id);
    //void getChannels(QString id);
    //google signals remove these out of here
    void googleFilesListReceived(QList<fileInfos> list_file_infos);

public slots:
    void saveFileLocal(QString fileName, QString fileContent, QString site_id, QString item_id, QString version, bool open);

protected slots:
    virtual void onOauthError(const QString &error, const QString &errorDescription, const QUrl &uri);
    virtual void onGranted();

};

