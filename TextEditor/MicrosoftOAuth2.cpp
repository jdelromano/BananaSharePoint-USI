#include "MicrosoftOAuth2.h"
#include "Microsoft_secrets.h"
#include "mainwindow.h"
#include "qdesktopservices.h"
#include "qoauthhttpserverreplyhandler.h"
#include "QFile"
#include "QJsonDocument"
#include "QJsonObject"
#include "QJsonArray"
#include "QNetworkReply"
#include "QIterator"
#include "QList"

//Changing Authenticator to MicrosoftOAuth2.cpp
MicrosoftOAuth2::MicrosoftOAuth2(QObject* parent) : OAuth2Base(parent)
{

    this->microsoft = new QOAuth2AuthorizationCodeFlow(this);

    this->microsoft = new QOAuth2AuthorizationCodeFlow(this);

    // Parse json to get authorization parameters
    QUrl authUri = Microsoft_secrets::auth_uri;
    QString clientId = Microsoft_secrets::client_id;
    QUrl tokenUri = Microsoft_secrets::token_uri;
    QUrl redirectUri = Microsoft_secrets::redirect_uris;
    quint16 port = Microsoft_secrets::port; // Get the port

    // required values for authorization code request
    this->microsoft->setClientIdentifier(clientId);
    this->microsoft->setScope(
            "User.Read Team.ReadBasic.All Channel.ReadBasic.All Files.ReadWrite.All");
    this->microsoft->setAuthorizationUrl(authUri);

    // additional required values for access token request
    this->microsoft->setAccessTokenUrl(tokenUri);

    /*
    Sets the parameter-modification function modifyParametersFunction.
    This function is used to customize the parameters sent to the server during a specified authorization stage.
    */
    this->microsoft->setModifyParametersFunction([](QAbstractOAuth::Stage stage, QMultiMap<QString, QVariant> * parameters) {
        // Percent-decode the "code" parameter
        if (stage == QAbstractOAuth::Stage::RequestingAccessToken) {
            qDebug() << "requesting access token";
            QByteArray code = parameters->value("code").toByteArray();
            parameters->replace("code", QUrl::fromPercentEncoding(code));
        }
        if (stage == QAbstractOAuth::Stage::RequestingAuthorization){
            qDebug() << "requesting autorization code";
        }
    });

    /*
    Setting reply handler
    */
    this->replyHandler = new QOAuthHttpServerReplyHandler(port, this);
    this->microsoft->setReplyHandler(replyHandler);

    /*
    authorizeWithBrowser: signal emitted when the url generated by resourceOwnerAuthorization() is ready to be used in the web browser
    resourceOwnerAuthorization(): builds the resource owner authorization URL to be used in the browser, from the values
    set above (client id, scope, authorization url).
    */
    connect(this->microsoft, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, &QDesktopServices::openUrl);

    connect(this->microsoft, &QOAuth2AuthorizationCodeFlow::granted, [this]() {
        qDebug() << "token received";
        emit loggedIn();
        // qDebug() << this->microsoft->token();
        // this->getTeamsList();
    });
}

void MicrosoftOAuth2::authenticate()
{
    this->microsoft->grant();
}



void MicrosoftOAuth2::getTeamsList()
    {
    QUrl url("https://graph.microsoft.com/v1.0/me/joinedTeams");
    QNetworkReply * reply =  this->microsoft->get(url);
    connect(reply, &QNetworkReply::finished, [this, reply](){
        QJsonObject reply_obj  = (QJsonDocument::fromJson(reply->readAll())).object();
        QJsonArray values = reply_obj["value"].toArray();
        qDebug() << "MY teams:";
        QList<QPair<QString, QString>> list_id_name;
        for(int p=0; p < values.size(); ++p){
            QJsonObject obj = (values.at(p)).toObject();
            QString team_name = obj["displayName"].toString();
            QString team_id = obj["id"].toString();
            list_id_name.append(qMakePair(team_id, team_name));
            qDebug() << "Team name: " << team_name;
            qDebug() << "Team id: " << team_id;
        }
        emit teamsListReceived(list_id_name);
    });
}

