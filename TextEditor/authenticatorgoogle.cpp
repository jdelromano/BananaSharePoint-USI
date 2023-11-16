#include "authenticatorgoogle.h"
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

AuthenticatorGoogle::AuthenticatorGoogle(QObject *parent, bool secret) : AbstractAuthenticator(parent, secret){}

/*!
 * \brief Authenticator::getTeamsList retrieves the list of teams the user is a part of
 */
void AuthenticatorGoogle::getTeamsList(){
    connect(this, &AbstractAuthenticator::fileContentReceived, this, &AuthenticatorGoogle::saveFileLocal);
    auto reply = this->microsoft->get(QUrl("https://www.googleapis.com/drive/v3/files"));
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
    QNetworkReply * reply = this->microsoft->get(QUrl(url), map);
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
    QNetworkReply * reply = this->microsoft->put(QUrl(url), new_text);
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
void AuthenticatorGoogle::saveFileLocal(QString fileName, QString fileContent, QString site_id, QString item_id, QString version, bool open){
    QString filesJsonPath = this->files_path + "/files_params.json";
    QFile files_infos(filesJsonPath);
    if( !files_infos.open(QIODevice::ReadOnly)){
        return;
    }
    QJsonDocument files_infos_json = QJsonDocument::fromJson(files_infos.readAll());
    files_infos.close();
    QJsonArray files_infos_array = files_infos_json.array();
    QString file_path = this->files_path + "/" + fileName;
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
    //qDebug() << files_infos_array;
    QJsonDocument doc(files_infos_array);
    files_infos.write(doc.toJson());
    files_infos.close();
    if (open){
        emit openFile(fileName, site_id, item_id, version);
    }
}
