#ifndef HELPERSTRUCTURES_H
#define HELPERSTRUCTURES_H

#include <QMainWindow>

/*!
 * \brief The fileInfos structure contains informations about a file
 */
struct fileInfos{
    QString item_id;
    QString site_id;
    QString file_name;
}; Q_DECLARE_METATYPE(fileInfos);

/*!
 * \brief The openFile structure contains informations about the currently open file in the text editor
 */
struct openFile{
    QString fileName;
    QString site_id;
    QString item_id;
    QString version;
}; Q_DECLARE_METATYPE(openFile);

#endif // HELPERSTRUCTURES_H
