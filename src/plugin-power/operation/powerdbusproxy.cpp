/*
* Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
*
* Author:     caixiangrong <caixiangrong@uniontech.com>
*
* Maintainer: caixiangrong <caixiangrong@uniontech.com>
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
#include "powerdbusproxy.h"

#include <QMetaObject>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingReply>

const static QString PowerService = "com.deepin.daemon.Power";
const static QString PowerPath = "/com/deepin/daemon/Power";
const static QString PowerInterface = "com.deepin.daemon.Power";

const static QString SysPowerService = "com.deepin.system.Power";
const static QString SysPowerPath = "/com/deepin/system/Power";
const static QString SysPowerInterface = "com.deepin.system.Power";

const static QString Login1ManagerService = "org.freedesktop.login1";
const static QString Login1ManagerPath = "/org/freedesktop/login1";
const static QString Login1ManagerInterface = "org.freedesktop.login1.Manager";

const static QString PowerManagerService = "com.deepin.daemon.PowerManager";
const static QString PowerManagerPath = "/com/deepin/daemon/PowerManager";
const static QString PowerManagerInterface = "com.deepin.daemon.PowerManager";

const static QString PropertiesInterface = "org.freedesktop.DBus.Properties";
const static QString PropertiesChanged = "PropertiesChanged";

PowerDBusProxy::PowerDBusProxy(QObject *parent)
    : QObject(parent)
{
    init();
}

void PowerDBusProxy::init()
{
    m_powerInter = new QDBusInterface(PowerService, PowerPath, PowerInterface, QDBusConnection::sessionBus(), this);
    m_sysPowerInter = new QDBusInterface(SysPowerService, SysPowerPath, SysPowerInterface, QDBusConnection::systemBus(), this);
    m_login1ManagerInter = new QDBusInterface(Login1ManagerService, Login1ManagerPath, Login1ManagerInterface, QDBusConnection::systemBus(), this);
    m_powerManager = new QDBusInterface(PowerManagerService, PowerManagerPath, PowerManagerInterface, QDBusConnection::systemBus(), this);

    QDBusConnection dbusConnection = m_powerInter->connection();
    dbusConnection.connect(PowerService, PowerPath, PropertiesInterface, PropertiesChanged, this, SLOT(onPropertiesChanged(QDBusMessage)));

    dbusConnection = m_sysPowerInter->connection();
    dbusConnection.connect(SysPowerService, SysPowerPath, PropertiesInterface, PropertiesChanged, this, SLOT(onPropertiesChanged(QDBusMessage)));
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
    return qvariant_cast<bool>(m_powerInter->property("$L2owPowerNotifyEnable"));
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
    QDBusInterface Interface("com.deepin.daemon.Display",
                             "/com/deepin/daemon/Display",
                             "com.deepin.daemon.Display",
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
    QList<QVariant> argumentList;
    return QDBusPendingReply<bool>(m_powerManager->asyncCallWithArgumentList(QStringLiteral("CanSuspend"), argumentList));
}

bool PowerDBusProxy::CanHibernate()
{
    QList<QVariant> argumentList;
    return QDBusPendingReply<bool>(m_powerManager->asyncCallWithArgumentList(QStringLiteral("CanHibernate"), argumentList));
}

bool PowerDBusProxy::login1ManagerCanSuspend()
{
    QList<QVariant> argumentList;
    QDBusPendingReply<QString> reply = m_login1ManagerInter->asyncCallWithArgumentList(QStringLiteral("CanSuspend"), argumentList);
    reply.waitForFinished();
    return reply.value().contains("yes");
}

bool PowerDBusProxy::login1ManagerCanHibernate()
{
    QList<QVariant> argumentList;
    QDBusPendingReply<QString> reply = m_login1ManagerInter->asyncCallWithArgumentList(QStringLiteral("CanHibernate"), argumentList);
    reply.waitForFinished();
    return reply.value().contains("yes");
}

void PowerDBusProxy::onPropertiesChanged(const QDBusMessage &message)
{
    QVariantMap changedProps = qdbus_cast<QVariantMap>(message.arguments().at(1).value<QDBusArgument>());
    for (QVariantMap::const_iterator it = changedProps.begin(); it != changedProps.end(); ++it) {
        QMetaObject::invokeMethod(this, it.key().toLatin1() + "Changed", Qt::DirectConnection, QGenericArgument(it.value().typeName(), it.value().data()));
    }
}
