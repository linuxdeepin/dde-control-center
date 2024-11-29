// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "deepiniddbusproxy.h"
#include "utils.h"

DeepinidDBusProxy::DeepinidDBusProxy(QObject *parent)
    : QObject(parent)
    , m_deepinId(new DDBusInterface(DEEPINID_SERVICE, DEEPINID_PATH, DEEPINID_INTERFACE, QDBusConnection::sessionBus(), this))
{
}

void DeepinidDBusProxy::login() const
{
    m_deepinId->asyncCall("Login");
}

void DeepinidDBusProxy::logout() const
{
    m_deepinId->asyncCall("Logout");
}

QDBusReply<QString> DeepinidDBusProxy::localBindCheck(const QString &uuid)
{
    return m_deepinId->call(QDBus::BlockWithGui, "LocalBindCheck", uuid);
}

QDBusReply<QString> DeepinidDBusProxy::meteInfo()
{
    return m_deepinId->call("MeteInfo");
}

QVariantMap DeepinidDBusProxy::userInfo()
{
    return m_deepinId->property("UserInfo").toMap();
}
