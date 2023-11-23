#include "authenticatorMicrosoft.h"
#include "abstractauthenticator.h"
#include "QFile"
#include "QJsonDocument"
#include "QJsonObject"
#include "QJsonArray"
#include "QNetworkReply"
#include "QIterator"
#include "QList"
#include "QDir"
#include "secrets.h"

/*!
 * \brief Authenticator::Authenticator sets up the authenticator
 * \param parent object
 */
AuthenticatorMicrosoft::AuthenticatorMicrosoft(QObject *parent)
    : AbstractAuthenticator(parent, 3000)
{
    // MicrosoftSecrets::REDIRECT_URI;
    this->m_oAuth->setClientIdentifier(MicrosoftSecrets::CLIENT_ID);
    this->m_oAuth->setAuthorizationUrl(QUrl(MicrosoftSecrets::AUTH_URI));
    this->m_oAuth->setAccessTokenUrl(QUrl(MicrosoftSecrets::TOKEN_URI));

    this->m_oAuth->setScope(
            "User.Read Team.ReadBasic.All Channel.ReadBasic.All Files.ReadWrite.All");
}

/*!
 * \brief Authenticator::getList retrieves the list of teams the user is a part of
 */
void AuthenticatorMicrosoft::getList()
{
    QUrl url("https://graph.microsoft.com/v1.0/me/joinedTeams");
    QNetworkReply *reply = this->m_oAuth->get(url);
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error()) {
            qDebug() << reply->errorString();
            return;
        }
        QJsonObject reply_obj = (QJsonDocument::fromJson(reply->readAll())).object();
        QJsonArray values = reply_obj["value"].toArray();
        qDebug() << "MY teams:";
        QList<QPair<QString, QString>> list_id_name;
        for (int p = 0; p < values.size(); ++p) {
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

/*!
 * \brief Authenticator::getChannelsList retrieves the list of channels for a certain team
 * \param team_id the id of the team
 */
void AuthenticatorMicrosoft::getChannelsList(QString team_id)
{
    QUrl url("https://graph.microsoft.com/v1.0/teams/" + team_id + "/channels");
    QNetworkReply *reply = this->m_oAuth->get(url);
    connect(reply, &QNetworkReply::finished, [this, reply, team_id]() {
        if (reply->error()) {
            qDebug() << reply->errorString();
            return;
        }
        QJsonObject reply_obj = (QJsonDocument::fromJson(reply->readAll())).object();
        QJsonArray values = reply_obj["value"].toArray();
        qDebug() << "Team channels:";
        QMap<QString, QString> channels;
        for (int p = 0; p < values.size(); ++p) {
            QJsonObject obj = (values.at(p)).toObject();
            QString channel_name = obj["displayName"].toString();
            QString channel_id = obj["id"].toString();
            channels.insert(channel_id, channel_name);
            qDebug() << "Channel name: " << obj["displayName"].toString();
        }
        emit channelsListReceived(channels, team_id);
    });
}

/*!
 * \brief Authenticator::getFilesFolder retrieves the folder of files for a certain channel, in a
 * certain team. Then calls Authenticator::getFilesFolderContent on the received folder. \param
 * team_id the id of the team \param channel_id the id of the channel
 */
void AuthenticatorMicrosoft::getFilesFolder(QString team_id, QString channel_id)
{
    QUrl url("https://graph.microsoft.com/v1.0/teams/" + team_id + "/channels/" + channel_id
             + "/filesFolder?");
    QNetworkReply *reply = this->m_oAuth->get(url);
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error()) {
            qDebug() << reply->errorString();
            return;
        }
        QJsonObject reply_obj = (QJsonDocument::fromJson(reply->readAll())).object();
        QString item_id = reply_obj["id"].toString();
        QJsonObject obj = (reply_obj["parentReference"]).toObject();
        QString drive_id = obj["driveId"].toString();
        this->getFilesFolderContent(drive_id, item_id);
    });
}

/*!
 * \brief Authenticator::getFilesFolderContent retrieves the content of the folder, which contains informations
 * about every file in the folder. Then calls Authenticator::getFileContent for each of these files.
 * \param drive_id the id of the drive containing the folder
 * \param item_id the item id (folder id)
 */
