/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef ADDVPNPAGE_H
#define ADDVPNPAGE_H

#include <dboxwidget.h>
#include <libdui_global.h>

#include "dbus/dbusconnectionsession.h"

DWIDGET_USE_NAMESPACE

class AddVpnPage : public DVBoxWidget
{
    Q_OBJECT

public:
    explicit AddVpnPage(const QString &dbusPath, QWidget *parent = 0);
    ~AddVpnPage();

private:
    void init();

    DBusConnectionSession *m_dbus;
    QMap<QString, QString> m_mapVpnNameToType;
};

#endif // ADDVPNPAGE_H
