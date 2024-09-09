//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "powerdbusproxy.h"

#include <QMetaObject>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QDBusReply>
#include <QFile>
#include <QVariant>

#include <unistd.h>

const QString PowerService = QStringLiteral("org.deepin.dde.Power1");
const QString PowerPath = QStringLiteral("/org/deepin/dde/Power1");
const QString PowerInterface = QStringLiteral("org.deepin.dde.Power1");

const QString SysPowerService = QStringLiteral("org.deepin.dde.Power1");
const QString SysPowerPath = QStringLiteral("/org/deepin/dde/Power1");
const QString SysPowerInterface = QStringLiteral("org.deepin.dde.Power1");

const QString Login1ManagerService = QStringLiteral("org.freedesktop.login1");
const QString Login1ManagerPath = QStringLiteral("/org/freedesktop/login1");
const QString Login1ManagerInterface = QStringLiteral("org.freedesktop.login1.Manager");

const QString UPowerService = QStringLiteral("org.freedesktop.UPower");
const QString UPowerPath = QStringLiteral("/org/freedesktop/UPower");
const QString UPowerInterface = QStringLiteral("org.freedesktop.UPower");

const QString accountsService = QStringLiteral("org.deepin.dde.Accounts1");
const QString defaultAccountsPath = QStringLiteral("/org/deepin/dde/Accounts1");
const QString accountsInterface = QStringLiteral("org.deepin.dde.Accounts1");

const QString accountsUserInterface = QStringLiteral("org.deepin.dde.Accounts1.User");

const QString PropertiesInterface = QStringLiteral("org.freedesktop.DBus.Properties");
const QString PropertiesChanged = QStringLiteral("PropertiesChanged");

PowerDBusProxy::PowerDBusProxy(QObject *parent)
    : QObject(parent)
    , m_accountRootInter(new DDBusInterface(accountsService, defaultAccountsPath, accountsInterface, QDBusConnection::systemBus(), this))
    , m_currentAccountInter(nullptr)
    , m_powerInter(new DDBusInterface(PowerService, PowerPath, PowerInterface, QDBusConnection::sessionBus(), this))
    , m_sysPowerInter(new DDBusInterface(SysPowerService, SysPowerPath, SysPowerInterface, QDBusConnection::systemBus(), this))
    , m_login1ManagerInter(new DDBusInterface(Login1ManagerService, Login1ManagerPath, Login1ManagerInterface, QDBusConnection::systemBus(), this))
    , m_upowerInter(new DDBusInterface(UPowerService, UPowerPath, UPowerInterface, QDBusConnection::systemBus(), this))

{
}

std::optional<QString> PowerDBusProxy::findUserById()
{
    int id = getuid();
    QDBusReply<QString> reply = m_accountRootInter->callWithArgumentList(QDBus::CallMode::Block, "FindUserById", {QString::number(id)});
    if (reply.isValid()) {
        return reply.value();
    }
    return std::nullopt;
}

