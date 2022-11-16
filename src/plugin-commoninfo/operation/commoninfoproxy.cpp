/*
* Copyright (C) 2019 ~ 2022 Deepin Technology Co., Ltd.
*
* Author:     guoyao <guoyao@uniontech.com>

* Maintainer: guoyao <guoyao@uniontech.com>

* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "commoninfoproxy.h"
#include "widgets/dccdbusinterface.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusArgument>
#include <QDBusReply>

const QString &GrubService = QStringLiteral("org.deepin.dde.Grub2");
const QString &GrubPath = QStringLiteral("/org/deepin/dde/Grub2");
const QString &GrubInterface = QStringLiteral("org.deepin.dde.Grub2");

const QString &GrubThemePath = QStringLiteral("/org/deepin/dde/Grub2/Theme");
const QString &GrubThemeInterface = QStringLiteral("org.deepin.dde.Grub2.Theme");

const QString &GrubEditAuthPath = QStringLiteral("/org/deepin/dde/Grub2/EditAuthentication");
const QString &GrubEditAuthInterface = QStringLiteral("org.deepin.dde.Grub2.EditAuthentication");

const QString &DeepinIdService = QStringLiteral("com.deepin.deepinid");
const QString &DeepinIdPath = QStringLiteral("/com/deepin/deepinid");
const QString &DeepinIdInterface = QStringLiteral("com.deepin.deepinid");

const QString &LicenseService = QStringLiteral("com.deepin.license");
const QString &LicensePath = QStringLiteral("/com/deepin/license/Info");
const QString &LicenseInterface = QStringLiteral("com.deepin.license.Info");

const QString &UserexperienceService = QStringLiteral("com.deepin.userexperience.Daemon");
const QString &UserexperiencePath = QStringLiteral("/com/deepin/userexperience/Daemon");
const QString &UserexperienceInterface = QStringLiteral("com.deepin.userexperience.Daemon");

const QString &NotificationService = QStringLiteral("org.deepin.dde.Notification1");
const QString &NotificationPath = QStringLiteral("/org/deepin/dde/Notification1");
const QString &NotificationInterface = QStringLiteral("org.deepin.dde.Notification");

const QString &PropertiesInterface = QStringLiteral("org.freedesktop.DBus.Properties");
const QString &PropertiesChanged = QStringLiteral("PropertiesChanged");

CommonInfoProxy::CommonInfoProxy(QObject *parent)
    : QObject(parent)
    , m_grubInter(new DCC_NAMESPACE::DCCDBusInterface(GrubService, GrubPath, GrubInterface, QDBusConnection::systemBus(), this))
    , m_grubThemeInter(new DCC_NAMESPACE::DCCDBusInterface(GrubService, GrubThemePath, GrubThemeInterface, QDBusConnection::systemBus(), this))
    , m_grubEditAuthInter(new DCC_NAMESPACE::DCCDBusInterface(GrubService, GrubEditAuthPath, GrubEditAuthInterface, QDBusConnection::systemBus(), this))
    , m_deepinIdInter(new DCC_NAMESPACE::DCCDBusInterface(DeepinIdService, DeepinIdPath, DeepinIdInterface, QDBusConnection::sessionBus(), this))
    , m_licenseInter(new DCC_NAMESPACE::DCCDBusInterface(LicenseService, LicensePath, LicenseInterface, QDBusConnection::systemBus(), this))
    , m_userexperienceInter(new DCC_NAMESPACE::DCCDBusInterface(UserexperienceService, UserexperiencePath, UserexperienceInterface, QDBusConnection::sessionBus(), this))
    , m_notificationInter(new DCC_NAMESPACE::DCCDBusInterface(NotificationService, NotificationPath, NotificationInterface, QDBusConnection::sessionBus(), this))
{
}

bool CommonInfoProxy::IsLogin()
{
    return qvariant_cast<bool>(m_deepinIdInter->property("IsLogin"));
}

bool CommonInfoProxy::DeviceUnlocked()
{
    return qvariant_cast<bool>(m_deepinIdInter->property("DeviceUnlocked"));
}

void CommonInfoProxy::UnlockDevice()
{
    m_deepinIdInter->asyncCall("UnlockDevice");
}

void CommonInfoProxy::Login()
{
    m_deepinIdInter->asyncCall("Login");
}

QStringList CommonInfoProxy::GetSimpleEntryTitles()
{
    QDBusReply<QStringList> reply = m_grubInter->call(QStringLiteral("GetSimpleEntryTitles"));
    if (reply.isValid())
        return reply.value();
    return {};
}

bool CommonInfoProxy::EnableTheme()
{
    return qvariant_cast<bool>(m_grubInter->property("EnableTheme"));
}

void CommonInfoProxy::setEnableTheme(const bool value)
{
    QDBusPendingCall call = m_grubInter->asyncCallWithArgumentList("SetEnableTheme", { value });
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (call.isError()) {
            Q_EMIT resetEnableTheme();
        }
        watcher->deleteLater();
    });
}

bool CommonInfoProxy::Updating()
{
    return qvariant_cast<bool>(m_grubInter->property("Updating"));
}

QString CommonInfoProxy::DefaultEntry()
{
    return qvariant_cast<QString>(m_grubInter->property("DefaultEntry"));
}

void CommonInfoProxy::setDefaultEntry(const QString &entry)
{
    m_grubInter->asyncCallWithArgumentList("SetDefaultEntry", { entry });
}

uint CommonInfoProxy::Timeout()
{
    return qvariant_cast<uint>(m_grubInter->property("Timeout"));
}

void CommonInfoProxy::setTimeout(const uint timeout)
{
    m_grubInter->asyncCallWithArgumentList("SetTimeout", { timeout });
}

QStringList CommonInfoProxy::EnabledUsers()
{
    return qvariant_cast<QStringList>(m_grubEditAuthInter->property("EnabledUsers"));
}

void CommonInfoProxy::DisableUser(const QString &username)
{
    QDBusPendingCall call = m_grubEditAuthInter->asyncCallWithArgumentList("Disable", { username });
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (call.isError()) {
            Q_EMIT resetGrubEditAuthEnabled();
        }
        watcher->deleteLater();
    });
}

void CommonInfoProxy::EnableUser(const QString &username, const QString &password)
{
    QDBusPendingCall call = m_grubEditAuthInter->asyncCallWithArgumentList("Enable", { username, password });
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (call.isError()) {
            Q_EMIT resetGrubEditAuthEnabled();
        }
        watcher->deleteLater();
    });
}

QString CommonInfoProxy::Background()
{
    QDBusReply<QString> reply = m_grubThemeInter->call(QStringLiteral("GetBackground"));
    if (reply.isValid())
        return reply.value();
    return QString();
}

void CommonInfoProxy::setBackground(const QString &name)
{
    m_grubThemeInter->asyncCallWithArgumentList("SetBackgroundSourceFile", { name });
}

int CommonInfoProxy::AuthorizationState()
{
    return qvariant_cast<int>(m_licenseInter->property("AuthorizationState"));
}

int CommonInfoProxy::LicenseState()
{
    return qvariant_cast<int>(m_licenseInter->property("LicenseState"));
}

void CommonInfoProxy::Enable(const bool value)
{
    m_userexperienceInter->asyncCallWithArgumentList("Enable", { value });
}

bool CommonInfoProxy::IsEnabled()
{
    QDBusReply<bool> reply = m_userexperienceInter->call(QStringLiteral("IsEnabled"));
    if (reply.isValid())
        return reply.value();
    return false;
}

void CommonInfoProxy::Notify(const QString &in0, const uint in1, const QString &in2, const QString &in3, const QString &in4, const QStringList &in5, const QVariantMap &in6, const int in7)
{
    m_notificationInter->asyncCall("Notify", in0, in1, in2, in3, in4, in5, in6, in7);
}
