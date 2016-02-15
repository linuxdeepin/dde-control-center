/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef NETWORKINFO_H
#define NETWORKINFO_H

#include <QMap>

#include <dboxwidget.h>

#include "dbus/dbusnetwork.h"

DWIDGET_USE_NAMESPACE

class NetworkInfo : public DVBoxWidget
{
    Q_OBJECT
public:
    explicit NetworkInfo(DBusNetwork *dbus, QWidget *parent = 0);

private:
    void init(DBusNetwork *dbus);

    QMap<QString, QString> m_mapConnectionTypeToName;
};

#endif // NETWORKINFO_H
