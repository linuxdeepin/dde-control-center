// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "updatedbusproxy.h"

#include "widgets/dccdbusinterface.h"

#include <QDBusArgument>
#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDBusPendingReply>
#include <QDBusReply>

// HostName
const static QString HostnameService = QStringLiteral("org.freedesktop.hostname1");
const static QString HostnamePath = QStringLiteral("/org/freedesktop/hostname1");
const static QString HostnameInterface = QStringLiteral("org.freedesktop.hostname1");

// Updater
const static QString UpdaterService = QStringLiteral("org.deepin.dde.Lastore1");
const static QString UpdaterPath = QStringLiteral("/org/deepin/dde/Lastore1");
const static QString UpdaterInterface = QStringLiteral("org.deepin.dde.Lastore1.Updater");

// Manager
const static QString ManagerService = QStringLiteral("org.deepin.dde.Lastore1");
const static QString ManagerPath = QStringLiteral("/org/deepin/dde/Lastore1");
const static QString ManagerInterface = QStringLiteral("org.deepin.dde.Lastore1.Manager");

// PowerInter
const static QString PowerService = QStringLiteral("org.deepin.dde.Power1");
const static QString PowerPath = QStringLiteral("/org/deepin/dde/Power1");
const static QString PowerInterface = QStringLiteral("org.deepin.dde.Power1");

// Atomic Upgrade
const static QString AtomicUpdaterService = QStringLiteral("org.deepin.AtomicUpgrade1");
const static QString AtomicUpdaterPath = QStringLiteral("/org/deepin/AtomicUpgrade1");
const static QString AtomicUpdaterJobInterface = QStringLiteral("org.deepin.AtomicUpgrade1");

const static QString PropertiesInterface = QStringLiteral("org.freedesktop.DBus.Properties");
const static QString PropertiesChanged = QStringLiteral("PropertiesChanged");

UpdateDBusProxy::UpdateDBusProxy(QObject *parent)
    : QObject(parent)
    , m_hostname1Inter(new DDBusInterface(
              HostnameService, HostnamePath, HostnameInterface, QDBusConnection::systemBus(), this))
    , m_updateInter(new DDBusInterface(
              UpdaterService, UpdaterPath, UpdaterInterface, QDBusConnection::systemBus(), this))
    , m_managerInter(new DDBusInterface(
              ManagerService, ManagerPath, ManagerInterface, QDBusConnection::systemBus(), this))
    , m_powerInter(new DDBusInterface(
              PowerService, PowerPath, PowerInterface, QDBusConnection::sessionBus(), this))
    , m_atomicUpgradeInter(new DDBusInterface(AtomicUpdaterService,
                                              AtomicUpdaterPath,
                                              AtomicUpdaterJobInterface,
                                              QDBusConnection::systemBus(),
                                              this))
    , m_smartMirrorInter(new DDBusInterface("org.deepin.dde.Lastore1.Smartmirror",
                                            "/org/deepin/dde/Lastore1/Smartmirror",
                                              "org.deepin.dde.Lastore1.Smartmirror",
                                              QDBusConnection::systemBus(),
                                              this))

{
    qRegisterMetaType<LastoreUpdatePackagesInfo>("LastoreUpdatePackagesInfo");
    qDBusRegisterMetaType<LastoreUpdatePackagesInfo>();

    qRegisterMetaType<BatteryPercentageInfo>("BatteryPercentageInfo");
    qDBusRegisterMetaType<BatteryPercentageInfo>();
}

UpdateDBusProxy::~UpdateDBusProxy()
{
    m_hostname1Inter->deleteLater();
    m_updateInter->deleteLater();
    m_managerInter->deleteLater();
    m_powerInter->deleteLater();
    m_atomicUpgradeInter->deleteLater();
}

QString UpdateDBusProxy::staticHostname() const
{
    return qvariant_cast<QString>(m_hostname1Inter->property("StaticHostname"));
}

