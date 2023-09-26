//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "powerdbusproxy.h"

#include <QDBusPendingReply>

const QString PowerService = QStringLiteral("org.deepin.dde.Power1");
const QString PowerPath = QStringLiteral("/org/deepin/dde/Power1");
const QString PowerInterface = QStringLiteral("org.deepin.dde.Power1");

const QString Login1ManagerService = QStringLiteral("org.freedesktop.login1");
const QString Login1ManagerPath = QStringLiteral("/org/freedesktop/login1");
const QString Login1ManagerInterface = QStringLiteral("org.freedesktop.login1.Manager");

PowerDBusProxy::PowerDBusProxy(QObject *parent)
    : QObject(parent)
    , m_powerInter(new DDBusInterface(
              PowerService, PowerPath, PowerInterface, QDBusConnection::sessionBus(), this))
    , m_login1ManagerInter(new DDBusInterface(Login1ManagerService,
                                              Login1ManagerPath,
                                              Login1ManagerInterface,
                                              QDBusConnection::systemBus(),
                                              this))
{
}

// power
bool PowerDBusProxy::screenBlackLock()
{
    return qvariant_cast<bool>(m_powerInter->property("ScreenBlackLock"));
}

void PowerDBusProxy::setScreenBlackLock(bool value)
{
    m_powerInter->setProperty("ScreenBlackLock", QVariant::fromValue(value));
}

bool PowerDBusProxy::sleepLock()
{
    return qvariant_cast<bool>(m_powerInter->property("SleepLock"));
}

void PowerDBusProxy::setSleepLock(bool value)
{
    m_powerInter->setProperty("SleepLock", QVariant::fromValue(value));
}

bool PowerDBusProxy::login1ManagerCanSuspend()
{
    QList<QVariant> argumentList;
    QDBusPendingReply<QString> reply = m_login1ManagerInter->asyncCallWithArgumentList(QStringLiteral("CanSuspend"), argumentList);
    reply.waitForFinished();
    return reply.value().contains("yes");
}
