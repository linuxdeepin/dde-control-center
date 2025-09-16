//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "commoninfoproxy.h"

#include <DNotifySender>

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusArgument>
#include <QDBusReply>
#include <QDBusConnectionInterface>
#include <QLoggingCategory>

namespace {
    Q_LOGGING_CATEGORY(dcCommonLog, "dcc.commoninfo")
}

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

const QString &PlyMouthScaleService = QStringLiteral("org.deepin.dde.Daemon1");
const QString &PlyMouthScalePath = QStringLiteral("/org/deepin/dde/Daemon1");
const QString &PlyMouthScaleInterface = QStringLiteral("org.deepin.dde.Daemon1");

const QString &SyncHelperService = QStringLiteral("com.deepin.sync.Helper");
const QString &SyncHelperPath = QStringLiteral("/com/deepin/sync/Helper");
const QString &SyncHelperInterface = QStringLiteral("com.deepin.sync.Helper");

static const QString &ACLHelperService = QStringLiteral("com.deepin.daemon.ACL");
static const QString &ACLHelperPath = QStringLiteral("/com/uos/usec/DeveloperMode");
static const QString &ACLHelperInterface = QStringLiteral("com.uos.usec.DeveloperMode");

// 判断是否可以使用ACL服务来处理开发者模式
static bool isACLActivatable()
{
    const bool enableACL = qEnvironmentVariableIsSet("DCC_ACL_ENABLED");
    if (enableACL)
        return true;

    if (!QDBusConnection::systemBus().interface()->isServiceRegistered(ACLHelperService))
        return false;

    QDBusInterface interface(ACLHelperService, ACLHelperPath, ACLHelperInterface, QDBusConnection::systemBus());
    return interface.isValid();
}

CommonInfoProxy::CommonInfoProxy(QObject *parent)
    : QObject(parent)
    , m_grubInter(new DDBusInterface(GrubService, GrubPath, GrubInterface, QDBusConnection::systemBus(), this))
    , m_grubThemeInter(new DDBusInterface(GrubService, GrubThemePath, GrubThemeInterface, QDBusConnection::systemBus(), this))
    , m_grubEditAuthInter(new DDBusInterface(GrubService, GrubEditAuthPath, GrubEditAuthInterface, QDBusConnection::systemBus(), this))
    , m_deepinIdInter(nullptr)
    , m_licenseInter(new DDBusInterface(LicenseService, LicensePath, LicenseInterface, QDBusConnection::systemBus(), this))
    , m_userexperienceInter(new DDBusInterface(UserexperienceService, UserexperiencePath, UserexperienceInterface, QDBusConnection::sessionBus(), this))
    , m_grubScaleInter(new DDBusInterface(PlyMouthScaleService, PlyMouthScalePath, PlyMouthScaleInterface, QDBusConnection::systemBus(), this))
    , m_syncHelperInter(nullptr)
    , m_aclInter(nullptr)
{
    // in this function, it will wait for 50 seconds finall return
    m_grubScaleInter->setTimeout(50000);

    QDBusConnection::systemBus().connect(
        GrubService,
        GrubThemePath,
        GrubThemeInterface,
        "BackgroundChanged",
        this,
        SIGNAL(BackgroundChanged())
    );

    m_isACLController =  isACLActivatable();
    if (m_isACLController) {
        qInfo(dcCommonLog) << "DeveloperMode uses ACL service to work.";
    } else {
        qInfo(dcCommonLog) << "ACL is unActivatable, DeveloperMode uses deepinId service to work.";
    }
    if (m_isACLController) {
        m_aclInter = new DDBusInterface(ACLHelperService, ACLHelperPath, ACLHelperInterface, QDBusConnection::systemBus(), this);
        if (!m_aclInter->isValid()) {
            qWarning(dcCommonLog) << "ACL is invalid, error msg:" << m_aclInter->lastError();
        }
        QDBusConnection::systemBus().connect(ACLHelperService, ACLHelperPath, ACLHelperInterface, "NotifyDeveloperMode",
                                             this,
                                             SIGNAL(DeveloperModeChanged(bool)));
        QDBusConnection::systemBus().connect(ACLHelperService, ACLHelperPath, ACLHelperInterface, "NotifyDeveloperModeError",
                                             this,
                                             SLOT(onACLError(quint32)));
    } else {
        m_syncHelperInter = new DDBusInterface(SyncHelperService, SyncHelperPath, SyncHelperInterface, QDBusConnection::systemBus(), this);
        m_deepinIdInter = new DDBusInterface(DeepinIdService, DeepinIdPath, DeepinIdInterface, QDBusConnection::sessionBus(), this);
        QDBusConnection::sessionBus().connect(DeepinIdService, DeepinIdPath, DeepinIdInterface, "Error", this, SIGNAL(onDeepinIdError(const int, const QString &)));
    }
}

