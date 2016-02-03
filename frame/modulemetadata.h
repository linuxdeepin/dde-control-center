/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

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
