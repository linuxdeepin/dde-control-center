//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "userdbusproxy.h"

#include <QMetaObject>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QDBusMetaType>
#include <QDebug>

UserDBusProxy::UserDBusProxy(QString accountsUserPath, QObject *parent)
    : QObject(parent)
    , m_accountsUserPath(accountsUserPath)
{
    init();
}

void UserDBusProxy::init()
{
    const QString AccountsUserService = "org.deepin.dde.Accounts1";
    const QString AccountsUserInterface = "org.deepin.dde.Accounts1.User";
    const QString PropertiesInterface = "org.freedesktop.DBus.Properties";
    const QString PropertiesChanged = "PropertiesChanged";

    m_dBusAccountsUserInter = new QDBusInterface(AccountsUserService, m_accountsUserPath, AccountsUserInterface, QDBusConnection::systemBus(), this);
    QDBusConnection dbusConnection = m_dBusAccountsUserInter->connection();
    dbusConnection.connect(AccountsUserService, m_accountsUserPath, PropertiesInterface, PropertiesChanged, this, SLOT(onPropertiesChanged(QDBusMessage)));
}

//users
QDBusPendingReply<> UserDBusProxy::AddGroup(const QString &group)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(group);
    return m_dBusAccountsUserInter->asyncCallWithArgumentList(QStringLiteral("AddGroup"), argumentList);
}
QDBusPendingReply<> UserDBusProxy::DeleteGroup(const QString &group)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(group);
    return m_dBusAccountsUserInter->asyncCallWithArgumentList(QStringLiteral("DeleteGroup"), argumentList);
}
QDBusPendingReply<> UserDBusProxy::DeleteIconFile(const QString &iconFile)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(iconFile);
    return m_dBusAccountsUserInter->asyncCallWithArgumentList(QStringLiteral("DeleteIconFile"), argumentList);
}
QDBusPendingReply<> UserDBusProxy::EnableNoPasswdLogin(bool enabled)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(enabled);
    return m_dBusAccountsUserInter->asyncCallWithArgumentList(QStringLiteral("EnableNoPasswdLogin"), argumentList);
}
QDBusPendingReply<bool> UserDBusProxy::IsPasswordExpired()
{
    QList<QVariant> argumentList;
    return m_dBusAccountsUserInter->asyncCallWithArgumentList(QStringLiteral("IsPasswordExpired"), argumentList);
}
QDBusPendingReply<> UserDBusProxy::SetAutomaticLogin(bool enabled)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(enabled);
    return m_dBusAccountsUserInter->asyncCallWithArgumentList(QStringLiteral("SetAutomaticLogin"), argumentList);
}
QDBusPendingReply<> UserDBusProxy::SetDesktopBackgrounds(const QStringList &backgrounds)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(backgrounds);
    return m_dBusAccountsUserInter->asyncCallWithArgumentList(QStringLiteral("SetDesktopBackgrounds"), argumentList);
}
QDBusPendingReply<> UserDBusProxy::SetFullName(const QString &name)
{
    qInfo() << "m_accountsUserPath" << m_accountsUserPath;
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(name);
    return m_dBusAccountsUserInter->asyncCallWithArgumentList(QStringLiteral("SetFullName"), argumentList);
}
QDBusPendingReply<> UserDBusProxy::SetGreeterBackground(const QString &background)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(background);
    return m_dBusAccountsUserInter->asyncCallWithArgumentList(QStringLiteral("SetGreeterBackground"), argumentList);
}
QDBusPendingReply<> UserDBusProxy::SetGroups(const QStringList &groups)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(groups);
    return m_dBusAccountsUserInter->asyncCallWithArgumentList(QStringLiteral("SetGroups"), argumentList);
}
QDBusPendingReply<> UserDBusProxy::SetHistoryLayout(const QStringList &layouts)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(layouts);
    return m_dBusAccountsUserInter->asyncCallWithArgumentList(QStringLiteral("SetHistoryLayout"), argumentList);
}
QDBusPendingReply<> UserDBusProxy::SetHomeDir(const QString &home)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(home);
    return m_dBusAccountsUserInter->asyncCallWithArgumentList(QStringLiteral("SetHomeDir"), argumentList);
}
QDBusPendingReply<> UserDBusProxy::SetIconFile(const QString &iconFile)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(iconFile);
    return m_dBusAccountsUserInter->asyncCallWithArgumentList(QStringLiteral("SetIconFile"), argumentList);
}
QDBusPendingReply<> UserDBusProxy::SetLayout(const QString &layout)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(layout);
    return m_dBusAccountsUserInter->asyncCallWithArgumentList(QStringLiteral("SetLayout"), argumentList);
}
QDBusPendingReply<> UserDBusProxy::SetLocale(const QString &locale)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(locale);
    return m_dBusAccountsUserInter->asyncCallWithArgumentList(QStringLiteral("SetLocale"), argumentList);
}
QDBusPendingReply<> UserDBusProxy::SetLocked(bool locked)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(locked);
    return m_dBusAccountsUserInter->asyncCallWithArgumentList(QStringLiteral("SetLocked"), argumentList);
}
QDBusPendingReply<> UserDBusProxy::SetMaxPasswordAge(int nDays)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(nDays);
    return m_dBusAccountsUserInter->asyncCallWithArgumentList(QStringLiteral("SetMaxPasswordAge"), argumentList);
}
QDBusPendingReply<> UserDBusProxy::SetPassword(const QString &password)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(password);
    return m_dBusAccountsUserInter->asyncCallWithArgumentList(QStringLiteral("SetPassword"), argumentList);
}
QDBusPendingReply<> UserDBusProxy::SetPasswordHint(const QString &hint)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(hint);
    return m_dBusAccountsUserInter->asyncCallWithArgumentList(QStringLiteral("SetPasswordHint"), argumentList);
}
QDBusPendingReply<> UserDBusProxy::SetShell(const QString &shell)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(shell);
    return m_dBusAccountsUserInter->asyncCallWithArgumentList(QStringLiteral("SetShell"), argumentList);
}

