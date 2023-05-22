//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "securitydbusproxy.h"
#include "widgets/dccdbusinterface.h"

#include <QDBusConnection>
#include <QDBusPendingReply>
#include <QDebug>

SecurityDBusProxy::SecurityDBusProxy(QObject *parent)
    : QObject(parent)
{
    init();
}

QString SecurityDBusProxy::Status()
{
    QDBusPendingReply<QString> reply = m_dBusInter->asyncCall("Status");
    reply.waitForFinished();
    if (reply.isError()) {
        m_lastError = reply.error().message();
    } else {
        return reply.argumentAt<0>();
    }
    return QString();
}

std::tuple<QString, QString> SecurityDBusProxy::GetSEUserByName(const QString &user)
{
    Q_UNUSED(user)
    std::tuple<QString, QString> result;
    QDBusPendingReply<QString, QString> reply = m_dBusInter->asyncCall("GetSEUserByName");
    reply.waitForFinished();
    if (reply.isError()) {
        m_lastError = reply.error().message();
    } else {
        result = std::make_tuple(reply.argumentAt<0>(), reply.argumentAt<1>());
    }
    return result;
}

void SecurityDBusProxy::init()
{
    const QString &service = QStringLiteral("com.deepin.daemon.SecurityEnhance");
    const QString &path = QStringLiteral("/com/deepin/daemon/SecurityEnhance");
    const QString &interface = QStringLiteral("com.deepin.daemon.SecurityEnhance");

    m_dBusInter = new DDBusInterface(service, path, interface, QDBusConnection::systemBus(), this);

    if (!m_dBusInter->isValid()) {
        qWarning() << "Security interface invalid: " << m_dBusInter->lastError().message();
        return;
    }
}

