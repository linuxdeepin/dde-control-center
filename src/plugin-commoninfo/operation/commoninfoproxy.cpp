//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

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

const QString &PlyMouthScaleService = QStringLiteral("org.deepin.dde.Daemon1");
const QString &PlyMouthScalePath = QStringLiteral("/org/deepin/dde/Daemon1");
const QString &PlyMouthScaleInterface = QStringLiteral("org.deepin.dde.Daemon1");

CommonInfoProxy::CommonInfoProxy(QObject *parent)
    : QObject(parent)
    , m_grubInter(new DDBusInterface(GrubService, GrubPath, GrubInterface, QDBusConnection::systemBus(), this))
    , m_grubThemeInter(new DDBusInterface(GrubService, GrubThemePath, GrubThemeInterface, QDBusConnection::systemBus(), this))
    , m_grubEditAuthInter(new DDBusInterface(GrubService, GrubEditAuthPath, GrubEditAuthInterface, QDBusConnection::systemBus(), this))
    , m_deepinIdInter(new DDBusInterface(DeepinIdService, DeepinIdPath, DeepinIdInterface, QDBusConnection::sessionBus(), this))
    , m_licenseInter(new DDBusInterface(LicenseService, LicensePath, LicenseInterface, QDBusConnection::systemBus(), this))
    , m_userexperienceInter(new DDBusInterface(UserexperienceService, UserexperiencePath, UserexperienceInterface, QDBusConnection::sessionBus(), this))
    , m_notificationInter(new DDBusInterface(NotificationService, NotificationPath, NotificationInterface, QDBusConnection::sessionBus(), this))
    , m_grubScaleInter(new DDBusInterface(PlyMouthScaleService, PlyMouthScalePath, PlyMouthScaleInterface, QDBusConnection::systemBus(), this))
{
    // in this function, it will wait for 50 seconds finall return
    m_grubScaleInter->setTimeout(50000);
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

QDBusPendingCall CommonInfoProxy::SetScalePlymouth(int scale)
{
    return m_grubScaleInter->asyncCallWithArgumentList("ScalePlymouth", { scale });
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