void MicrosoftOAuth2::getChannelsList(QString team_id)
{
    QUrl url("https://graph.microsoft.com/v1.0/teams/" + team_id + "/channels");
    QNetworkReply * reply =  this->microsoft->get(url);
    connect(reply, &QNetworkReply::finished, [this, reply, team_id](){
        QJsonObject reply_obj  = (QJsonDocument::fromJson(reply->readAll())).object();
        QJsonArray values = reply_obj["value"].toArray();
        qDebug() << "Team channels:";
        QMap<QString, QString> channels;
        for(int p=0; p < values.size(); ++p){
            QJsonObject obj = (values.at(p)).toObject();
            QString channel_name = obj["displayName"].toString();
            QString channel_id = obj["id"].toString();
            channels.insert(channel_id, channel_name);
            qDebug() << "Channel name: " << obj["displayName"].toString();
        }
        emit channelsListReceived(channels, team_id);
    });
}

void MicrosoftOAuth2::getFilesFolder(QString team_id, QString channel_id)
{
    QUrl url("https://graph.microsoft.com/v1.0/teams/" + team_id
             + "/channels/" + channel_id + "/filesFolder?");
    QNetworkReply * reply =  this->microsoft->get(url);
    connect(reply, &QNetworkReply::finished, [this, reply](){
        QJsonObject reply_obj  = (QJsonDocument::fromJson(reply->readAll())).object();
        QString item_id = reply_obj["id"].toString();
        QJsonObject obj = (reply_obj["parentReference"]).toObject();
        QString drive_id = obj["driveId"].toString();
        this->getFilesFolderContent(drive_id, item_id);
    });
}

void MicrosoftOAuth2::getFilesFolderContent(QString drive_id, QString item_id)
{
    QUrl url("https://graph.microsoft.com/v1.0/drives/" + drive_id
             + "/items/" + item_id + "/children");
    QNetworkReply * reply =  this->microsoft->get(url);
    connect(reply, &QNetworkReply::finished, [this, reply](){
        QJsonObject reply_obj  = (QJsonDocument::fromJson(reply->readAll())).object();
        QJsonArray values = reply_obj["value"].toArray();
        qDebug() << "My files:";
        QList<fileInfos> list_file_infos;
        for(int p=0; p < values.size(); ++p){
            QJsonObject obj = (values.at(p)).toObject();
            QString item_id = obj["id"].toString();
            QJsonObject obj2 = (obj["parentReference"]).toObject();
            QString site_id = obj2["siteId"].toString();
            QString file_name = obj["name"].toString();
            list_file_infos.append({item_id, site_id, file_name});
            qDebug() << "File name: " << file_name;
        }
        emit filesListReceived(list_file_infos);
    });
}

void MicrosoftOAuth2::getFileContent(QString site_id, QString item_id)
{
    qDebug() << "here";
    QUrl url("https://graph.microsoft.com/v1.0/sites/" + site_id
             +"/drive/items/"+ item_id + "/content");
    QNetworkReply * reply =  this->microsoft->get(url);
    connect(reply, &QNetworkReply::finished, [this, reply, site_id, item_id](){
        qDebug() << "File content:";
        QByteArray fileContent = reply->readAll();
        qDebug() << fileContent;
        emit fileContentReceived(fileContent, site_id, item_id);
        //this->updateFileContent(site_id, item_id);
    });
}

void MicrosoftOAuth2::updateFileContent(QString site_id, QString item_id, QByteArray new_text)
{
    QUrl url("https://graph.microsoft.com/v1.0/sites/" + site_id
             + "/drive/items/" + item_id + "/content");
    QNetworkReply * reply =  this->microsoft->put(url, new_text);
    connect(reply, &QNetworkReply::finished, [reply](){
        qDebug() << "updated text sent";
    });
}














