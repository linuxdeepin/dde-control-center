/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef CONNECTTOHIDDENAPPAGE_H
#define CONNECTTOHIDDENAPPAGE_H

#include "dbus/dbusconnectionsession.h"
#include "networkglobal.h"
#include "listwidgetcontainer.h"

class NetworkBaseEditLine;
class ConnectToHiddenApPage : public ListWidgetContainer
{
    Q_OBJECT
public:
    explicit ConnectToHiddenApPage(DBusConnectionSession *dbus, QWidget *parent = 0);
    ~ConnectToHiddenApPage();

signals:
    void cancel();
    void confirm();

private:
    void init();

    DBusConnectionSession *m_dbus;
};

#endif // CONNECTTOHIDDENAPPAGE_H
