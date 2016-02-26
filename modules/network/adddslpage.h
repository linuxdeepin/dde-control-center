/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef ADDDSLPAGE_H
#define ADDDSLPAGE_H

#include "listwidgetcontainer.h"
#include "dbus/dbusconnectionsession.h"

class AddDslPage : public ListWidgetContainer
{
    Q_OBJECT

public:
    explicit AddDslPage(const QString &dbusPath, QWidget *parent = 0);
    ~AddDslPage();

private:
    DBusConnectionSession *m_dbus;
};

#endif // ADDDSLPAGE_H