void AuthenticatorMicrosoft::getFilesFolderContent(QString drive_id, QString item_id)
{
    connect(this, &AbstractAuthenticator::fileContentReceived, this,
            &AuthenticatorMicrosoft::saveFileLocal);
    QUrl url("https://graph.microsoft.com/v1.0/drives/" + drive_id + "/items/" + item_id + "/children");
    QNetworkReply *reply = this->m_oAuth->get(url);
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error()) {
            qDebug() << reply->errorString();
            return;
        }
        QJsonObject reply_obj = (QJsonDocument::fromJson(reply->readAll())).object();
        QJsonArray values = reply_obj["value"].toArray();
        qDebug() << "My files:";
        QList<fileInfos> list_file_infos;
        for (int p = 0; p < values.size(); ++p) {
            QJsonObject obj = (values.at(p)).toObject();
            QJsonObject obj_file = (obj["file"]).toObject();
            if ((obj_file["mimeType"].toString()).compare("text/plain") == 0) {
                QString item_id = obj["id"].toString();
                QJsonObject obj2 = (obj["parentReference"]).toObject();
                QString site_id = obj2["siteId"].toString();
                QString file_name = obj["name"].toString();
                list_file_infos.append({ item_id, site_id, file_name });
                qDebug() << "File name: " << file_name;
                this->getFileContent(site_id, item_id, file_name, false);
            }
        }
        emit filesListReceived(list_file_infos);
    });
}

/*!
 * \brief Authenticator::getFileContent retrieves the content and the version of a specific file.
 * \param site_id the site id for the file
 * \param item_id the item id (file id)
 * \param file_name the name of the file
 * \param open boolean value which says if the file has to be opened in the text editor after it is retrieved or not
 */
void AuthenticatorMicrosoft::getFileContent(QString site_id, QString item_id, QString file_name, bool open)
{
    QUrl url("https://graph.microsoft.com/v1.0/sites/" + site_id + "/drive/items/" + item_id
             + "/content");
    QNetworkReply *reply = this->m_oAuth->get(url);
    connect(reply, &QNetworkReply::finished, [this, reply, site_id, item_id, file_name, open]() {
        if (reply->error()) {
            qDebug() << reply->errorString();
            return;
        }
        qDebug() << "Getting file content";
        QByteArray fileContent = reply->readAll();
        // qDebug() << fileContent;
        QUrl url2("https://graph.microsoft.com/v1.0/sites/" + site_id + "/drive/items/" + item_id
                  + "/versions/current");
        QNetworkReply *reply2 = this->m_oAuth->get(url2);
        connect(reply2, &QNetworkReply::finished,
                [this, fileContent, reply2, site_id, item_id, file_name, open]() {
                    if (reply2->error()) {
                        qDebug() << reply2->errorString();
                        return;
                    }
                    QJsonObject reply_obj = (QJsonDocument::fromJson(reply2->readAll())).object();
                    QString version = reply_obj["id"].toString();
                    qDebug() << "file version: " << version;
                    emit fileContentReceived(file_name, fileContent, site_id, item_id, version, open);
                    // qDebug() << "emitted signal with open: " << open;
                });
    });
}

/*!
 * \brief Authenticator::updateFileContent updates the content of a file online
 * \param new_text the new content of the file
 * \param current_open_file structure containing informations about the currently open file (the one to update)
 */
void AuthenticatorMicrosoft::updateFileContent(QByteArray new_text, struct openFile *current_open_file)
{
    QString site_id = current_open_file->site_id;
    QString item_id = current_open_file->item_id;
    QUrl url("https://graph.microsoft.com/v1.0/sites/" + site_id + "/drive/items/" + item_id
             + "/content");
    QNetworkReply *reply = this->m_oAuth->put(url, new_text);
    // MODIFIED FROM HERE
    connect(reply, &QNetworkReply::finished, [reply, current_open_file]() {
        if (reply->error()) {
            qDebug() << reply->errorString();
            return;
        }
        qDebug() << "updated text sent";
        qDebug() << "current version: " << current_open_file->version;
        float v = (current_open_file->version).toFloat();
        v++;
        QString new_version = QString::number(v) + ".0";
        current_open_file->version = new_version;
        qDebug() << "new version update locally: " << new_version;
    });
    // TO HERE
}

/*!
 * \brief Authenticator::checkVersion performs a check on the version of a file. It checks wether the version of the
 * local file and the version of the online file match.
 * \param site_id the site id for the file
 * \param item_id the item id (file id)
 * \param version the version of the file stored locally
 */
void AuthenticatorMicrosoft::checkVersion(QString site_id, QString item_id, QString version)
{
    QUrl url("https://graph.microsoft.com/v1.0/sites/" + site_id + "/drive/items/" + item_id
             + "/versions/current");
    QNetworkReply *reply = this->m_oAuth->get(url);
    connect(reply, &QNetworkReply::finished, [this, reply, version]() {
        if (reply->error()) {
            qDebug() << reply->errorString();
            return;
        }
        QJsonObject reply_obj = (QJsonDocument::fromJson(reply->readAll())).object();
        QString new_version = reply_obj["id"].toString();
        qDebug() << "my version:";
        qDebug() << version;
        qDebug() << "new version:";
        qDebug() << new_version;
        if (new_version.compare(version, Qt::CaseSensitive) == 0) {
            emit versionChecked(true);
        } else {
            emit versionChecked(false);
        }
    });
}