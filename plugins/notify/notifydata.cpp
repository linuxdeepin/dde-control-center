/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "notifydata.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonArray>
#include <QDebug>
#include <QDBusInterface>
#include <QDBusConnection>

NotifyData::NotifyData(QObject *parent):QObject(parent)
{
    QDBusConnection sessionConn = QDBusConnection::sessionBus();
    sessionConn.registerObject("/org/freedesktop/Notifications", this, QDBusConnection::ExportAllSlots);

    QString matchString = "interface='org.freedesktop.Notifications',member='Notify',type='method_call',eavesdrop='true'";
    QDBusInterface *busInterface = new QDBusInterface("org.freedesktop.DBus", "/org/freedesktop/DBus",
                                "org.freedesktop.DBus");
    busInterface->call("AddMatch", matchString);
}

NotifyData::~NotifyData()
{

}

void NotifyData::Notify(const QString &app_name, uint replaces_id, const QString &app_icon, const QString &summary, const QString &body, const QStringList &actions, const QVariantHash &hints, int expire_timeout)
{
    Q_UNUSED(replaces_id);
    Q_UNUSED(actions);
    Q_UNUSED(hints);
    Q_UNUSED(expire_timeout);

    QJsonObject json;
    json.insert("appName", app_name);
    json.insert("appIcon", app_icon);
    json.insert("summary", summary);
    json.insert("body"   , body);
    QJsonDocument document;
    document.setObject(json);
    QByteArray byte_array = document.toJson(QJsonDocument::Compact);
    emit ValueChanged(byte_array);
}
