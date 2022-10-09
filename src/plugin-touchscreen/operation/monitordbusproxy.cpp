/*
* Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
*
* Author:     Tinalu Shao <shaotianlu@uniontech.com>
*
* Maintainer: Tinalu Shao <shaotianlu@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "monitordbusproxy.h"

#include <QMetaObject>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QDBusMetaType>
#include <QDebug>

MonitorDBusProxy::MonitorDBusProxy(QString monitorPath, QObject *parent)
    : QObject(parent)
    , m_monitorUserPath(monitorPath)
{
    init();
}

void MonitorDBusProxy::init()
{
    m_dBusMonitorInter = new QDBusInterface("com.deepin.daemon.Display", m_monitorUserPath, "com.deepin.daemon.Display.Monitor", QDBusConnection::sessionBus(), this);
}

QString MonitorDBusProxy::name()
{
    return qvariant_cast<QString>(m_dBusMonitorInter->property("Name"));
}