QDBusPendingReply<QList<int>> UserDBusProxy::GetSecretQuestions()
{
    //获取安全问题需要使用同步调用
    return m_dBusAccountsUserInter->call(QStringLiteral("GetSecretQuestions"));
}

QDBusPendingReply<> UserDBusProxy::SetSecretQuestions(const QMap<int, QByteArray> &securityQuestions)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(securityQuestions);
    return m_dBusAccountsUserInter->asyncCallWithArgumentList(QStringLiteral("SetSecretQuestions"), argumentList);
}

//获取属性值
int UserDBusProxy::accountType()
{
    return qvariant_cast<int>(m_dBusAccountsUserInter->property("AccountType"));
}
bool UserDBusProxy::automaticLogin()
{
    return qvariant_cast<bool>(m_dBusAccountsUserInter->property("AutomaticLogin"));
}
qulonglong UserDBusProxy::createdTime()
{
    return qvariant_cast<qulonglong>(m_dBusAccountsUserInter->property("CreatedTime"));
}
QStringList UserDBusProxy::desktopBackgrounds()
{
    return qvariant_cast<QStringList>(m_dBusAccountsUserInter->property("DesktopBackgrounds"));
}
QString UserDBusProxy::fullName()
{
    return qvariant_cast<QString>(m_dBusAccountsUserInter->property("FullName"));
}
QString UserDBusProxy::gid()
{
    return qvariant_cast<QString>(m_dBusAccountsUserInter->property("Gid"));
}
QString UserDBusProxy::greeterBackground()
{
    return qvariant_cast<QString>(m_dBusAccountsUserInter->property("GreeterBackground"));
}
QStringList UserDBusProxy::groups()
{
    return qvariant_cast<QStringList>(m_dBusAccountsUserInter->property("Groups"));
}
QStringList UserDBusProxy::historyLayout()
{
    return qvariant_cast<QStringList>(m_dBusAccountsUserInter->property("HistoryLayout"));
}
QString UserDBusProxy::homeDir()
{
    return qvariant_cast<QString>(m_dBusAccountsUserInter->property("HomeDir"));
}
QString UserDBusProxy::iconFile()
{
    return qvariant_cast<QString>(m_dBusAccountsUserInter->property("IconFile"));
}
QStringList UserDBusProxy::iconList()
{
    return qvariant_cast<QStringList>(m_dBusAccountsUserInter->property("IconList"));
}
QString UserDBusProxy::layout()
{
    return qvariant_cast<QString>(m_dBusAccountsUserInter->property("Layout"));
}
QString UserDBusProxy::locale()
{
    return qvariant_cast<QString>(m_dBusAccountsUserInter->property("Locale"));
}
bool UserDBusProxy::locked()
{
    return qvariant_cast<int>(m_dBusAccountsUserInter->property("Locked"));
}
qulonglong UserDBusProxy::loginTime()
{
    return qvariant_cast<qulonglong>(m_dBusAccountsUserInter->property("LoginTime"));
}
int UserDBusProxy::longDateFormat()
{
    return qvariant_cast<int>(m_dBusAccountsUserInter->property("LongDateFormat"));
}
int UserDBusProxy::longTimeFormat()
{
    return qvariant_cast<int>(m_dBusAccountsUserInter->property("LongTimeFormat"));
}
int UserDBusProxy::maxPasswordAge()
{
    return qvariant_cast<int>(m_dBusAccountsUserInter->property("MaxPasswordAge"));
}
bool UserDBusProxy::noPasswdLogin()
{
    return qvariant_cast<bool>(m_dBusAccountsUserInter->property("NoPasswdLogin"));
}
QString UserDBusProxy::passwordHint()
{
    return qvariant_cast<QString>(m_dBusAccountsUserInter->property("PasswordHint"));
}
int UserDBusProxy::passwordLastChange()
{
    return qvariant_cast<int>(m_dBusAccountsUserInter->property("PasswordLastChange"));
}
QString UserDBusProxy::passwordStatus()
{
    return qvariant_cast<QString>(m_dBusAccountsUserInter->property("PasswordStatus"));
}
QString UserDBusProxy::shell()
{
    return qvariant_cast<QString>(m_dBusAccountsUserInter->property("Shell"));
}
int UserDBusProxy::shortDateFormat()
{
    return qvariant_cast<int>(m_dBusAccountsUserInter->property("ShortDateFormat"));
}
int UserDBusProxy::shortTimeFormat()
{
    return qvariant_cast<int>(m_dBusAccountsUserInter->property("ShortTimeFormat"));
}
bool UserDBusProxy::systemAccount()
{
    return qvariant_cast<bool>(m_dBusAccountsUserInter->property("SystemAccount"));
}
QString UserDBusProxy::uid()
{
    return qvariant_cast<QString>(m_dBusAccountsUserInter->property("Uid"));
}

