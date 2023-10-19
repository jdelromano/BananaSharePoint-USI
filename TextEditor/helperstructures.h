#ifndef HELPERSTRUCTURES_H
#define HELPERSTRUCTURES_H

#include <QMainWindow>

struct fileInfos{
    QString item_id;
    QString site_id;
    QString file_name;
}; Q_DECLARE_METATYPE(fileInfos);

#endif // HELPERSTRUCTURES_H
