//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "powerworker.h"
#include "powermodel.h"
#include "widgets/utils.h"

#include <QProcessEnvironment>
#include <QFutureWatcher>
#include <QtConcurrent>

#define POWER_CAN_SLEEP "POWER_CAN_SLEEP"
#define POWER_CAN_HIBERNATE "POWER_CAN_HIBERNATE"

static const QStringList DCC_CONFIG_FILES {
    "/etc/deepin/dde-control-center.conf",
    "/usr/share/dde-control-center/dde-control-center.conf"
};

PowerWorker::PowerWorker(PowerModel *model, QObject *parent)
    : QObject(parent)
    , m_powerModel(model)
    , m_powerDBusProxy(new PowerDBusProxy(this))
{
    connect(m_powerDBusProxy, &PowerDBusProxy::noPasswdLoginChanged, m_powerModel, &PowerModel::setNoPasswdLogin);
    connect(m_powerDBusProxy, &PowerDBusProxy::ScreenBlackLockChanged, m_powerModel, &PowerModel::setScreenBlackLock);
    connect(m_powerDBusProxy, &PowerDBusProxy::SleepLockChanged, m_powerModel, &PowerModel::setSleepLock);
    connect(m_powerDBusProxy, &PowerDBusProxy::LidIsPresentChanged, m_powerModel, &PowerModel::setLidPresent);
    connect(m_powerDBusProxy, &PowerDBusProxy::LidClosedSleepChanged, m_powerModel, &PowerModel::setSleepOnLidOnPowerClose);
    connect(m_powerDBusProxy, &PowerDBusProxy::LinePowerScreenBlackDelayChanged, this, &PowerWorker::setScreenBlackDelayToModelOnPower);
    connect(m_powerDBusProxy, &PowerDBusProxy::LinePowerSleepDelayChanged, this, &PowerWorker::setSleepDelayToModelOnPower);
    connect(m_powerDBusProxy, &PowerDBusProxy::BatteryScreenBlackDelayChanged, this, &PowerWorker::setScreenBlackDelayToModelOnBattery);
    connect(m_powerDBusProxy, &PowerDBusProxy::BatterySleepDelayChanged, this, &PowerWorker::setSleepDelayToModelOnBattery);
    connect(m_powerDBusProxy, &PowerDBusProxy::BatteryLockDelayChanged, this, &PowerWorker::setResponseBatteryLockScreenDelay);
    connect(m_powerDBusProxy, &PowerDBusProxy::LinePowerLockDelayChanged, this, &PowerWorker::setResponsePowerLockScreenDelay);
    connect(m_powerDBusProxy, &PowerDBusProxy::IsHighPerformanceSupportedChanged, this, &PowerWorker::setHighPerformanceSupported);
    connect(m_powerDBusProxy, &PowerDBusProxy::IsPowerSaveSupportedChanged, this, &PowerWorker::setPowerSaveSupported);

    connect(m_powerDBusProxy, &PowerDBusProxy::PowerSavingModeAutoChanged, m_powerModel, &PowerModel::setAutoPowerSaveMode);
    connect(m_powerDBusProxy, &PowerDBusProxy::PowerSavingModeEnabledChanged, m_powerModel, &PowerModel::setPowerSaveMode);

    connect(m_powerDBusProxy, &PowerDBusProxy::HasBatteryChanged, m_powerModel, &PowerModel::setHaveBettary);
    connect(m_powerDBusProxy, &PowerDBusProxy::BatteryPercentageChanged, m_powerModel, &PowerModel::setBatteryPercentage);

    //--------------------sp2 add----------------------------
    connect(m_powerDBusProxy, &PowerDBusProxy::PowerSavingModeAutoWhenBatteryLowChanged, m_powerModel, &PowerModel::setPowerSavingModeAutoWhenQuantifyLow);
    connect(m_powerDBusProxy, &PowerDBusProxy::PowerSavingModeAutoChanged, m_powerModel, &PowerModel::setPowerSavingModeAuto);
    connect(m_powerDBusProxy, &PowerDBusProxy::PowerSavingModeBrightnessDropPercentChanged, m_powerModel, &PowerModel::setPowerSavingModeLowerBrightnessThreshold);
    connect(m_powerDBusProxy, &PowerDBusProxy::PowerSavingModeAutoBatteryPercentChanged, m_powerModel, &PowerModel::setPowerSavingModeAutoBatteryPercentage);
    connect(m_powerDBusProxy, &PowerDBusProxy::LinePowerPressPowerBtnActionChanged, m_powerModel, &PowerModel::setLinePowerPressPowerBtnAction);
    connect(m_powerDBusProxy, &PowerDBusProxy::LinePowerLidClosedActionChanged, m_powerModel, &PowerModel::setLinePowerLidClosedAction);
    connect(m_powerDBusProxy, &PowerDBusProxy::BatteryPressPowerBtnActionChanged, m_powerModel, &PowerModel::setBatteryPressPowerBtnAction);
    connect(m_powerDBusProxy, &PowerDBusProxy::BatteryLidClosedActionChanged, m_powerModel, &PowerModel::setBatteryLidClosedAction);
    connect(m_powerDBusProxy, &PowerDBusProxy::LowPowerNotifyEnableChanged, m_powerModel, &PowerModel::setLowPowerNotifyEnable);
    connect(m_powerDBusProxy, &PowerDBusProxy::LowPowerNotifyThresholdChanged, m_powerModel, &PowerModel::setLowPowerNotifyThreshold);
    connect(m_powerDBusProxy, &PowerDBusProxy::LowPowerAutoSleepThresholdChanged, m_powerModel, &PowerModel::setLowPowerAutoSleepThreshold);
    //-------------------------------------------------------
    connect(m_powerDBusProxy, &PowerDBusProxy::ModeChanged, m_powerModel, &PowerModel::setPowerPlan);

    // init base property
    m_powerModel->setHaveBettary(m_powerDBusProxy->hasBattery());
}

