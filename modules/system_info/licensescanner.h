/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef LICENSESSCANNER_H
#define LICENSESSCANNER_H

#include <QString>
#include <QMap>

class licenseScanner
{
public:
    static QMap<QString, QString> scan();
};

#endif // LICENSESSCANNER_H