bool UpdateDBusProxy::updateNotify()
{
    return qvariant_cast<bool>(m_updateInter->property("UpdateNotify"));
}

void UpdateDBusProxy::SetUpdateNotify(bool in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    m_updateInter->asyncCallWithArgumentList(QStringLiteral("SetUpdateNotify"), argumentList);
}

LastoreUpdatePackagesInfo UpdateDBusProxy::classifiedUpdatablePackages()
{
    QDBusInterface updateInter(m_updateInter->service(),
                               m_updateInter->path(),
                               PropertiesInterface,
                               m_updateInter->connection());
    QDBusMessage mess = updateInter.call(QStringLiteral("Get"),
                                         m_updateInter->interface(),
                                         QStringLiteral("ClassifiedUpdatablePackages"));
    QVariant v = mess.arguments().first();
    const QDBusArgument arg = v.value<QDBusVariant>().variant().value<QDBusArgument>();
    LastoreUpdatePackagesInfo packagesInfo;
    arg >> packagesInfo;
    return packagesInfo;
}

double UpdateDBusProxy::GetCheckIntervalAndTime(QString &out1)
{
    QList<QVariant> argumentList;
    QDBusMessage reply =
            m_updateInter->callWithArgumentList(QDBus::Block,
                                                QStringLiteral("GetCheckIntervalAndTime"),
                                                argumentList);
    if (reply.type() == QDBusMessage::ReplyMessage && reply.arguments().count() == 2) {
        out1 = qdbus_cast<QString>(reply.arguments().at(1));
    }
    return qdbus_cast<double>(reply.arguments().at(0));
}

bool UpdateDBusProxy::autoDownloadUpdates()
{
    return qvariant_cast<bool>(m_updateInter->property("AutoDownloadUpdates"));
}

bool UpdateDBusProxy::autoInstallUpdates()
{
    return qvariant_cast<bool>(m_updateInter->property("AutoInstallUpdates"));
}

qulonglong UpdateDBusProxy::autoInstallUpdateType()
{
    return qvariant_cast<qulonglong>(m_updateInter->property("AutoInstallUpdateType"));
}

bool UpdateDBusProxy::autoCheckUpdates()
{
    return qvariant_cast<bool>(m_updateInter->property("AutoCheckUpdates"));
}

void UpdateDBusProxy::SetAutoCheckUpdates(bool in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    m_updateInter->asyncCallWithArgumentList(QStringLiteral("SetAutoCheckUpdates"), argumentList);
}

QString UpdateDBusProxy::mirrorSource() const
{
    return qvariant_cast<QString>(m_updateInter->property("MirrorSource"));
}

void UpdateDBusProxy::SetAutoDownloadUpdates(bool in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    m_updateInter->asyncCallWithArgumentList(QStringLiteral("SetAutoDownloadUpdates"),
                                             argumentList);
}

void UpdateDBusProxy::setAutoInstallUpdates(bool value)
{
    m_updateInter->setProperty("AutoInstallUpdates", QVariant::fromValue(value));
}

void UpdateDBusProxy::SetMirrorSource(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    m_updateInter->asyncCallWithArgumentList(QStringLiteral("SetMirrorSource"), argumentList);
}

bool UpdateDBusProxy::autoClean()
{
    return qvariant_cast<bool>(m_managerInter->property("AutoClean"));
}

uint UpdateDBusProxy::updateMode()
{
    return qvariant_cast<uint>(m_managerInter->property("UpdateMode"));
}

void UpdateDBusProxy::setUpdateMode(qulonglong value)
{
    m_managerInter->setProperty("UpdateMode", QVariant::fromValue(value));
}

QList<QDBusObjectPath> UpdateDBusProxy::jobList()
{
    return qvariant_cast<QList<QDBusObjectPath>>(m_managerInter->property("JobList"));
}

