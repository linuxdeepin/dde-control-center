//SPDX-FileCopyrightText: 2018 - 2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "securitydbusproxy.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusPendingReply>
#include <QDebug>

SecurityDBusProxy::SecurityDBusProxy(QObject *parent)
    : QObject(parent)
{
    init();
}

QString SecurityDBusProxy::Status()
{
    if (!m_serviceAvailable) {
        return QString();
    }
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
    if (!m_serviceAvailable) {
        return result;
    }
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

    // SecurityEnhance 服务为可选的安全增强组件，并非所有镜像都预装。
    // 若该服务未注册到系统总线上，跳过后续 DBus 调用，避免产生
    // "The name com.deepin.daemon.SecurityEnhance was not provided by any
    // .service files" 这类 ServiceUnknown 报错日志。
    m_serviceAvailable = QDBusConnection::systemBus().interface()->isServiceRegistered(service);
    if (!m_serviceAvailable) {
        return;
    }

    if (!m_dBusInter->isValid()) {
        qWarning() << "Security interface invalid: " << m_dBusInter->lastError().message();
        m_serviceAvailable = false;
        return;
    }
}

