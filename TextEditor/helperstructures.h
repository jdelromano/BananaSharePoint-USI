#ifndef HELPERSTRUCTURES_H
#define HELPERSTRUCTURES_H

#include <QMainWindow>

struct fileInfos{
    QString item_id;
    QString site_id;
    QString file_name;
}; Q_DECLARE_METATYPE(fileInfos);

struct openFile{
    QString fileName;
    QString site_id;
    QString item_id;
    QString version;
}; Q_DECLARE_METATYPE(openFile);

#endif // HELPERSTRUCTURES_H