void PowerWorker::active()
{
    m_powerDBusProxy->blockSignals(false);

    // refersh data
    m_powerModel->setScreenBlackLock(m_powerDBusProxy->screenBlackLock());
    m_powerModel->setSleepLock(m_powerDBusProxy->sleepLock());
    m_powerModel->setLidPresent(m_powerDBusProxy->lidIsPresent());
    m_powerModel->setSleepOnLidOnPowerClose(m_powerDBusProxy->lidClosedSleep());
    m_powerModel->setHaveBettary(m_powerDBusProxy->hasBattery());
    m_powerModel->setPowerSavingModeAutoWhenQuantifyLow(m_powerDBusProxy->powerSavingModeAutoWhenBatteryLow());
    m_powerModel->setPowerSavingModeAutoBatteryPercentage(m_powerDBusProxy->powerSavingModeAutoBatteryPercent());
    m_powerModel->setPowerSavingModeLowerBrightnessThreshold(m_powerDBusProxy->powerSavingModeBrightnessDropPercent());
    m_powerModel->setLowPowerNotifyEnable(m_powerDBusProxy->lowPowerNotifyEnable());
    m_powerModel->setLowPowerAutoSleepThreshold(m_powerDBusProxy->lowPowerAutoSleepThreshold());
    m_powerModel->setLowPowerNotifyThreshold(m_powerDBusProxy->lowPowerNotifyThreshold());
    m_powerModel->setLinePowerPressPowerBtnAction(m_powerDBusProxy->linePowerPressPowerBtnAction());
    m_powerModel->setLinePowerLidClosedAction(m_powerDBusProxy->linePowerLidClosedAction());
    m_powerModel->setBatteryPressPowerBtnAction(m_powerDBusProxy->batteryPressPowerBtnAction());
    m_powerModel->setBatteryLidClosedAction(m_powerDBusProxy->batteryLidClosedAction());
    m_powerModel->setPowerPlan(m_powerDBusProxy->mode());

    m_powerModel->setNoPasswdLogin(m_powerDBusProxy->noPasswdLogin());

    setHighPerformanceSupported(m_powerDBusProxy->isHighPerformanceSupported());
    setBalancePerformanceSupported(m_powerDBusProxy->isBalancePerformanceSupported());
    setPowerSaveSupported(m_powerDBusProxy->isPowerSaveSupported());

    setScreenBlackDelayToModelOnPower(m_powerDBusProxy->linePowerScreenBlackDelay());
    setSleepDelayToModelOnPower(m_powerDBusProxy->linePowerSleepDelay());

    setScreenBlackDelayToModelOnBattery(m_powerDBusProxy->batteryScreenBlackDelay());
    setSleepDelayToModelOnBattery(m_powerDBusProxy->batterySleepDelay());

    setResponseBatteryLockScreenDelay(m_powerDBusProxy->batteryLockDelay());
    setResponsePowerLockScreenDelay(m_powerDBusProxy->linePowerLockDelay());

    m_powerModel->setAutoPowerSaveMode(m_powerDBusProxy->powerSavingModeAuto());
    m_powerModel->setPowerSaveMode(m_powerDBusProxy->powerSavingModeEnabled());

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    const bool confVal = valueByQSettings<bool>(DCC_CONFIG_FILES, "Power", "sleep", true);
    const bool envVal = QVariant(env.value(POWER_CAN_SLEEP)).toBool();
    const bool envVal_hibernate = QVariant(env.value(POWER_CAN_HIBERNATE)).toBool();

    QFutureWatcher<bool> *canSleepWatcher = new QFutureWatcher<bool>();
    connect(canSleepWatcher, &QFutureWatcher<bool>::finished, this, [=] {
        bool canSuspend = canSleepWatcher->result();
        bool can_suspend = env.contains(POWER_CAN_SLEEP) ? envVal : confVal && canSuspend;
        m_powerModel->setCanSuspend(can_suspend);
        canSleepWatcher->deleteLater();
    });

    QFutureWatcher<bool> *canHibernateWatcher = new QFutureWatcher<bool>();
    connect(canHibernateWatcher, &QFutureWatcher<bool>::finished, this, [=] {
        bool canHibernate = canHibernateWatcher->result();
        bool can_hibernate = env.contains(POWER_CAN_HIBERNATE)
                                     ? envVal_hibernate
                                     : canHibernate;
        m_powerModel->setCanHibernate(can_hibernate);
        canHibernateWatcher->deleteLater();
    });

    canSleepWatcher->setFuture(QtConcurrent::run([=] {
        return m_powerDBusProxy->login1ManagerCanSuspend();
    }));

    canHibernateWatcher->setFuture(QtConcurrent::run([=] {
        return m_powerDBusProxy->login1ManagerCanHibernate();
    }));
}

