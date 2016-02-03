/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef PERSIONALIZATIONFWD
#define PERSIONALIZATIONFWD

#include <QMetaType>

typedef QList<QMap<QString, QString>> ImageInfoList;
typedef QMap<QString, QJsonObject> JosnMapObjs;

Q_DECLARE_METATYPE(ImageInfoList)
Q_DECLARE_METATYPE(JosnMapObjs)

#endif // PERSIONALIZATIONFWD

