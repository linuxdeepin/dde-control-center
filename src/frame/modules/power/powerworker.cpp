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

#include "powerworker.h"
#include "powermodel.h"

using namespace dcc;
using namespace dcc::power;

PowerWorker::PowerWorker(PowerModel *model, QObject *parent)
    : QObject(parent)
    , m_powerModel(model)
    , m_powerInter(new PowerInter("com.deepin.daemon.Power", "/com/deepin/daemon/Power", QDBusConnection::sessionBus(), this))
    , m_sysPowerInter(new SysPowerInter("com.deepin.system.Power", "/com/deepin/system/Power", QDBusConnection::systemBus(), this))
{
    m_powerInter->setSync(false);

    connect(m_powerInter, &PowerInter::ScreenBlackLockChanged, m_powerModel, &PowerModel::setScreenBlackLock);
    connect(m_powerInter, &PowerInter::SleepLockChanged, m_powerModel, &PowerModel::setSleepLock);
    connect(m_powerInter, &PowerInter::LidIsPresentChanged, m_powerModel, &PowerModel::setLidPresent);
    connect(m_powerInter, &PowerInter::LidClosedSleepChanged, m_powerModel, &PowerModel::setSleepOnLidClose);
    connect(m_powerInter, &PowerInter::LinePowerScreenBlackDelayChanged, this, &PowerWorker::setScreenBlackDelayToModelOnPower);
    connect(m_powerInter, &PowerInter::LinePowerSleepDelayChanged, this, &PowerWorker::setSleepDelayToModelOnPower);
    connect(m_powerInter, &PowerInter::BatteryScreenBlackDelayChanged, this, &PowerWorker::setScreenBlackDelayToModelOnBattery);
    connect(m_powerInter, &PowerInter::BatterySleepDelayChanged, this, &PowerWorker::setSleepDelayToModelOnBattery);
#ifndef DCC_DISABLE_POWERSAVE
    connect(m_sysPowerInter, &SysPowerInter::PowerSavingModeAutoChanged, m_powerModel, &PowerModel::setAutoPowerSaveMode);
    connect(m_sysPowerInter, &SysPowerInter::PowerSavingModeEnabledChanged, m_powerModel, &PowerModel::setPowerSaveMode);
#endif
    connect(m_sysPowerInter, &SysPowerInter::HasBatteryChanged, m_powerModel, &PowerModel::setHaveBettary);
}

void PowerWorker::active()
{
    m_powerInter->blockSignals(false);

    // refersh data
    m_powerModel->setScreenBlackLock(m_powerInter->screenBlackLock());
    m_powerModel->setSleepLock(m_powerInter->sleepLock());
    m_powerModel->setLidPresent(m_powerInter->lidIsPresent());
    m_powerModel->setSleepOnLidClose(m_powerInter->lidClosedSleep());
    m_powerModel->setHaveBettary(m_sysPowerInter->hasBattery());

    setScreenBlackDelayToModelOnPower(m_powerInter->linePowerScreenBlackDelay());
    setSleepDelayToModelOnPower(m_powerInter->linePowerSleepDelay());

    setScreenBlackDelayToModelOnBattery(m_powerInter->batteryScreenBlackDelay());
    setSleepDelayToModelOnBattery(m_powerInter->batterySleepDelay());

#ifndef DCC_DISABLE_POWERSAVE
    m_powerModel->setAutoPowerSaveMode(m_sysPowerInter->powerSavingModeAuto());
    m_powerModel->setPowerSaveMode(m_sysPowerInter->powerSavingModeEnabled());
#endif
}

void PowerWorker::deactive()
{
    m_powerInter->blockSignals(true);
}

void PowerWorker::setScreenBlackLock(const bool lock)
{
    m_powerInter->setScreenBlackLock(lock);
}

void PowerWorker::setSleepLock(const bool lock)
{
    m_powerInter->setSleepLock(lock);
}

void PowerWorker::setSleepOnLidClosed(const bool sleep)
{
    m_powerInter->setLidClosedSleep(sleep);
}

void PowerWorker::setSleepDelayOnPower(const int delay)
{
    m_powerInter->setLinePowerSleepDelay(converToDelayDBus(delay));
}

void PowerWorker::setSleepDelayOnBattery(const int delay)
{
    m_powerInter->setBatterySleepDelay(converToDelayDBus(delay));
}

void PowerWorker::setScreenBlackDelayOnPower(const int delay)
{
    m_powerInter->setLinePowerScreenBlackDelay(converToDelayDBus(delay));
}

void PowerWorker::setScreenBlackDelayOnBattery(const int delay)
{
    m_powerInter->setBatteryScreenBlackDelay(converToDelayDBus(delay));
}

void PowerWorker::setSleepDelayToModelOnPower(const int delay)
{
    m_powerModel->setSleepDelayOnPower(converToDelayModel(delay));
}

void PowerWorker::setScreenBlackDelayToModelOnPower(const int delay)
{
    m_powerModel->setScreenBlackDelayOnPower(converToDelayModel(delay));
}

void PowerWorker::setSleepDelayToModelOnBattery(const int delay)
{
    m_powerModel->setSleepDelayOnBattery(converToDelayModel(delay));
}

void PowerWorker::setScreenBlackDelayToModelOnBattery(const int delay)
{
    m_powerModel->setScreenBlackDelayOnBattery(converToDelayModel(delay));
}

#ifndef DCC_DISABLE_POWERSAVE
void PowerWorker::setEnablePowerSave(const bool isEnable)
{
    m_sysPowerInter->setPowerSavingModeEnabled(isEnable);
}

void PowerWorker::setAutoEnablePowerSave(const bool isEnable)
{
    m_sysPowerInter->setPowerSavingModeAuto(isEnable);
}
#endif

int PowerWorker::converToDelayModel(int value)
{
    if (value == 0) {
        return 7;
    }

    if (value <= 60) {
        return 1;
    } else if (value <= 300) {
        return 2;
    } else if (value <= 600) {
        return 3;
    } else if (value <= 900) {
        return 4;
    } else if (value <= 1800) {
        return 5;
    } else {
        return 6;
    }
}

int PowerWorker::converToDelayDBus(int value)
{
    switch (value) {
    case 1:
        return 60;
    case 2:
        return 300;
    case 3:
        return 600;
    case 4:
        return 900;
    case 5:
        return 1800;
    case 6:
        return 3600;
    case 7:
        return 0;
    default:
        return 900;
    }
}