void PowerWorker::deactive()
{
    m_powerDBusProxy->blockSignals(true);
}

void PowerWorker::setScreenBlackLock(const bool lock)
{
    m_powerDBusProxy->setScreenBlackLock(lock);
}

void PowerWorker::setSleepLock(const bool lock)
{
    m_powerDBusProxy->setSleepLock(lock);
}

void PowerWorker::setSleepOnLidOnPowerClosed(const bool sleep)
{
    m_powerDBusProxy->setLidClosedSleep(sleep);
}

void PowerWorker::setSleepDelayOnPower(const int delay)
{
    qDebug() << "m_powerDBusProxy->setLinePowerSleepDelay: " << converToDelayDBus(delay);
    m_powerDBusProxy->setLinePowerSleepDelay(converToDelayDBus(delay));
}

void PowerWorker::setSleepDelayOnBattery(const int delay)
{
    qDebug() << "m_powerDBusProxy->setBatterySleepDelay: " << converToDelayDBus(delay);
    m_powerDBusProxy->setBatterySleepDelay(converToDelayDBus(delay));
}

void PowerWorker::setScreenBlackDelayOnPower(const int delay)
{
    qDebug() << "m_powerDBusProxy->setLinePowerScreenBlackDelay: " << converToDelayDBus(delay);
    m_powerDBusProxy->setLinePowerScreenBlackDelay(converToDelayDBus(delay));
}

