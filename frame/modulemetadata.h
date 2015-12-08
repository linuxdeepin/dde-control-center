#ifndef MODULEMETADATA_H
#define MODULEMETADATA_H

#include <QString>
#include <QObject>

struct ModuleMetaData {
    QString path;
    QString id;
    QString name;
};

Q_DECLARE_METATYPE(ModuleMetaData)

#endif // MODULEMETADATA_H
