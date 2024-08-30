//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

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

