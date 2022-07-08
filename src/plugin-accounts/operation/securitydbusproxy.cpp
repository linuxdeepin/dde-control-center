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

    m_dBusInter = new DCC_NAMESPACE::DCCDBusInterface(service, path, interface, QDBusConnection::systemBus(), this);

    if (!m_dBusInter->isValid()) {
        qWarning() << "Security interface invalid: " << m_dBusInter->lastError().message();
        return;
    }
}