bool PowerDBusProxy::noPasswdLogin()
{
    if (!m_currentAccountInter) {
        auto path = findUserById();
        if (!path.has_value()) {
            return false;
        }
        m_currentAccountInter = new DDBusInterface(accountsInterface, path.value(), accountsUserInterface, QDBusConnection::systemBus(), this);
    }
    return qvariant_cast<bool>(m_currentAccountInter->property("NoPasswdLogin"));
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

bool PowerDBusProxy::lidIsPresent()
{
    return qvariant_cast<bool>(m_powerInter->property("LidIsPresent"));
}

bool PowerDBusProxy::lidClosedSleep()
{
    return qvariant_cast<bool>(m_powerInter->property("LidClosedSleep"));
}

void PowerDBusProxy::setLidClosedSleep(bool value)
{
    m_powerInter->setProperty("LidClosedSleep", QVariant::fromValue(value));
}

bool PowerDBusProxy::lowPowerNotifyEnable()
{
    return qvariant_cast<bool>(m_powerInter->property("LowPowerNotifyEnable"));
}

void PowerDBusProxy::setLowPowerNotifyEnable(bool value)
{
    m_powerInter->setProperty("LowPowerNotifyEnable", QVariant::fromValue(value));
}

int PowerDBusProxy::lowPowerAutoSleepThreshold()
{
    return qvariant_cast<int>(m_powerInter->property("LowPowerAutoSleepThreshold"));
}

void PowerDBusProxy::setLowPowerAutoSleepThreshold(int value)
{
    m_powerInter->setProperty("LowPowerAutoSleepThreshold", QVariant::fromValue(value));
}

int PowerDBusProxy::lowPowerNotifyThreshold()
{
    return qvariant_cast<int>(m_powerInter->property("LowPowerNotifyThreshold"));
}

void PowerDBusProxy::setLowPowerNotifyThreshold(int value)
{
    m_powerInter->setProperty("LowPowerNotifyThreshold", QVariant::fromValue(value));
}

int PowerDBusProxy::linePowerPressPowerBtnAction()
{
    return qvariant_cast<int>(m_powerInter->property("LinePowerPressPowerBtnAction"));
}

void PowerDBusProxy::setLinePowerPressPowerBtnAction(int value)
{
    m_powerInter->setProperty("LinePowerPressPowerBtnAction", QVariant::fromValue(value));
}

int PowerDBusProxy::linePowerLidClosedAction()
{
    return qvariant_cast<int>(m_powerInter->property("LinePowerLidClosedAction"));
}

void PowerDBusProxy::setLinePowerLidClosedAction(int value)
{
    m_powerInter->setProperty("LinePowerLidClosedAction", QVariant::fromValue(value));
}

int PowerDBusProxy::batteryPressPowerBtnAction()
{
    return qvariant_cast<int>(m_powerInter->property("BatteryPressPowerBtnAction"));
}

void PowerDBusProxy::setBatteryPressPowerBtnAction(int value)
{
    m_powerInter->setProperty("BatteryPressPowerBtnAction", QVariant::fromValue(value));
}

int PowerDBusProxy::batteryLidClosedAction()
{
    return qvariant_cast<int>(m_powerInter->property("BatteryLidClosedAction"));
}

void PowerDBusProxy::setBatteryLidClosedAction(int value)
{
    m_powerInter->setProperty("BatteryLidClosedAction", QVariant::fromValue(value));
}

bool PowerDBusProxy::isHighPerformanceSupported()
{
    return qvariant_cast<bool>(m_powerInter->property("IsHighPerformanceSupported"));
}

bool PowerDBusProxy::isBalancePerformanceSupported()
{
    return qvariant_cast<bool>(m_sysPowerInter->property("IsBalancePerformanceSupported"));
}

bool PowerDBusProxy::isPowerSaveSupported()
{
    return qvariant_cast<bool>(m_sysPowerInter->property("IsPowerSaveSupported"));
}

int PowerDBusProxy::linePowerScreenBlackDelay()
{
    return qvariant_cast<int>(m_powerInter->property("LinePowerScreenBlackDelay"));
}

void PowerDBusProxy::setLinePowerScreenBlackDelay(int value)
{
    m_powerInter->setProperty("LinePowerScreenBlackDelay", QVariant::fromValue(value));
}

int PowerDBusProxy::linePowerSleepDelay()
{
    return qvariant_cast<int>(m_powerInter->property("LinePowerSleepDelay"));
}

void PowerDBusProxy::setLinePowerSleepDelay(int value)
{
    m_powerInter->setProperty("LinePowerSleepDelay", QVariant::fromValue(value));
}

int PowerDBusProxy::batteryScreenBlackDelay()
{
    return qvariant_cast<int>(m_powerInter->property("BatteryScreenBlackDelay"));
}

void PowerDBusProxy::setBatteryScreenBlackDelay(int value)
{
    m_powerInter->setProperty("BatteryScreenBlackDelay", QVariant::fromValue(value));
}

int PowerDBusProxy::batterySleepDelay()
{
    return qvariant_cast<int>(m_powerInter->property("BatterySleepDelay"));
}

void PowerDBusProxy::setBatterySleepDelay(int value)
{
    m_powerInter->setProperty("BatterySleepDelay", QVariant::fromValue(value));
}

int PowerDBusProxy::batteryLockDelay()
{
    return qvariant_cast<int>(m_powerInter->property("BatteryLockDelay"));
}

void PowerDBusProxy::setBatteryLockDelay(int value)
{
    m_powerInter->setProperty("BatteryLockDelay", QVariant::fromValue(value));
}

int PowerDBusProxy::linePowerLockDelay()
{
    return qvariant_cast<int>(m_powerInter->property("LinePowerLockDelay"));
}

void PowerDBusProxy::setLinePowerLockDelay(int value)
{
    m_powerInter->setProperty("LinePowerLockDelay", QVariant::fromValue(value));
}
// sysPower
bool PowerDBusProxy::hasBattery()
{
    return qvariant_cast<bool>(m_sysPowerInter->property("HasBattery"));
}

bool PowerDBusProxy::powerSavingModeAutoWhenBatteryLow()
{
    return qvariant_cast<bool>(m_sysPowerInter->property("PowerSavingModeAutoWhenBatteryLow"));
}

void PowerDBusProxy::setPowerSavingModeAutoWhenBatteryLow(bool value)
{
    m_sysPowerInter->setProperty("PowerSavingModeAutoWhenBatteryLow", QVariant::fromValue(value));
}

uint PowerDBusProxy::powerSavingModeBrightnessDropPercent()
{
    return qvariant_cast<uint>(m_sysPowerInter->property("PowerSavingModeBrightnessDropPercent"));
}

void PowerDBusProxy::setPowerSavingModeBrightnessDropPercent(uint value)
{
    m_sysPowerInter->setProperty("PowerSavingModeBrightnessDropPercent", QVariant::fromValue(value));
}

uint PowerDBusProxy::powerSavingModeAutoBatteryPercent()
{
    return qvariant_cast<uint>(m_sysPowerInter->property("PowerSavingModeAutoBatteryPercent"));
}

void PowerDBusProxy::setPowerSavingModeAutoBatteryPercent(uint value)
{
    m_sysPowerInter->setProperty("PowerSavingModeAutoBatteryPercent", QVariant::fromValue(value));
}

QString PowerDBusProxy::mode()
{
    return qvariant_cast<QString>(m_sysPowerInter->property("Mode"));
}

bool PowerDBusProxy::powerSavingModeAuto()
{
    return qvariant_cast<bool>(m_sysPowerInter->property("PowerSavingModeAuto"));
}

void PowerDBusProxy::setPowerSavingModeAuto(bool value)
{
    m_sysPowerInter->setProperty("PowerSavingModeAuto", QVariant::fromValue(value));
}

bool PowerDBusProxy::powerSavingModeEnabled()
{
    return qvariant_cast<bool>(m_sysPowerInter->property("PowerSavingModeEnabled"));
}

void PowerDBusProxy::setPowerSavingModeEnabled(bool value)
{
    m_sysPowerInter->setProperty("PowerSavingModeEnabled", QVariant::fromValue(value));
}

double PowerDBusProxy::batteryCapacity()
{
    return qvariant_cast<double>(m_sysPowerInter->property("BatteryCapacity"));
}

int PowerDBusProxy::maxBacklightBrightness()
{
    QDBusInterface Interface("org.deepin.dde.Display1",
                             "/org/deepin/dde/Display1",
                             "org.deepin.dde.Display1",
                             QDBusConnection::sessionBus());
    return Interface.property("MaxBacklightBrightness").toInt();
}

void PowerDBusProxy::SetMode(const QString &mode)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(mode);
    m_sysPowerInter->asyncCallWithArgumentList(QStringLiteral("SetMode"), argumentList);
}

bool PowerDBusProxy::CanSuspend()
{
    if (!QFile("/sys/power/mem_sleep").exists())
        return false;

    return login1ManagerCanSuspend();
}

bool PowerDBusProxy::CanHibernate()
{
    return login1ManagerCanHibernate();
}

bool PowerDBusProxy::login1ManagerCanSuspend()
{
    /* return true; */
    QList<QVariant> argumentList;
    QDBusPendingReply<QString> reply = m_login1ManagerInter->callWithArgumentList(QDBus::BlockWithGui, QStringLiteral("CanSuspend"), argumentList);
    return reply.value().contains("yes");
}

bool PowerDBusProxy::login1ManagerCanHibernate()
{
    /* return true; */
    QList<QVariant> argumentList;
    QDBusPendingReply<QString> reply = m_login1ManagerInter->callWithArgumentList(QDBus::BlockWithGui, QStringLiteral("CanHibernate"), argumentList);
    return reply.value().contains("yes");
}
