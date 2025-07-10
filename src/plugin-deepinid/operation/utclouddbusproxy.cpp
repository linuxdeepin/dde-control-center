// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "utclouddbusproxy.h"
#include "utils.h"

#include <QDBusConnection>
#include <QDBusInterface>

UtcloudDBusProxy::UtcloudDBusProxy(QObject *parent)
    : QObject(parent)
{
    m_utcloudInter = new QDBusInterface(SYNC_SERVICE, UTCLOUD_PATH, UTCLOUD_INTERFACE, QDBusConnection::sessionBus(), this);
    QDBusConnection dbusConnection = m_utcloudInter->connection();

    dbusConnection.connect(SYNC_SERVICE, UTCLOUD_PATH, UTCLOUD_INTERFACE, "SwitcherChange", "av", this, SIGNAL(SwitcherChange(QVariantList)));
    dbusConnection.connect(SYNC_SERVICE, UTCLOUD_PATH, UTCLOUD_INTERFACE, "LoginStatus", "av", this, SIGNAL(LoginStatus(QVariantList)));

}

void UtcloudDBusProxy::SwitcherSet(const QString &arg_0, bool state)
{
    m_utcloudInter->asyncCallWithArgumentList("SwitcherSet", { arg_0, state });
}

bool UtcloudDBusProxy::SwitcherGet(const QString &arg_0)
{
    QDBusPendingReply<bool> reply = m_utcloudInter->asyncCallWithArgumentList("SwitcherGet", { arg_0 });
    reply.waitForFinished();
    if (!reply.isValid()) {
        return false;
    }
    return reply.value();
}

QDBusPendingCall UtcloudDBusProxy::SwitcherDump()
{
    return m_utcloudInter->asyncCall("SwitcherDump");
}

bool UtcloudDBusProxy::SetNickname(const QString &name, QString &errorMsg)
{
    QDBusPendingReply<bool> reply = m_utcloudInter->asyncCallWithArgumentList("SetNickname", { name });
    reply.waitForFinished();
    if (!reply.isValid()) {
        qWarning() << "set nickname failed, error: " << reply.error().message();
        errorMsg = reply.error().message();
        return false;
    }

    return reply.value();
}
