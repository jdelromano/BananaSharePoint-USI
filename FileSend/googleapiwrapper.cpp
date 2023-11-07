#include "GoogleAPIIntegration.h"
#include "googleapiwrapper.h"
#include "QJsonDocument"
#include "QJsonObject"
#include "Google_secrets.h"

GoogleAPIWrapper::GoogleAPIWrapper(QObject *parent) : QObject(parent)
{
    this->google = new QOAuth2AuthorizationCodeFlow;
    this->google->setScope("https://mail.google.com/ https://www.googleapis.com/auth/drive");

    connect(this->google, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, &QDesktopServices::openUrl);



    const QUrl authUri=Secrets::auth_uri;
    const QString clientId =Secrets::client_id;
    const QUrl tokenUri= Secrets::token_uri;
    const QString clientSecret = Secrets::client_secret;
    const QUrl redirectUri= Secrets::redirect_uris;
    const auto port = Secrets::port; // Get the port

    this->google->setAuthorizationUrl(authUri);
    this->google->setClientIdentifier(clientId);
    this->google->setAccessTokenUrl(tokenUri);
    this->google->setClientIdentifierSharedKey(clientSecret);

    //qDebug() << "fields: \n" << authUri << "\n" << clientId << "\n" << tokenUri << "\n" << clientSecret << "\n port: " << port;

    this->google->setModifyParametersFunction([](QAbstractOAuth::Stage stage, QMultiMap<QString, QVariant> * parameters) {
        // Percent-decode the "code" parameter so Google can match it
        if (stage == QAbstractOAuth::Stage::RequestingAccessToken) {
            QByteArray code = parameters->value("code").toByteArray();
            //qDebug() << "code after decode: " << QUrl::fromPercentEncoding(code);
            parameters->replace("code", QUrl::fromPercentEncoding(code));
        }
    });

    this->replyHandler = new QOAuthHttpServerReplyHandler(port, this);
    this->google->setReplyHandler(replyHandler);

    connect(this, &GoogleAPIWrapper::gotToken, this, &GoogleAPIWrapper::getFilesList);


    connect(this->google, &QOAuth2AuthorizationCodeFlow::granted, [=](){
        //const QString token = this->google->token();
        //qDebug() << "got token";
        QDateTime date = this->google->expirationAt();
        qDebug() << "expire date: " << date;
        emit gotToken();
    });
}

GoogleAPIWrapper:: ~GoogleAPIWrapper(){
    qDebug() << "google wrapper destructor";
    delete this->google;
    delete this->replyHandler;
}

void GoogleAPIWrapper::go(){
    this->google->grant();
}

void GoogleAPIWrapper::getUserEmail(){
    qDebug() << "getting user email";
    connect(this->google, &QOAuth2AuthorizationCodeFlow::granted, [=]() {
        auto reply = this->google->get(QUrl("https://gmail.googleapis.com/gmail/v1/users/me/profile"));
        connect(reply, &QNetworkReply::finished, [reply]() {
            qDebug() << "REQUEST FINISHED. Error? " << (reply->error() != QNetworkReply::NoError);
            qDebug() << reply->readAll();
        });
    });
}

void GoogleAPIWrapper::getFilesList(){
    this->files.clear();
    auto reply = this->google->get(QUrl("https://www.googleapis.com/drive/v3/files"));
    connect(reply, &QNetworkReply::finished, [reply, this]() {
        //qDebug() << "REQUEST FINISHED. Error? " << (reply->error() != QNetworkReply::NoError);
        QByteArray r = reply->readAll();
        QJsonDocument r_json = QJsonDocument::fromJson(r);
        QJsonObject r_jsonobj = r_json.object();
        QJsonArray files = r_jsonobj["files"].toArray();
        for (auto file = files.begin(); file != files.end(); file++){
            QJsonObject file_obj = (*file).toObject();
            QString id = file_obj["id"].toString();
            QString name = file_obj["name"].toString();
            this->files.insert(id, name);
        }
        emit getFilesDone();
    });
}


void GoogleAPIWrapper::readFiles(){
    qDebug() << "reading files";
    QMap<QString, QString> files_map = this->files;
    for (auto file = files_map.begin(); file != files_map.end(); file++){
        QString id = file.key();
        QString name = file.value();
        qDebug() << id << " " << name;
    }
}

void GoogleAPIWrapper::getFile(QString id){
    //QString url = "https://docs.googleapis.com/v1/documents/" + id;
    QString url = "https://www.googleapis.com/drive/v2/files/" + id + "/export";
    QVariantMap map;
    QVariant map_value("text/plain");
    map.insert("mimeType", map_value);
    QNetworkReply * reply = this->google->get(QUrl(url), map);
    connect(reply, &QNetworkReply::finished, [reply, this, id]() {
        emit createFileChannel(id);
        emit getFileContentDone(reply, id);
    });
}


void GoogleAPIWrapper::saveFile(QString id, QByteArray new_text){
    QString url = "https://www.googleapis.com/upload/drive/v2/files/" + id;
    QNetworkReply * reply = this->google->put(QUrl(url), new_text);
    connect(reply, &QNetworkReply::finished, [reply, this]() {
        qDebug() << "REQUEST FINISHED. Error? " << (reply->error() != QNetworkReply::NoError);
        //qDebug() << reply->readAll();
    });
}