QString UserDBusProxy::uuid()
{
    return qvariant_cast<QString>(m_dBusAccountsUserInter->property("UUID"));
}

bool UserDBusProxy::use24HourFormat()
{
    return qvariant_cast<bool>(m_dBusAccountsUserInter->property("Use24HourFormat"));
}
QString UserDBusProxy::userName()
{
    return qvariant_cast<QString>(m_dBusAccountsUserInter->property("UserName"));
}
int UserDBusProxy::weekBegins()
{
    return qvariant_cast<int>(m_dBusAccountsUserInter->property("WeekBegins"));
}
int UserDBusProxy::weekdayFormat()
{
    return qvariant_cast<int>(m_dBusAccountsUserInter->property("WeekdayFormat"));
}
QString UserDBusProxy::xSession()
{
    return qvariant_cast<QString>(m_dBusAccountsUserInter->property("XSession"));
}

void UserDBusProxy::onPropertiesChanged(const QDBusMessage &message)
{
    QVariantMap changedProps = qdbus_cast<QVariantMap>(message.arguments().at(1).value<QDBusArgument>());
    for (QVariantMap::const_iterator it = changedProps.begin(); it != changedProps.end(); ++it) {
        QMetaObject::invokeMethod(this, it.key().toLatin1() + "Changed", Qt::DirectConnection, QGenericArgument(it.value().typeName(), it.value().data()));
    }
}
