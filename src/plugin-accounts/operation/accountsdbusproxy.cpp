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

#include <QMetaObject>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QDBusMetaType>
#include <QDebug>

AccountsDBusProxy::AccountsDBusProxy(QObject *parent)
    : QObject(parent)
{
    init();
}

void AccountsDBusProxy::init()
{
    const QString accountsService = "com.deepin.daemon.Accounts";
    const QString accountsPath = "/com/deepin/daemon/Accounts";
    const QString accountsInterface = "com.deepin.daemon.Accounts";

    const QString displayManagerService = "org.freedesktop.DisplayManager";
    const QString displayManagerPath = "/org/freedesktop/DisplayManager";
    const QString displayManagerInterface = "org.freedesktop.DisplayManager";

    const QString propertiesInterface = "org.freedesktop.DBus.Properties";
    const QString propertiesChanged = "PropertiesChanged";

    m_dBusAccountsInter = new QDBusInterface(accountsService, accountsPath, accountsInterface, QDBusConnection::systemBus(), this);
    QDBusConnection dbusConnection = m_dBusAccountsInter->connection();
    dbusConnection.connect(accountsService, accountsPath, propertiesInterface, propertiesChanged, this, SLOT(onPropertiesChanged(QDBusMessage)));

    m_dBusDisplayManagerInter = new QDBusInterface(displayManagerService, displayManagerPath, displayManagerInterface, QDBusConnection::systemBus(), this);
    dbusConnection = m_dBusDisplayManagerInter->connection();
    dbusConnection.connect(displayManagerService, displayManagerPath, propertiesInterface, propertiesChanged, this, SLOT(onPropertiesChanged(QDBusMessage)));
}

//Accounts
bool AccountsDBusProxy::allowGuest()
{
    return qvariant_cast<bool>(m_dBusAccountsInter->property("AllowGuest"));
}

QString AccountsDBusProxy::guestIcon()
{
    return qvariant_cast<QString>(m_dBusAccountsInter->property("GuestIcon"));
}

QStringList AccountsDBusProxy::userList()
{
    return qvariant_cast<QStringList>(m_dBusAccountsInter->property("UserList"));
}

QList<QDBusObjectPath> AccountsDBusProxy::seats()
{
    return qvariant_cast<QList<QDBusObjectPath>>(m_dBusDisplayManagerInter->property("Seats"));
}

QList<QDBusObjectPath> AccountsDBusProxy::sessions()
{
    return qvariant_cast<QList<QDBusObjectPath>>(m_dBusDisplayManagerInter->property("Sessions"));
}

QDBusPendingReply<> AccountsDBusProxy::AllowGuestAccount(bool in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusAccountsInter->asyncCallWithArgumentList(QStringLiteral("AllowGuestAccount"), argumentList);
}


QDBusPendingReply<QString> AccountsDBusProxy::CreateGuestAccount()
{
    QList<QVariant> argumentList;
    return m_dBusAccountsInter->asyncCallWithArgumentList(QStringLiteral("CreateGuestAccount"), argumentList);
}



QDBusPendingReply<QDBusObjectPath> AccountsDBusProxy::CreateUser(const QString &in0, const QString &in1, int in2)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1) << QVariant::fromValue(in2);
    return m_dBusAccountsInter->asyncCallWithArgumentList(QStringLiteral("CreateUser"), argumentList);
}



QDBusPendingReply<> AccountsDBusProxy::DeleteUser(const QString &in0, bool in1)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1);
    return m_dBusAccountsInter->asyncCallWithArgumentList(QStringLiteral("DeleteUser"), argumentList);
}

QDBusPendingReply<QString> AccountsDBusProxy::FindUserById(const qint64 &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusAccountsInter->asyncCallWithArgumentList(QStringLiteral("FindUserById"), argumentList);
}

QDBusPendingReply<QString> AccountsDBusProxy::FindUserByName(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusAccountsInter->asyncCallWithArgumentList(QStringLiteral("FindUserByName"), argumentList);
}

QDBusPendingReply<QStringList> AccountsDBusProxy::GetGroups()
{
    QList<QVariant> argumentList;
    return m_dBusAccountsInter->asyncCallWithArgumentList(QStringLiteral("GetGroups"), argumentList);
}

QDBusPendingReply<QStringList> AccountsDBusProxy::GetPresetGroups(int in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusAccountsInter->asyncCallWithArgumentList(QStringLiteral("GetPresetGroups"), argumentList);
}

QDBusPendingReply<bool, QString, int> AccountsDBusProxy::IsPasswordValid(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusAccountsInter->asyncCallWithArgumentList(QStringLiteral("IsPasswordValid"), argumentList);
}

QDBusPendingReply<bool, QString, int> AccountsDBusProxy::IsUsernameValid(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusAccountsInter->asyncCallWithArgumentList(QStringLiteral("IsUsernameValid"), argumentList);
}

QDBusPendingReply<QString> AccountsDBusProxy::RandUserIcon()
{
    QList<QVariant> argumentList;
    return m_dBusAccountsInter->asyncCallWithArgumentList(QStringLiteral("RandUserIcon"), argumentList);
}

void AccountsDBusProxy::onPropertiesChanged(const QDBusMessage &message)
{
    QVariantMap changedProps = qdbus_cast<QVariantMap>(message.arguments().at(1).value<QDBusArgument>());
    for (QVariantMap::const_iterator it = changedProps.begin(); it != changedProps.end(); ++it) {
        QMetaObject::invokeMethod(this, it.key().toLatin1() + "Changed", Qt::DirectConnection, QGenericArgument(it.value().typeName(), it.value().data()));
    }
}
