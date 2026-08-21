// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "syncdbusproxy.h"
#include "utils.h"

SyncDBusProxy::SyncDBusProxy(QObject *parent)
    : QObject(parent)
    , m_syncInter(new DDBusInterface(SYNC_SERVICE, SYNC_PATH, SYNC_INTERFACE, QDBusConnection::sessionBus(), this))
{

}

void SyncDBusProxy::SwitcherSet(const QString &arg_0, bool state)
{
    m_syncInter->asyncCallWithArgumentList("SwitcherSet", { arg_0, state });
}

bool SyncDBusProxy::SwitcherGet(const QString &arg_0)
{
    QDBusPendingReply<bool> reply = m_syncInter->asyncCallWithArgumentList("SwitcherGet", { arg_0 });
    reply.waitForFinished();
    if (!reply.isValid()) {
        return false;
    }
    return reply.value();
}

QDBusPendingCall SyncDBusProxy::SwitcherDump()
{
    return m_syncInter->asyncCall("SwitcherDump");
}

qlonglong SyncDBusProxy::lastSyncTime()
{
    return m_syncInter->property("LastSyncTime").toLongLong();
}