void PowerWorker::setScreenBlackDelayOnBattery(const int delay)
{
    qDebug() << "m_powerDBusProxy->setBatteryScreenBlackDelay: " << converToDelayDBus(delay);
    m_powerDBusProxy->setBatteryScreenBlackDelay(converToDelayDBus(delay));
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

void PowerWorker::setResponseBatteryLockScreenDelay(const int delay)
{
    m_powerModel->setBatteryLockScreenDelay(converToDelayModel(delay));
}

void PowerWorker::setResponsePowerLockScreenDelay(const int delay)
{
    m_powerModel->setPowerLockScreenDelay(converToDelayModel(delay));
}

void PowerWorker::setHighPerformanceSupported(bool state)
{
    m_powerModel->setHighPerformanceSupported(state);
}

void PowerWorker::setBalancePerformanceSupported(bool state)
{
    m_powerModel->setBalancePerformanceSupported(state);
}

void PowerWorker::setPowerSaveSupported(bool state)
{
    m_powerModel->setPowerSaveSupported(state);
}

void PowerWorker::setPowerSavingModeAutoWhenQuantifyLow(bool bLowBatteryAutoIntoSaveEnergyMode)
{
    m_powerDBusProxy->setPowerSavingModeAutoWhenBatteryLow(bLowBatteryAutoIntoSaveEnergyMode);
}

void PowerWorker::setPowerSavingModeAuto(bool bAutoIntoSaveEnergyMode)
{
    m_powerDBusProxy->setPowerSavingModeAuto(bAutoIntoSaveEnergyMode);
}

void PowerWorker::setPowerSavingModeLowerBrightnessThreshold(uint dPowerSavingModeLowerBrightnessThreshold)
{
    m_powerDBusProxy->setPowerSavingModeBrightnessDropPercent(dPowerSavingModeLowerBrightnessThreshold);
}

void PowerWorker::setPowerSavingModeAutoBatteryPercentage(uint dPowerSavingModeBatteryPercentage)
{
    m_powerDBusProxy->setPowerSavingModeAutoBatteryPercent(dPowerSavingModeBatteryPercentage);
}

void PowerWorker::setLinePowerPressPowerBtnAction(int nLinePowerPressPowerBtnAction)
{
    m_powerDBusProxy->setLinePowerPressPowerBtnAction(nLinePowerPressPowerBtnAction);
}

void PowerWorker::setLinePowerLidClosedAction(int nLinePowerLidClosedAction)
{
    m_powerDBusProxy->setLinePowerLidClosedAction(nLinePowerLidClosedAction);
}

void PowerWorker::setBatteryPressPowerBtnAction(int nBatteryPressPowerBtnAction)
{
    m_powerDBusProxy->setBatteryPressPowerBtnAction(nBatteryPressPowerBtnAction);
}

void PowerWorker::setBatteryLidClosedAction(int nBatteryLidClosedAction)
{
    m_powerDBusProxy->setBatteryLidClosedAction(nBatteryLidClosedAction);
}

void PowerWorker::setLowPowerNotifyEnable(bool bLowPowerNotifyEnable)
{
    m_powerDBusProxy->setLowPowerNotifyEnable(bLowPowerNotifyEnable);
}

void PowerWorker::setLowPowerNotifyThreshold(int dLowPowerNotifyThreshold)
{
    m_powerDBusProxy->setLowPowerNotifyThreshold(dLowPowerNotifyThreshold);
}

void PowerWorker::setLowPowerAutoSleepThreshold(int dLowPowerAutoSleepThreshold)
{
    m_powerDBusProxy->setLowPowerAutoSleepThreshold(dLowPowerAutoSleepThreshold);
}

/**
 * @brief PowerWorker::setPowerPlan
 * @param powerPlan
 * 设置性能模式的dbus接口
 */
void PowerWorker::setPowerPlan(const QString &powerPlan)
{
    m_powerDBusProxy->SetMode(powerPlan);
}

bool PowerWorker::getCurCanSuspend()
{
    return m_powerDBusProxy->CanSuspend();
}

bool PowerWorker::getCurCanHibernate()
{
    return m_powerDBusProxy->CanHibernate();
}

void PowerWorker::setScreenBlackDelayToModelOnBattery(const int delay)
{
    m_powerModel->setScreenBlackDelayOnBattery(converToDelayModel(delay));
}

void PowerWorker::setLockScreenDelayOnBattery(const int delay)
{
    qDebug() << "m_powerDBusProxy->setBatteryLockDelay: " << converToDelayDBus(delay);
    m_powerDBusProxy->setBatteryLockDelay(converToDelayDBus(delay));
}

void PowerWorker::setLockScreenDelayOnPower(const int delay)
{
    qDebug() << "m_powerDBusProxy->setLinePowerLockDelay: " << converToDelayDBus(delay);
    m_powerDBusProxy->setLinePowerLockDelay(converToDelayDBus(delay));
}

void PowerWorker::setEnablePowerSave(const bool isEnable)
{
    m_powerDBusProxy->setPowerSavingModeEnabled(isEnable);
}

double PowerWorker::getBatteryCapacity()
{
    return m_powerDBusProxy->batteryCapacity();
}

int PowerWorker::getMaxBacklightBrightness()
{
    return m_powerDBusProxy->maxBacklightBrightness();
}

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
