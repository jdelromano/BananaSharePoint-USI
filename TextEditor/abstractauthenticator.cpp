#include "abstractauthenticator.h"
#include "qapplication.h"
#include "qjsonarray.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "qoauthhttpserverreplyhandler.h"
#include "qdesktopservices.h"

#include <stdio.h>

/*!
 * \brief AbstractAuthenticator::AbstractAuthenticator constructor which initializes the authenticator
 * \param parent parent object
 * \param secret boolean value which says if a secret value is needed for the authentication process or not
 */
AbstractAuthenticator::AbstractAuthenticator(QObject *parent, int port) : QObject(parent) 
{
    this->replyHandler = new QOAuthHttpServerReplyHandler(port, this);
    this->m_oAuth = new QOAuth2AuthorizationCodeFlow(this);
    this->m_oAuth->setReplyHandler(replyHandler);

    // Connect signal for browser authorization
    connect(this->m_oAuth, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
            &QDesktopServices::openUrl);

    if (port = 8080) {
        this->m_oAuth->setModifyParametersFunction(
                [](QAbstractOAuth::Stage stage, QMultiMap<QString, QVariant> *parameters) {
                    // Percent-decode the "code" parameter
                    if (stage == QAbstractOAuth::Stage::RequestingAccessToken) {
                        QByteArray code = parameters->value("code").toByteArray();
                        parameters->replace("code", QUrl::fromPercentEncoding(code));
                    }
        });
    }
    
    connect(this->m_oAuth, &QOAuth2AuthorizationCodeFlow::error, this,
            &AbstractAuthenticator::onOauthError);

    // Connect signal for token granted
    /*connect(this->m_oAuth, &QOAuth2AuthorizationCodeFlow::granted, this,
            &AbstractAuthenticator::onGranted );*/

    connect(this->m_oAuth, &QOAuth2AuthorizationCodeFlow::granted, [this]() {
        qDebug() << "token received";
        emit loggedIn();
    });

}

void AbstractAuthenticator::onOauthError(const QString& error, const QString& errorDescription,
    const QUrl& uri)
{
    qDebug() << error << errorDescription << uri;
}

void AbstractAuthenticator::onGranted()
{
    qDebug() << "granted";
}

 /*!
 * \brief AbstractAuthenticator::startLogin starts the login process
 */
void AbstractAuthenticator::startLogin(){
    this->m_oAuth->grant();
}

/*!
 * \brief Authenticator::saveFileLocal saves a file locally. Updates the file content, if the file already exists, otherwise it
 * creates a new file. Also updates the file informations: id, name and version (or adds a new object with the file informations
 * in case it is a newly added file)
 * \param fileName the name of the file
 * \param fileContent the content of the file version found online
 * \param site_id the site id for the file
 * \param item_id the item id (file id)
 * \param version the version of the file currently online
 * \param open boolean value which says if the file has to be opened in the text editor or not
 */
void AbstractAuthenticator::saveFileLocal(QString fileName, QString fileContent, QString site_id, QString item_id, QString version, bool open){

    QString filesJsonPath = this->m_filesPath + "/files_params.json";
    //QString filesJsonPath = this->fileDataDir + "/files_params.json";
    QFile files_infos(filesJsonPath);
    if( !files_infos.open(QIODevice::ReadOnly)){
        return;
    }
    QJsonDocument files_infos_json = QJsonDocument::fromJson(files_infos.readAll());
    files_infos.close();
    QJsonArray files_infos_array = files_infos_json.array();
    QString file_path = this->m_filesPath + "/" + fileName;
    //QString file_path = this->fileDataDir + "/" + fileName;
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
        QFile file(file_path);
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
    QJsonDocument doc(files_infos_array);
    files_infos.write(doc.toJson());
    files_infos.close();
    if (open){
        emit openFile(fileName, site_id, item_id, version);
    }
}
