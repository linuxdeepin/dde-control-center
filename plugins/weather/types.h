/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef TYPES_H
#define TYPES_H

#include <QObject>

struct City {
    QString country;
    QString region;
    QString city;
    float latitude;
    float longitude;
};

Q_DECLARE_METATYPE(City)

#endif // TYPES_H
