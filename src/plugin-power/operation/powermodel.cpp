//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "powermodel.h"
#include "utils.h"

#include <QDebug>

const double EPSINON = 1e-6;

PowerModel::PowerModel(QObject *parent)
    : QObject(parent)
    , m_lidPresent(false)
    , m_sleepOnLidOnPowerClose(false)
    , m_sleepOnLidOnBatteryClose(false)
    , m_screenBlackLock(false)
    , m_sleepLock(false)
    , m_canSuspend(true)
    , m_canHibernate(false)
    , m_screenBlackDelayOnPower(0)
    , m_sleepDelayOnPower(0)
    , m_screenBlackDelayOnBattery(0)
    , m_sleepDelayOnBattery(0)
    , m_haveBettary(false)
    , m_batteryLockScreenDelay(0)
    , m_powerLockScreenDelay(0)
    , m_bPowerSavingModeAutoWhenQuantifyLow(0)
    , m_bPowerSavingModeAuto(false)
    , m_dPowerSavingModeLowerBrightnessThreshold(0)
    , m_dPowerSavingModeAutoBatteryPercentage(0)
    , m_nLinePowerPressPowerBtnAction(0)
    , m_nLinePowerLidClosedAction(0)
    , m_nBatteryPressPowerBtnAction(0)
    , m_nBatteryLidClosedAction(0)
    , m_bLowPowerNotifyEnable(false)
    , m_dLowPowerNotifyThreshold(0)
    , m_dLowPowerAutoSleepThreshold(0)
    , m_isSuspend(true)
    , m_isHibernate(true)
    , m_isShutdown(true)
    , m_powerPlan("")
    , m_isHighPerformanceSupported(false)
    , m_isBalancePerformanceSupported(false)
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