QString UpdateDBusProxy::hardwareId()
{
    return qvariant_cast<QString>(m_managerInter->property("HardwareId"));
}

QDBusPendingReply<QDBusObjectPath> UpdateDBusProxy::UpdateSource()
{
    QList<QVariant> argumentList;
    return m_managerInter->asyncCallWithArgumentList(QStringLiteral("UpdateSource"), argumentList);
}

void UpdateDBusProxy::CleanJob(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    m_managerInter->asyncCallWithArgumentList(QStringLiteral("CleanJob"), argumentList);
}

void UpdateDBusProxy::SetAutoClean(bool in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    m_managerInter->asyncCallWithArgumentList(QStringLiteral("SetAutoClean"), argumentList);
}

void UpdateDBusProxy::StartJob(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    m_managerInter->asyncCallWithArgumentList(QStringLiteral("StartJob"), argumentList);
}

void UpdateDBusProxy::PauseJob(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    m_managerInter->asyncCallWithArgumentList(QStringLiteral("PauseJob"), argumentList);
}

QDBusPendingReply<QDBusObjectPath> UpdateDBusProxy::InstallPackage(const QString &jobname,
                                                                   const QString &packages)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(jobname) << QVariant::fromValue(packages);
    return m_managerInter->asyncCallWithArgumentList(QStringLiteral("InstallPackage"),
                                                     argumentList);
}

QDBusPendingReply<QDBusObjectPath> UpdateDBusProxy::RemovePackage(const QString &jobname,
                                                                  const QString &packages)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(jobname) << QVariant::fromValue(packages);
    return m_managerInter->asyncCallWithArgumentList(QStringLiteral("RemovePackage"), argumentList);
}

QDBusPendingReply<QList<QDBusObjectPath>> UpdateDBusProxy::ClassifiedUpgrade(qulonglong in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_managerInter->asyncCallWithArgumentList(QStringLiteral("ClassifiedUpgrade"),
                                                     argumentList);
}

QDBusPendingReply<qlonglong> UpdateDBusProxy::PackagesDownloadSize(const QStringList &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_managerInter->asyncCallWithArgumentList(QStringLiteral("PackagesDownloadSize"),
                                                     argumentList);
}

QDBusPendingReply<bool> UpdateDBusProxy::PackageExists(const QString &pkgid)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(pkgid);
    return m_managerInter->asyncCallWithArgumentList(QStringLiteral("PackageExists"), argumentList);
}

bool UpdateDBusProxy::onBattery()
{
    return qvariant_cast<bool>(m_powerInter->property("OnBattery"));
}

BatteryPercentageInfo UpdateDBusProxy::batteryPercentage()
{
    QDBusInterface powerInter(m_powerInter->service(),
                              m_powerInter->path(),
                              PropertiesInterface,
                              m_powerInter->connection());
    QDBusMessage mess = powerInter.call(QStringLiteral("Get"),
                                        m_powerInter->interface(),
                                        QStringLiteral("BatteryPercentage"));
    QVariant v = mess.arguments().first();
    const QDBusArgument arg = v.value<QDBusVariant>().variant().value<QDBusArgument>();
    BatteryPercentageInfo packagesInfo;
    arg >> packagesInfo;
    return packagesInfo;
}

void UpdateDBusProxy::commit(const QString &commitDate)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(commitDate);
    m_atomicUpgradeInter->asyncCallWithArgumentList(QStringLiteral("Commit"), argumentList);
}

bool UpdateDBusProxy::atomBackupIsRunning()
{
    return qvariant_cast<bool>(m_atomicUpgradeInter->property("Running"));
}

bool UpdateDBusProxy::enable() const
{
    return qvariant_cast<bool>(m_smartMirrorInter->property("Enable"));
}

void UpdateDBusProxy::SetEnable(bool enable)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(enable);
    m_smartMirrorInter->asyncCallWithArgumentList(QStringLiteral("SetEnable"), argumentList);
}
