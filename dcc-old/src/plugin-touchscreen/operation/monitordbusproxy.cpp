//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
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
    m_dBusMonitorInter = new QDBusInterface("org.deepin.dde.Display1", m_monitorUserPath, "org.deepin.dde.Display1.Monitor", QDBusConnection::sessionBus(), this);
}

QString MonitorDBusProxy::name()
{
    return qvariant_cast<QString>(m_dBusMonitorInter->property("Name"));
}