bool CommonInfoProxy::IsLogin()
{
    if (m_isACLController) {
        qWarning(dcCommonLog) << "Don't need to check isLogin deepinId when using ACL service.";
        return false;
    } else {
        return qvariant_cast<bool>(m_deepinIdInter->property("IsLogin"));
    }
}

bool CommonInfoProxy::DeviceUnlocked()
{
    if (m_isACLController) {
        QDBusReply<bool> reply = m_aclInter->call("GetDeveloperModeStatus");
        if (reply.isValid()) {
            return reply.value();
        }
        qWarning(dcCommonLog) << "Failed to GetDeveloperModeStatus";
        return false;
    } else {
        return qvariant_cast<bool>(m_deepinIdInter->property("DeviceUnlocked"));
    }
}

void CommonInfoProxy::UnlockDevice()
{
    if (m_isACLController) {
        QDBusPendingCall call = m_aclInter->asyncCall("AsyncEnableDeveloperModeCompatible");
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *w) {
            if (w->isError()) {
                qWarning(dcCommonLog) << "EnableDeveloperModeCompatible DBus call failed:" << w->error().message();
                Q_EMIT developModeError("1001");
            }
            w->deleteLater();
        });
    } else {
        m_deepinIdInter->asyncCall("UnlockDevice");
    }
}

void CommonInfoProxy::Login()
{
    if (m_isACLController) {
        qWarning(dcCommonLog) << "Don't need login deepinId when using ACL service.";
    } else {
        m_deepinIdInter->asyncCall("Login");
    }
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
        } else {
            Q_EMIT EnableThemeChanged(value);
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

bool CommonInfoProxy::DeveloperMode()
{
    if (m_isACLController) {
        return DeviceUnlocked();
    } else {
        return qvariant_cast<bool>(m_syncHelperInter->property("DeveloperMode"));
    }
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

void CommonInfoProxy::Notify(const QString &inAppName, const uint replacesId, const QString &appIcon, const QString &summary, const QString &body, const QStringList &actions, const QVariantMap &hints, const int expireTimeout)
{
    Dtk::Core::DUtil::DNotifySender(summary).appName(inAppName).replaceId(replacesId).appIcon(appIcon).appBody(body).actions(actions).hints(hints).timeOut(expireTimeout).call();
}

bool CommonInfoProxy::isACLController() const
{
    return m_isACLController;
}

void CommonInfoProxy::onDeepinIdError(const int code, const QString &msg)
{
    Q_UNUSED(code);

    QString msgCode = msg;
    msgCode = msgCode.split(":").at(0);

    Q_EMIT developModeError(msgCode);
}

void CommonInfoProxy::onACLError(quint32 exitCode)
{
    const auto operatorCode = static_cast<uint16_t>(exitCode & 0xFFFF0000);
    if (operatorCode != 0x100) {
        qDebug(dcCommonLog) << "ACL error, exitCode:" << exitCode << ", operatorCode" << operatorCode;
        return;
    }
    const auto errorCode = static_cast<uint16_t>(exitCode & 0xFFFF);
    if (errorCode != 0) {
        qWarning(dcCommonLog) << "AsyncEnableDeveloperModeCompatible DBus call exitCode:"
            << exitCode << ", errorCode" << errorCode;
        Q_EMIT developModeError("1001");
    }
}
