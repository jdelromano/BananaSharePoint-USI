#include "authenticatorGoogle.h"
#include "qjsonarray.h"
#include "abstractauthenticator.h"
#include "QFile"
#include "QJsonDocument"
#include "QJsonObject"
#include "QJsonArray"
#include "QNetworkReply"
#include "QIterator"
#include "QList"
#include "QDir"

/*!
 * \brief AuthenticatorGoogle::AuthenticatorGoogle constructor for google authenticator
 * \param parent parent object
 * \param secret boolean value which tells if the authentication needs a secret value, in this case is true
 */
AuthenticatorGoogle::AuthenticatorGoogle(QObject *parent, bool secret) : AbstractAuthenticator(parent, secret){

    //client ID init with google...
}

/*!
 * \brief Authenticator::getGoogleList retrieves the list the user is a part of
 */
void AuthenticatorGoogle::getList(){
    connect(this, &AbstractAuthenticator::fileContentReceived, this, &AbstractAuthenticator::saveFileLocal);
    auto reply = this->m_oAuth->get(QUrl("https://www.googleapis.com/drive/v3/files"));
    connect(reply, &QNetworkReply::finished, [reply, this]() {
        QList<fileInfos> list_file_infos;
        QByteArray r = reply->readAll();
        QJsonDocument r_json = QJsonDocument::fromJson(r);
        QJsonObject r_jsonobj = r_json.object();
        QJsonArray files = r_jsonobj["files"].toArray();
        for (auto file = files.begin(); file != files.end(); file++){
            QJsonObject file_obj = (*file).toObject();
            QString id = file_obj["id"].toString();
            QString name = file_obj["name"].toString();
            list_file_infos.append({id, "unused", name});
        }
        emit googleFilesListReceived(list_file_infos);
    });
}

/*!
 * \brief Authenticator::getFileContent retrieves the content and the version of a specific file.
 * \param site_id the site id for the file
 * \param item_id the item id (file id)
 * \param file_name the name of the file
 * \param open boolean value which says if the file has to be opened in the text editor after it is retrieved or not
 */
void AuthenticatorGoogle::getFileContent(QString site_id, QString item_id, QString file_name, bool open){
    QString url = "https://www.googleapis.com/drive/v2/files/" + item_id + "/export";
    QVariantMap map;
    QVariant map_value("text/plain");
    map.insert("mimeType", map_value);
    QNetworkReply *reply = this->m_oAuth->get(QUrl(url), map);
    connect(reply, &QNetworkReply::finished, [reply, this, item_id, file_name, site_id, open]() {
        QByteArray fileContent = reply->readAll();
        emit fileContentReceived(file_name, fileContent, site_id, item_id, "version", open);
    });
}


/*!
 * \brief Authenticator::updateFileContent updates the content of a file online
 * \param new_text the new content of the file
 * \param current_open_file structure containing informations about the currently open file (the one to update)
 */
void AuthenticatorGoogle::updateFileContent(QByteArray new_text, struct openFile * current_open_file){
    QString url = "https://www.googleapis.com/upload/drive/v2/files/" + current_open_file->item_id;
    QNetworkReply *reply = this->m_oAuth->put(QUrl(url), new_text);
    connect(reply, &QNetworkReply::finished, [reply, this]() {
        qDebug() << "REQUEST FINISHED. Error? " << (reply->error() != QNetworkReply::NoError);
        //qDebug() << reply->readAll();
    });
}

/*!
 * \brief Authenticator::checkVersion performs a check on the version of a file. It checks wether the version of the
 * local file and the version of the online file match.
 * \param site_id the site id for the file
 * \param item_id the item id (file id)
 * \param version the version of the file stored locally
 */
void AuthenticatorGoogle::checkVersion(QString site_id, QString item_id, QString version){
    emit versionChecked(true);
}