void PowerModel::setSleepOnLidOnBatteryClose(bool sleepOnLidOnBatteryClose)
{
    if (sleepOnLidOnBatteryClose != m_sleepOnLidOnBatteryClose) {
        m_sleepOnLidOnBatteryClose = sleepOnLidOnBatteryClose;

        Q_EMIT sleepOnLidOnBatteryCloseChanged(sleepOnLidOnBatteryClose);
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

void PowerModel::setHaveBettary(bool haveBettary)
{
    if (haveBettary == m_haveBettary)
        return;

    m_haveBettary = haveBettary;

    Q_EMIT haveBettaryChanged(haveBettary);
}

bool PowerModel::getDoubleCompare(const double value1, const double value2)
{
    return ((value1 - value2 >= -EPSINON) && (value1 - value2 <= EPSINON));
}

void PowerModel::setPowerSavingModeAutoWhenQuantifyLow(bool bLowBatteryAutoIntoSaveEnergyMode)
{
    if (bLowBatteryAutoIntoSaveEnergyMode != m_bPowerSavingModeAutoWhenQuantifyLow) {
        m_bPowerSavingModeAutoWhenQuantifyLow = bLowBatteryAutoIntoSaveEnergyMode;

        Q_EMIT powerSavingModeAutoWhenQuantifyLowChanged(bLowBatteryAutoIntoSaveEnergyMode);
    }
}

void PowerModel::setPowerSavingModeAuto(bool bAutoIntoSaveEnergyMode)
{
    if (bAutoIntoSaveEnergyMode != m_bPowerSavingModeAuto) {
        m_bPowerSavingModeAuto = bAutoIntoSaveEnergyMode;

        Q_EMIT powerSavingModeAutoChanged(bAutoIntoSaveEnergyMode);
    }
}

void PowerModel::setPowerSavingModeLowerBrightnessThreshold(uint dPowerSavingModeLowerBrightnessThreshold)
{
    if (dPowerSavingModeLowerBrightnessThreshold != m_dPowerSavingModeLowerBrightnessThreshold) {
        m_dPowerSavingModeLowerBrightnessThreshold = dPowerSavingModeLowerBrightnessThreshold;

        Q_EMIT powerSavingModeLowerBrightnessThresholdChanged(dPowerSavingModeLowerBrightnessThreshold);
    }
}

void PowerModel::setPowerSavingModeAutoBatteryPercentage(uint dPowerSavingModeAutoBatteryPercentage)
{
    if (dPowerSavingModeAutoBatteryPercentage != m_dPowerSavingModeAutoBatteryPercentage) {
        m_dPowerSavingModeAutoBatteryPercentage = dPowerSavingModeAutoBatteryPercentage;

        Q_EMIT powerSavingModeAutoBatteryPercentageChanged(dPowerSavingModeAutoBatteryPercentage);
    }
}

void PowerModel::setLinePowerPressPowerBtnAction(int nLinePowerPressPowerBtnAction)
{
    if (nLinePowerPressPowerBtnAction != m_nLinePowerPressPowerBtnAction) {
        m_nLinePowerPressPowerBtnAction = nLinePowerPressPowerBtnAction;

        Q_EMIT linePowerPressPowerBtnActionChanged(nLinePowerPressPowerBtnAction);
    }
}

void PowerModel::setLinePowerLidClosedAction(int nLinePowerLidClosedAction)
{
    if (nLinePowerLidClosedAction != m_nLinePowerLidClosedAction) {
        m_nLinePowerLidClosedAction = nLinePowerLidClosedAction;

        Q_EMIT linePowerLidClosedActionChanged(nLinePowerLidClosedAction);
    }
}

void PowerModel::setBatteryPressPowerBtnAction(int nBatteryPressPowerBtnAction)
{
    if (nBatteryPressPowerBtnAction != m_nBatteryPressPowerBtnAction) {
        m_nBatteryPressPowerBtnAction = nBatteryPressPowerBtnAction;

        Q_EMIT batteryPressPowerBtnActionChanged(nBatteryPressPowerBtnAction);
    }
}

void PowerModel::setBatteryLidClosedAction(int nBatteryLidClosedAction)
{
    if (nBatteryLidClosedAction != m_nBatteryLidClosedAction) {
        m_nBatteryLidClosedAction = nBatteryLidClosedAction;

        Q_EMIT batteryLidClosedActionChanged(nBatteryLidClosedAction);
    }
}

void PowerModel::setLowPowerNotifyEnable(bool bLowPowerNotifyEnable)
{
    if (bLowPowerNotifyEnable != m_bLowPowerNotifyEnable) {
        m_bLowPowerNotifyEnable = bLowPowerNotifyEnable;

        Q_EMIT lowPowerNotifyEnableChanged(bLowPowerNotifyEnable);
    }
}

void PowerModel::setLowPowerNotifyThreshold(int dLowPowerNotifyThreshold)
{
    if (dLowPowerNotifyThreshold != m_dLowPowerNotifyThreshold) {
        m_dLowPowerNotifyThreshold = dLowPowerNotifyThreshold;

        Q_EMIT lowPowerNotifyThresholdChanged(dLowPowerNotifyThreshold);
    }
}

void PowerModel::setLowPowerAutoSleepThreshold(int dLowPowerAutoSleepThreshold)
{
    if (dLowPowerAutoSleepThreshold != m_dLowPowerAutoSleepThreshold) {
        m_dLowPowerAutoSleepThreshold = dLowPowerAutoSleepThreshold;

        Q_EMIT lowPowerAutoSleepThresholdChanged(dLowPowerAutoSleepThreshold);
    }
}

void PowerModel::setLowPowerAction(int action)
{
    if (m_lowPowerAction != action) {
        m_lowPowerAction = action;

        Q_EMIT lowPowerActionChanged(action);
    }
}

void PowerModel::setSleepLock(bool sleepLock)
{
    if (sleepLock != m_sleepLock) {
        m_sleepLock = sleepLock;

        Q_EMIT sleepLockChanged(sleepLock);
    }
}

void PowerModel::setCanSuspend(bool canSuspend)
{
    if (canSuspend != m_canSuspend) {
        m_canSuspend = canSuspend;

        Q_EMIT canSuspendChanged(canSuspend);
    }
}

void PowerModel::setCanHibernate(bool value)
{
    if (m_canHibernate != value) {
        m_canHibernate = value;

        Q_EMIT canHibernateChanged(value);
    }
}

void PowerModel::setPowerPlan(const QString &powerPlan)
{
    if (m_powerPlan != powerPlan) {
        m_powerPlan = powerPlan;

        Q_EMIT powerPlanChanged(m_powerPlan);
    }
}

void PowerModel::setHighPerformanceSupported(bool isHighSupport)
{
    if (m_isHighPerformanceSupported == isHighSupport)
        return;
    m_isHighPerformanceSupported = isHighSupport;
    Q_EMIT highPerformaceSupportChanged(isHighSupport);
}

void PowerModel::setBalancePerformanceSupported(bool isBalancePerformanceSupported)
{
    if (m_isBalancePerformanceSupported == isBalancePerformanceSupported)
        return;
    m_isBalancePerformanceSupported = isBalancePerformanceSupported;
    Q_EMIT highPerformaceSupportChanged(isBalancePerformanceSupported);
}

void PowerModel::setSuspend(bool suspend)
{
    bool enable = !IsServerSystem && suspend;
    if (m_isSuspend != enable) {
        m_isSuspend = enable;

        Q_EMIT suspendChanged(enable);
    }
}

void PowerModel::setHibernate(bool hibernate)
{
    bool enable = !IsServerSystem && hibernate;
    if (m_isHibernate != enable) {
        m_isHibernate = enable;

        Q_EMIT hibernateChanged(enable);
    }
}

void PowerModel::setShutdown(bool shutdown)
{
    if (m_isShutdown != shutdown) {
        m_isShutdown = shutdown;

        Q_EMIT shutdownChanged(shutdown);
    }
}

void PowerModel::setNoPasswdLogin(bool value)
{
    if (value != m_noPasswdLogin) {
        m_noPasswdLogin = value;

        Q_EMIT noPasswdLoginChanged(value);
    }
}

void PowerModel::setBatteryCapacity(int value)
{
    if (m_batteryCapacity != value) {
        m_batteryCapacity = value;

        Q_EMIT batteryCapacityChanged(value);
    }
}

void PowerModel::setShowBatteryTimeToFull(bool value)
{
    if (m_showBatteryTimeToFull != value) {
        m_showBatteryTimeToFull = value;

        Q_EMIT showBatteryTimeToFullChanged(value);
    }
}

void PowerModel::setScheduledShutdownState(bool value)
{
    if (m_scheduledShutdownState != value) {
        m_scheduledShutdownState = value;

        Q_EMIT scheduledShutdownStateChanged(value);
    }
}

void PowerModel::setShutdownTime(const QString &time)
{
    if (m_shutdownTime != time) {
        m_shutdownTime = time;

        Q_EMIT shutdownTimeChanged(time);
    }
}

void PowerModel::setShutdownRepetition(int repetition)
{
    if (m_shutdownRepetition != repetition) {
        m_shutdownRepetition = repetition;

        Q_EMIT shutdownRepetitionChanged(repetition);
    }
}

void PowerModel::setWeekBegins(int value)
{
    if (m_weekBegins != value) {
        m_weekBegins = value;

        Q_EMIT weekBeginsChanged(value);
    }
}

void PowerModel::setCustomShutdownWeekDays(const QVariantList &value)
{
    if (m_customShutdownWeekDays != value) {
        m_customShutdownWeekDays = value;

        Q_EMIT customShutdownWeekDaysChanged(value);
    }
}

void PowerModel::setBatteryLockDelayModel(const QVariantList &value)
{
    if (m_batteryLockDelayModel != value) {
        m_batteryLockDelayModel = value;

        Q_EMIT batteryLockDelayModelChanged(value);
    }
}

void PowerModel::setBatteryScreenBlackDelayModel(const QVariantList &value)
{
    if (m_batteryScreenBlackDelayModel != value) {
        m_batteryScreenBlackDelayModel = value;

        Q_EMIT batteryScreenBlackDelayModelChanged(value);
    }
}

void PowerModel::setBatterySleepDelayModel(const QVariantList &value)
{
    if (m_batterySleepDelayModel != value) {
        m_batterySleepDelayModel = value;

        Q_EMIT batterySleepDelayModelChanged(value);
    }
}

void PowerModel::setLinePowerLockDelayModel(const QVariantList &value)
{
    if (m_linePowerLockDelayModel != value) {
        m_linePowerLockDelayModel = value;

        Q_EMIT linePowerLockDelayModelChanged(value);
    }
}

void PowerModel::setLinePowerScreenBlackDelayModel(const QVariantList &value)
{
    if (m_linePowerScreenBlackDelayModel != value) {
        m_linePowerScreenBlackDelayModel = value;

        Q_EMIT linePowerScreenBlackDelayModelChanged(value);
    }
}

void PowerModel::setLinePowerSleepDelayModel(const QVariantList &value)
{
    if (m_linePowerSleepDelayModel != value) {
        m_linePowerSleepDelayModel = value;

        Q_EMIT linePowerSleepDelayModelChanged(value);
    }
}

void PowerModel::setEnableScheduledShutdown(const QString &value)
{
    if (m_enableScheduledShutdown != value) {
        m_enableScheduledShutdown = value;

        Q_EMIT enableScheduledShutdownChanged(value);
    }
}

void PowerModel::setIsVirtualEnvironment(bool isVirtualEnvironment)
{
    if (m_isVirtualEnvironment != isVirtualEnvironment) {
        m_isVirtualEnvironment = isVirtualEnvironment;

        Q_EMIT isVirtualEnvironmentChanged(isVirtualEnvironment);
    }
}
