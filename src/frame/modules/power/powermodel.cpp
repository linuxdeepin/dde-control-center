/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#include "powermodel.h"

#include <QDebug>

using namespace dcc;
using namespace dcc::power;

const double EPSINON = 1e-6;

PowerModel::PowerModel(QObject *parent)
    : QObject(parent)
    , m_canSleep(true)
    , m_haveBettary(false)
    , m_batteryPercentage(0.0)
{
}

void PowerModel::setScreenBlackLock(const bool lock)
{
    if (lock != m_screenBlackLock) {
        m_screenBlackLock = lock;

        Q_EMIT screenBlackLockChanged(lock);
    }
}

void PowerModel::setLidPresent(bool lidPresent)
{
    if (lidPresent != m_lidPresent) {
        m_lidPresent = lidPresent;

        Q_EMIT lidPresentChanged(lidPresent);
    }
}

void PowerModel::setScreenBlackDelayOnPower(const int screenBlackDelayOnPower)
{
    if (screenBlackDelayOnPower != m_screenBlackDelayOnPower) {
        m_screenBlackDelayOnPower = screenBlackDelayOnPower;

        Q_EMIT screenBlackDelayChangedOnPower(screenBlackDelayOnPower);
    }
}

void PowerModel::setSleepDelayOnPower(const int sleepDelayOnPower)
{
    if (sleepDelayOnPower != m_sleepDelayOnPower) {
        m_sleepDelayOnPower = sleepDelayOnPower;

        Q_EMIT sleepDelayChangedOnPower(sleepDelayOnPower);
    }
}

void PowerModel::setScreenBlackDelayOnBattery(const int screenBlackDelayOnBattery)
{
    if (screenBlackDelayOnBattery != m_screenBlackDelayOnBattery) {
        m_screenBlackDelayOnBattery = screenBlackDelayOnBattery;

        Q_EMIT screenBlackDelayChangedOnBattery(screenBlackDelayOnBattery);
    }
}

void PowerModel::setSleepDelayOnBattery(const int sleepDelayOnBattery)
{
    if (sleepDelayOnBattery != m_sleepDelayOnBattery) {
        m_sleepDelayOnBattery = sleepDelayOnBattery;

        Q_EMIT sleepDelayChangedOnBattery(sleepDelayOnBattery);
    }
}

void PowerModel::setSleepOnLidOnPowerClose(bool sleepOnLidClose)
{
    if (sleepOnLidClose != m_sleepOnLidOnPowerClose) {
        m_sleepOnLidOnPowerClose = sleepOnLidClose;

        Q_EMIT sleepOnLidOnPowerCloseChanged(sleepOnLidClose);
    }
}

void PowerModel::setSleepOnLidOnBatteryClose(bool sleepOnLidClose)
{
    if (sleepOnLidClose != m_sleepOnLidOnBatteryClose) {
        m_sleepOnLidOnBatteryClose = sleepOnLidClose;

        Q_EMIT sleepOnLidOnBatteryCloseChanged(sleepOnLidClose);
    }
}

void PowerModel::setBatteryLockScreenDelay(const int value)
{
    if (value != m_batteryLockScreenDelay) {
        m_batteryLockScreenDelay = value;

        Q_EMIT batteryLockScreenDelayChanged(value);
    }
}

void PowerModel::setPowerLockScreenDelay(const int value)
{
    if (value != m_powerLockScreenDelay) {
        m_powerLockScreenDelay = value;

        Q_EMIT powerLockScreenDelayChanged(value);
    }
}

#ifndef DCC_DISABLE_POWERSAVE
void PowerModel::setAutoPowerSaveMode(bool autoPowerSavingMode)
{
    if (m_autoPowerSaveMode == autoPowerSavingMode)
        return;

    m_autoPowerSaveMode = autoPowerSavingMode;

    Q_EMIT autoPowerSavingModeChanged(autoPowerSavingMode);
}

void PowerModel::setPowerSaveMode(bool powerSaveMode)
{
    if (m_powerSaveMode == powerSaveMode)
        return;

    m_powerSaveMode = powerSaveMode;

    Q_EMIT powerSaveModeChanged(powerSaveMode);
}
#endif

void PowerModel::setHaveBettary(bool haveBettary)
{
    if (haveBettary == m_haveBettary)
        return;

    m_haveBettary = haveBettary;

    Q_EMIT haveBettaryChanged(haveBettary);
}

void PowerModel::setBatteryPercentage(double batteryPercentage)
{
    if (!getDoubleCompare(batteryPercentage, m_batteryPercentage))
        return;

    m_batteryPercentage = batteryPercentage;

    Q_EMIT batteryPercentageChanged(batteryPercentage);
}

bool PowerModel::getDoubleCompare(const double value1, const double value2)
{
    return ((value1 - value2 >= -EPSINON) && (value1 - value2 <= EPSINON));
}

void PowerModel::setSleepLock(bool sleepLock)
{
    if (sleepLock != m_sleepLock) {
        m_sleepLock = sleepLock;

        Q_EMIT sleepLockChanged(sleepLock);
    }
}

void PowerModel::setCanSleep(bool canSleep)
{
    if (canSleep != m_canSleep) {
        m_canSleep = canSleep;

        Q_EMIT canSleepChanged(canSleep);
    }
}

void PowerModel::setSuspend(bool suspend)
{
    if (m_isSuspend == suspend)
        return;
    m_isSuspend = suspend;
    Q_EMIT suspendChanged(suspend);
}
