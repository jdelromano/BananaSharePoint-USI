#include "authenticator.h"
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
#include "QSaveFile"
#include "QDir"

Authenticator::Authenticator(QObject *parent) : QObject{parent}{

    this->microsoft = new QOAuth2AuthorizationCodeFlow(this);

    QFile file("../../../auth_params.json");
    //qDebug() << "exists? " << QFile::exists("../../../auth_params.json");
    QJsonDocument document;
    file.open(QIODeviceBase::ReadOnly);
    if(file.isOpen())
    {
        QByteArray json_bytes = file.readAll();
        document = QJsonDocument::fromJson(json_bytes);
        file.close();
    }

    //Parse json to get authorization parameters
    QJsonObject obj = document.object();
    QUrl authUri = obj["auth_uri"].toString();
    QString clientId = obj["client_id"].toString();
    QUrl tokenUri = obj["token_uri"].toString();
    QUrl redirectUri = obj["redirect_uri"].toString();
    quint16 port = static_cast<quint16>(redirectUri.port()); // Get the port

    //required values for authorization code request
    this->microsoft->setClientIdentifier(clientId);
    this->microsoft->setScope("User.Read Team.ReadBasic.All Channel.ReadBasic.All Files.ReadWrite.All");
    this->microsoft->setAuthorizationUrl(authUri);

    //additional required values for access token request
    this->microsoft->setAccessTokenUrl(tokenUri);

    /*
    Sets the parameter-modification function modifyParametersFunction.
    This function is used to customize the parameters sent to the server during a specified authorization stage.
    */
    this->microsoft->setModifyParametersFunction([](QAbstractOAuth::Stage stage, QMultiMap<QString, QVariant> * parameters) {
        // Percent-decode the "code" parameter
        if (stage == QAbstractOAuth::Stage::RequestingAccessToken) {
            //qDebug() << "requesting access token";
            QByteArray code = parameters->value("code").toByteArray();
            parameters->replace("code", QUrl::fromPercentEncoding(code));
        }
        if (stage == QAbstractOAuth::Stage::RequestingAuthorization){
            //qDebug() << "requesting autorization code";
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

    connect(this->microsoft, &QOAuth2AuthorizationCodeFlow::granted, [this](){
        qDebug() << "token received";
        emit loggedIn();
        //qDebug() << this->microsoft->token();
    });
}

void Authenticator::startLogin(){
    this->microsoft->grant();
}

void Authenticator::getTeamsList(){
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

void Authenticator::getChannelsList(QString team_id){
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

void Authenticator::getFilesFolder(QString team_id, QString channel_id){
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

void Authenticator::getFilesFolderContent(QString drive_id, QString item_id){
    connect(this, &Authenticator::fileContentReceived, this, &Authenticator::saveFileLocal);
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
            QJsonObject obj_file = (obj["file"]).toObject();
            if ((obj_file["mimeType"].toString()).compare("text/plain") == 0){
                QString item_id = obj["id"].toString();
                QJsonObject obj2 = (obj["parentReference"]).toObject();
                QString site_id = obj2["siteId"].toString();
                QString file_name = obj["name"].toString();
                list_file_infos.append({item_id, site_id, file_name});
                qDebug() << "File name: " << file_name;
                this->getFileContent(site_id, item_id, file_name, false);
            }
        }
        emit filesListReceived(list_file_infos);
    });
}

//UNUSED
//void Authenticator::getFileContent(QString site_id, QString item_id){
//    QUrl url("https://graph.microsoft.com/v1.0/sites/" + site_id
//             +"/drive/items/"+ item_id + "/content");
//    QNetworkReply * reply =  this->microsoft->get(url);
//    connect(reply, &QNetworkReply::finished, [this, reply, site_id, item_id](){
//        qDebug() << "File content:";
//        QByteArray fileContent = reply->readAll();
//        qDebug() << fileContent;
//        QUrl url2("https://graph.microsoft.com/v1.0/sites/" + site_id
//                 +"/drive/items/"+ item_id + "/versions/current");
//        QNetworkReply * reply2 =  this->microsoft->get(url2);
//        connect(reply2, &QNetworkReply::finished, [this, fileContent, reply2, site_id, item_id](){
//            QJsonObject reply_obj  = (QJsonDocument::fromJson(reply2->readAll())).object();
//            QString version = reply_obj["id"].toString();
//            qDebug() << "here 1 " << version;
//            emit fileContentReceived(fileContent, site_id, item_id, version);
//        });
//    });
//}

void Authenticator::getFileContent(QString site_id, QString item_id, QString file_name, bool open){
    QUrl url("https://graph.microsoft.com/v1.0/sites/" + site_id
             +"/drive/items/"+ item_id + "/content");
    QNetworkReply * reply =  this->microsoft->get(url);
    connect(reply, &QNetworkReply::finished, [this, reply, site_id, item_id, file_name, open](){
        qDebug() << "Getting file content";
        QByteArray fileContent = reply->readAll();
        //qDebug() << fileContent;
        QUrl url2("https://graph.microsoft.com/v1.0/sites/" + site_id
                  +"/drive/items/"+ item_id + "/versions/current");
        QNetworkReply * reply2 =  this->microsoft->get(url2);
        connect(reply2, &QNetworkReply::finished, [this, fileContent, reply2, site_id, item_id, file_name, open](){
            QJsonObject reply_obj  = (QJsonDocument::fromJson(reply2->readAll())).object();
            QString version = reply_obj["id"].toString();
            qDebug() << "file version: " << version;
            emit fileContentReceived(file_name, fileContent, site_id, item_id, version, open);
        });
    });
}


void Authenticator::updateFileContent(QByteArray new_text, struct openFile * current_open_file){
    QString site_id = current_open_file->site_id;
    QString item_id = current_open_file->item_id;
    QUrl url("https://graph.microsoft.com/v1.0/sites/" + site_id
             + "/drive/items/" + item_id + "/content");
    QNetworkReply * reply =  this->microsoft->put(url, new_text);
    connect(reply, &QNetworkReply::finished, [reply](){
        qDebug() << "updated text sent";
    });
}

void Authenticator::checkVersion(QString site_id, QString item_id, QString version){
    QUrl url("https://graph.microsoft.com/v1.0/sites/" + site_id
              +"/drive/items/"+ item_id + "/versions/current");
    QNetworkReply * reply =  this->microsoft->get(url);
    connect(reply, &QNetworkReply::finished, [this, reply, version](){
        QJsonObject reply_obj  = (QJsonDocument::fromJson(reply->readAll())).object();
        QString new_version = reply_obj["id"].toString();
        qDebug() << "my version:";
        qDebug() << version;
        qDebug() << "new version:";
        qDebug() << new_version;
        if (new_version.compare(version, Qt::CaseSensitive) == 0){
            emit versionChecked(true);
        }
        else {
            emit versionChecked(false);
        }
    });
}

void Authenticator::saveFileLocal(QString fileName, QString fileContent, QString site_id, QString item_id, QString version, bool open){
    QFile files_infos("../../../files/files_params.json");
    if( !files_infos.open(QIODevice::ReadOnly)){
        return;
    }
    QJsonDocument files_infos_json = QJsonDocument::fromJson(files_infos.readAll());
    files_infos.close();
    QJsonArray files_infos_array = files_infos_json.array();
    QString file_path = "../../../files/" + fileName;
    bool file_exists = QFile::exists(file_path);
    if (file_exists){ //update
        QFile file(file_path);
        QJsonDocument document;
        file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text);
        if(file.isOpen())
        {
            file.write(fileContent.toLocal8Bit().data());
            file.close();
        }
        bool json_updated = false;
        int index = 0;
        while(!json_updated && index < files_infos_array.size()){
            QJsonObject jobj = files_infos_array.at(index).toObject();
            if ((jobj["id"].toString()).compare(item_id) == 0){
                json_updated = true;
                jobj["version"] = version;
                files_infos_array.replace(index, jobj);
            }
            ++index;
        }
    }
    else{ //create new
        QJsonObject fileDetails = { {"id", item_id},
                                    {"name", fileName},
                                    {"version", version}};

        files_infos_array.push_back(fileDetails);
        QSaveFile file(file_path);
        if (file.open(QFile::WriteOnly | QFile::Text)) {
            QTextStream out(&file);
            out << fileContent;
        } else {
            qDebug() << "Cannot open file";
        }
    }

    if(!files_infos.open(QIODevice::WriteOnly)){
        return;
    }
    //qDebug() << files_infos_array;
    QJsonDocument doc(files_infos_array);
    files_infos.write(doc.toJson());
    files_infos.close();
    if (open){
        emit openFile(fileName, site_id, item_id, version);
    }
}















