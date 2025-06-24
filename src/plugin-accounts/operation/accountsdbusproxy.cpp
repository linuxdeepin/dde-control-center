//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "accountsdbusproxy.h"

#include <QMetaObject>
#include <QDBusConnection>
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
    const QString accountsService = "org.deepin.dde.Accounts1";
    const QString accountsPath = "/org/deepin/dde/Accounts1";
    const QString accountsInterface = "org.deepin.dde.Accounts1";
    const QString propertiesInterface = "org.freedesktop.DBus.Properties";
    const QString propertiesChanged = "PropertiesChanged";

    const QString displayManagerService = "org.freedesktop.DisplayManager";
    const QString displayManagerPath = "/org/freedesktop/DisplayManager";
    const QString displayManagerInterface = "org.freedesktop.DisplayManager";

    m_dBusAccountsInter = new DDBusInterface(accountsService, accountsPath, accountsInterface, QDBusConnection::systemBus(), this);
    m_dBusDisplayManagerInter = new DDBusInterface(displayManagerService, displayManagerPath, displayManagerInterface, QDBusConnection::systemBus(), this);

    QDBusConnection dbusConnection = m_dBusAccountsInter->connection();
    dbusConnection.connect(accountsService, accountsPath, propertiesInterface, propertiesChanged, this, SLOT(onPropertiesChanged(QDBusMessage)));
}

void AccountsDBusProxy::onPropertiesChanged(const QDBusMessage &message)
{
    if (message.arguments().size() < 2)
        return;
    QVariantMap changedProps = qdbus_cast<QVariantMap>(message.arguments().at(1).value<QDBusArgument>());
    if (changedProps.contains("QuickLoginEnabled")) {
        bool value = changedProps.value("QuickLoginEnabled").toBool();
        Q_EMIT QuickLoginEnabledChanged(value);
    }
}

// Accounts
QStringList AccountsDBusProxy::userList()
{
    return qvariant_cast<QStringList>(m_dBusAccountsInter->property("UserList"));
}

QStringList AccountsDBusProxy::groupList()
{
    return qvariant_cast<QStringList>(m_dBusAccountsInter->property("GroupList"));
}

QList<QDBusObjectPath> AccountsDBusProxy::sessions()
{
    return qvariant_cast<QList<QDBusObjectPath>>(m_dBusDisplayManagerInter->property("Sessions"));
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

QDBusPendingReply<QString> AccountsDBusProxy::GetGroupInfoByName(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusAccountsInter->asyncCallWithArgumentList(QStringLiteral("GetGroupInfoByName"), argumentList);
}

QDBusPendingReply<> AccountsDBusProxy::deleteGroup(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(true); // force = true
    return m_dBusAccountsInter->asyncCallWithArgumentList(QStringLiteral("DeleteGroup"), argumentList);
}

QDBusPendingReply<> AccountsDBusProxy::createGroup(const QString &in0, uint32_t in1, bool in2)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1) << QVariant::fromValue(in2);
    return m_dBusAccountsInter->asyncCallWithArgumentList(QStringLiteral("CreateGroup"), argumentList);
}

QDBusPendingReply<> AccountsDBusProxy::modifyGroup(const QString &in0, const QString &in1, uint32_t in2)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1) << QVariant::fromValue(in2);
    return m_dBusAccountsInter->asyncCallWithArgumentList(QStringLiteral("ModifyGroup"), argumentList);
}

bool AccountsDBusProxy::quickLoginEnabled()
{
    QVariant value = m_dBusAccountsInter->property("QuickLoginEnabled");
    return value.isValid() ? value.toBool() : true; // 默认为true
}
