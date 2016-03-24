/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef EDITCONNECTIONPAGE_H
#define EDITCONNECTIONPAGE_H

#include <QWidget>
#include <QJsonObject>

#include <libdui/darrowlineexpand.h>

#include "listwidgetcontainer.h"
#include "networkbaseeditline.h"

DUI_USE_NAMESPACE

class DBusConnectionSession;
class EditConnectionPage : public ListWidgetContainer
{
    Q_OBJECT
public:
    explicit EditConnectionPage(const QString &dbusPath, QWidget *parent = 0);
    ~EditConnectionPage();

private:
    NetworkBaseEditLine *getLineByMap(const QVariantMap &map);
    void onExpand(bool e);

    DBusConnectionSession *m_dbus;
    QMap<DArrowLineExpand*, QJsonObject> m_expandToJsonObject;
};

#endif // EDITCONNECTIONPAGE_H
