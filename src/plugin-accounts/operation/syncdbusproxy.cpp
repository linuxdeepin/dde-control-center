/*
* Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
*
* Author:     guoyao <guoyao@uniontech.com>
*
* Maintainer: guoyao <guoyao@uniontech.com>
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
#include "syncdbusproxy.h"

#include <QMetaObject>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QDBusMetaType>
#include <QDBusReply>
#include <QDebug>
#include <QString>

SyncDBusProxy::SyncDBusProxy(QObject *parent)
    : QObject(parent)
{
    init();
}

QString SyncDBusProxy::UOSID()
{
    QDBusReply<QString> retUOSID = m_dBusInter->call("UOSID");
    m_lastError = retUOSID.error().message();
    if (m_lastError.isEmpty()) {
        return retUOSID.value();
    } else {
        qWarning() << "UOSID failed:" << m_lastError;
        return QString();
    }
}

QString SyncDBusProxy::LocalBindCheck(const QString &uosid, const QString &uuid)
{
    QDBusReply<QString> retLocalBindCheck = m_dBusInter->call(QDBus::BlockWithGui, "LocalBindCheck", uosid, uuid);
    m_lastError = retLocalBindCheck.error().message();
    if (m_lastError.isEmpty()) {
         return retLocalBindCheck.value();
    } else {
        qWarning() << "localBindCheck failed:" << m_lastError;
        return QString();
    }
}

void SyncDBusProxy::init()
{
    const QString &service = QStringLiteral("com.deepin.sync.Helper");
    const QString &path = QStringLiteral("/com/deepin/sync/Helper");
    const QString &interface = QStringLiteral("com.deepin.sync.Helper");

    m_dBusInter = new QDBusInterface(service, path, interface, QDBusConnection::systemBus(), this);

    if (!m_dBusInter->isValid()) {
        qWarning() << "syncHelper interface invalid: " << m_dBusInter->lastError().message();
        return;
    }
}

