/*
 * Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     Tinalu Shao <shaotianlu@uniontech.com>
 *
 * Maintainer: Tinalu Shao <shaotianlu@uniontech.com>
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
#include "accountsdbusproxy.h"

#include "widgets/dccdbusinterface.h"

#include <QDBusConnection>
#include <QDBusMetaType>
#include <QDBusPendingReply>
#include <QDebug>
#include <QMetaObject>

AccountsDBusProxy::AccountsDBusProxy(QObject *parent)
    : QObject(parent)
{
    init();
}

void AccountsDBusProxy::init()
{
    const QString accountsService = "org.deepin.dde.Accounts1";
    const QString accountsPath = "/org/deepin/dde/Accounts1";
    const QString accountsInterface = "org.deepin.dde.Accounts1";

    const QString displayManagerService = "org.freedesktop.DisplayManager";
    const QString displayManagerPath = "/org/freedesktop/DisplayManager";
    const QString displayManagerInterface = "org.freedesktop.DisplayManager";

    const QString propertiesInterface = "org.freedesktop.DBus.Properties";
    const QString propertiesChanged = "PropertiesChanged";

    m_dBusAccountsInter = new DCC_NAMESPACE::DCCDBusInterface(accountsService,
                                                              accountsPath,
                                                              accountsInterface,
                                                              QDBusConnection::systemBus(),
                                                              this);
    m_dBusDisplayManagerInter = new DCC_NAMESPACE::DCCDBusInterface(displayManagerService,
                                                                    displayManagerPath,
                                                                    displayManagerInterface,
                                                                    QDBusConnection::systemBus(),
                                                                    this);
}

// Accounts
QStringList AccountsDBusProxy::userList()
{
    return qvariant_cast<QStringList>(m_dBusAccountsInter->property("UserList"));
}

QList<QDBusObjectPath> AccountsDBusProxy::sessions()
{
    return qvariant_cast<QList<QDBusObjectPath>>(m_dBusDisplayManagerInter->property("Sessions"));
}

QDBusPendingReply<QDBusObjectPath> AccountsDBusProxy::CreateUser(const QString &in0,
                                                                 const QString &in1,
                                                                 int in2)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1)
                 << QVariant::fromValue(in2);
    return m_dBusAccountsInter->asyncCallWithArgumentList(QStringLiteral("CreateUser"),
                                                          argumentList);
}

QDBusPendingReply<> AccountsDBusProxy::DeleteUser(const QString &in0, bool in1)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1);
    return m_dBusAccountsInter->asyncCallWithArgumentList(QStringLiteral("DeleteUser"),
                                                          argumentList);
}

QDBusPendingReply<QString> AccountsDBusProxy::FindUserById(const qint64 &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusAccountsInter->asyncCallWithArgumentList(QStringLiteral("FindUserById"),
                                                          argumentList);
}

QDBusPendingReply<QString> AccountsDBusProxy::FindUserByName(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusAccountsInter->asyncCallWithArgumentList(QStringLiteral("FindUserByName"),
                                                          argumentList);
}

QDBusPendingReply<QStringList> AccountsDBusProxy::GetGroups()
{
    QList<QVariant> argumentList;
    return m_dBusAccountsInter->asyncCallWithArgumentList(QStringLiteral("GetGroups"),
                                                          argumentList);
}

QDBusPendingReply<QStringList> AccountsDBusProxy::GetPresetGroups(int in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusAccountsInter->asyncCallWithArgumentList(QStringLiteral("GetPresetGroups"),
                                                          argumentList);
}

QDBusPendingReply<bool, QString, int> AccountsDBusProxy::IsPasswordValid(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusAccountsInter->asyncCallWithArgumentList(QStringLiteral("IsPasswordValid"),
                                                          argumentList);
}

QDBusPendingReply<bool, QString, int> AccountsDBusProxy::IsUsernameValid(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusAccountsInter->asyncCallWithArgumentList(QStringLiteral("IsUsernameValid"),
                                                          argumentList);
}

QDBusPendingReply<QString> AccountsDBusProxy::RandUserIcon()
{
    QList<QVariant> argumentList;
    return m_dBusAccountsInter->asyncCallWithArgumentList(QStringLiteral("RandUserIcon"),
                                                          argumentList);
}
