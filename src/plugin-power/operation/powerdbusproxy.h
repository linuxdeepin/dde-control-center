//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef POWERDBUSPROXY_H
#define POWERDBUSPROXY_H

#include <QObject>
#include <DDBusInterface>
#include <optional>
class QDBusInterface;
class QDBusMessage;
using Dtk::Core::DDBusInterface;
class PowerDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit PowerDBusProxy(QObject *parent = nullptr);

    // Power
    Q_PROPERTY(bool ScreenBlackLock READ screenBlackLock WRITE setScreenBlackLock NOTIFY ScreenBlackLockChanged)
    bool screenBlackLock();
    void setScreenBlackLock(bool value);
    Q_PROPERTY(bool SleepLock READ sleepLock WRITE setSleepLock NOTIFY SleepLockChanged)
    bool sleepLock();
    void setSleepLock(bool value);
    Q_PROPERTY(bool LidIsPresent READ lidIsPresent NOTIFY LidIsPresentChanged)
    bool lidIsPresent();
    Q_PROPERTY(bool LidClosedSleep READ lidClosedSleep WRITE setLidClosedSleep NOTIFY LidClosedSleepChanged)
    bool lidClosedSleep();
    void setLidClosedSleep(bool value);
    Q_PROPERTY(bool LowPowerNotifyEnable READ lowPowerNotifyEnable WRITE setLowPowerNotifyEnable NOTIFY LowPowerNotifyEnableChanged)
    bool lowPowerNotifyEnable();
    void setLowPowerNotifyEnable(bool value);
    Q_PROPERTY(int LowPowerAutoSleepThreshold READ lowPowerAutoSleepThreshold WRITE setLowPowerAutoSleepThreshold NOTIFY LowPowerAutoSleepThresholdChanged)
    int lowPowerAutoSleepThreshold();
    void setLowPowerAutoSleepThreshold(int value);
    Q_PROPERTY(int LowPowerNotifyThreshold READ lowPowerNotifyThreshold WRITE setLowPowerNotifyThreshold NOTIFY LowPowerNotifyThresholdChanged)
    int lowPowerNotifyThreshold();
    void setLowPowerNotifyThreshold(int value);
    Q_PROPERTY(int LinePowerPressPowerBtnAction READ linePowerPressPowerBtnAction WRITE setLinePowerPressPowerBtnAction NOTIFY LinePowerPressPowerBtnActionChanged)
    int linePowerPressPowerBtnAction();
    void setLinePowerPressPowerBtnAction(int value);
    Q_PROPERTY(int LinePowerLidClosedAction READ linePowerLidClosedAction WRITE setLinePowerLidClosedAction NOTIFY LinePowerLidClosedActionChanged)
    int linePowerLidClosedAction();
    void setLinePowerLidClosedAction(int value);
    Q_PROPERTY(int BatteryPressPowerBtnAction READ batteryPressPowerBtnAction WRITE setBatteryPressPowerBtnAction NOTIFY BatteryPressPowerBtnActionChanged)
    int batteryPressPowerBtnAction();
    void setBatteryPressPowerBtnAction(int value);
    Q_PROPERTY(int BatteryLidClosedAction READ batteryLidClosedAction WRITE setBatteryLidClosedAction NOTIFY BatteryLidClosedActionChanged)
    int batteryLidClosedAction();
    void setBatteryLidClosedAction(int value);
    Q_PROPERTY(bool IsHighPerformanceSupported READ isHighPerformanceSupported NOTIFY IsHighPerformanceSupportedChanged)
    bool isHighPerformanceSupported();
    Q_PROPERTY(bool IsBalancePerformanceSupported READ isBalancePerformanceSupported NOTIFY IsBalancePerformanceSupportedChanged)
    bool isBalancePerformanceSupported();
    Q_PROPERTY(int LinePowerScreenBlackDelay READ linePowerScreenBlackDelay WRITE setLinePowerScreenBlackDelay NOTIFY LinePowerScreenBlackDelayChanged)
    int linePowerScreenBlackDelay();
    void setLinePowerScreenBlackDelay(int value);
    Q_PROPERTY(int LinePowerSleepDelay READ linePowerSleepDelay WRITE setLinePowerSleepDelay NOTIFY LinePowerSleepDelayChanged)
    int linePowerSleepDelay();
    void setLinePowerSleepDelay(int value);
    Q_PROPERTY(int BatteryScreenBlackDelay READ batteryScreenBlackDelay WRITE setBatteryScreenBlackDelay NOTIFY BatteryScreenBlackDelayChanged)
    int batteryScreenBlackDelay();
    void setBatteryScreenBlackDelay(int value);
    Q_PROPERTY(int BatterySleepDelay READ batterySleepDelay WRITE setBatterySleepDelay NOTIFY BatterySleepDelayChanged)
    int batterySleepDelay();
    void setBatterySleepDelay(int value);
    Q_PROPERTY(int BatteryLockDelay READ batteryLockDelay WRITE setBatteryLockDelay NOTIFY BatteryLockDelayChanged)
    int batteryLockDelay();
    void setBatteryLockDelay(int value);
    Q_PROPERTY(int LinePowerLockDelay READ linePowerLockDelay WRITE setLinePowerLockDelay NOTIFY LinePowerLockDelayChanged)
    int linePowerLockDelay();
    void setLinePowerLockDelay(int value);
    // SystemPower
    Q_PROPERTY(bool HasBattery READ hasBattery NOTIFY HasBatteryChanged)
    bool hasBattery();
    Q_PROPERTY(bool PowerSavingModeAutoWhenBatteryLow READ powerSavingModeAutoWhenBatteryLow WRITE setPowerSavingModeAutoWhenBatteryLow NOTIFY PowerSavingModeAutoWhenBatteryLowChanged)
    bool powerSavingModeAutoWhenBatteryLow();
    void setPowerSavingModeAutoWhenBatteryLow(bool value);

    Q_PROPERTY(uint PowerSavingModeBrightnessDropPercent READ powerSavingModeBrightnessDropPercent WRITE setPowerSavingModeBrightnessDropPercent NOTIFY PowerSavingModeBrightnessDropPercentChanged)
    uint powerSavingModeBrightnessDropPercent();
    void setPowerSavingModeBrightnessDropPercent(uint value);

    Q_PROPERTY(uint PowerSavingModeAutoBatteryPercent READ powerSavingModeAutoBatteryPercent WRITE setPowerSavingModeAutoBatteryPercent NOTIFY PowerSavingModeAutoBatteryPercentChanged)
    uint powerSavingModeAutoBatteryPercent();
    void setPowerSavingModeAutoBatteryPercent(uint value);

    Q_PROPERTY(QString Mode READ mode NOTIFY ModeChanged)
    QString mode();
    Q_PROPERTY(bool PowerSavingModeAuto READ powerSavingModeAuto WRITE setPowerSavingModeAuto NOTIFY PowerSavingModeAutoChanged)
    bool powerSavingModeAuto();
    void setPowerSavingModeAuto(bool value);
    Q_PROPERTY(bool PowerSavingModeEnabled READ powerSavingModeEnabled WRITE setPowerSavingModeEnabled NOTIFY PowerSavingModeEnabledChanged)
    bool powerSavingModeEnabled();
    void setPowerSavingModeEnabled(bool value);
    Q_PROPERTY(double BatteryCapacity READ batteryCapacity NOTIFY BatteryCapacityChanged)
    double batteryCapacity();
    Q_PROPERTY(int MaxBacklightBrightness READ maxBacklightBrightness)
    int maxBacklightBrightness();

    // USER
    Q_PROPERTY(bool NoPasswdLogin READ noPasswdLogin NOTIFY noPasswdLoginChanged)
    bool noPasswdLogin();

    Q_PROPERTY(bool ScheduledShutdownState READ scheduledShutdownState WRITE setScheduledShutdownState NOTIFY ScheduledShutdownStateChanged)
    void setScheduledShutdownState(bool value);
    bool scheduledShutdownState();

    Q_PROPERTY(QString ShutdownTime READ shutdownTime WRITE setShutdownTime NOTIFY ShutdownTimeChanged)
    void setShutdownTime(const QString &time);
    QString shutdownTime();

    Q_PROPERTY(int ShutdownRepetition READ shutdownRepetition WRITE setShutdownRepetition NOTIFY ShutdownRepetitionChanged)
    void setShutdownRepetition(int repetition);
    int shutdownRepetition();

    Q_PROPERTY(QByteArray CustomShutdownWeekDays READ customShutdownWeekDays WRITE setCustomShutdownWeekDays NOTIFY CustomShutdownWeekDaysChanged)
    void setCustomShutdownWeekDays(const QByteArray &weekdays);
    QByteArray customShutdownWeekDays();

    Q_PROPERTY(int LowPowerAction READ lowPowerAction WRITE setLowPowerAction NOTIFY LowPowerActionChanged)
    void setLowPowerAction(int action);
    int lowPowerAction();

    std::optional<QString> findUserById();

signals:
    // Power
    void ScreenBlackLockChanged(bool value) const;
    void SleepLockChanged(bool value) const;
    void LidIsPresentChanged(bool value) const;
    void LidClosedSleepChanged(bool value) const;
    void LinePowerScreenBlackDelayChanged(int value) const;
    void LinePowerSleepDelayChanged(int value) const;
    void BatteryScreenBlackDelayChanged(int value) const;
    void BatterySleepDelayChanged(int value) const;
    void BatteryLockDelayChanged(int value) const;
    void LinePowerLockDelayChanged(int value) const;
    void IsHighPerformanceSupportedChanged(bool value) const;
    void IsBalancePerformanceSupportedChanged(bool value) const;
    void LinePowerPressPowerBtnActionChanged(int value) const;
    void LinePowerLidClosedActionChanged(int value) const;
    void BatteryPressPowerBtnActionChanged(int value) const;
    void BatteryLidClosedActionChanged(int value) const;
    void LowPowerNotifyEnableChanged(bool value) const;
    void LowPowerNotifyThresholdChanged(int value) const;
    void LowPowerAutoSleepThresholdChanged(int value) const;
    void ShutdownTimeChanged(const QString &time);
    // SystemPower
    void PowerSavingModeAutoChanged(bool value) const;
    void PowerSavingModeEnabledChanged(bool value) const;
    void HasBatteryChanged(bool value) const;
    void BatteryPercentageChanged(double value) const;
    void PowerSavingModeAutoWhenBatteryLowChanged(bool value) const;
    void PowerSavingModeBrightnessDropPercentChanged(uint value) const;
    void PowerSavingModeAutoBatteryPercentChanged(uint value) const;
    void ModeChanged(const QString &value) const;
    void BatteryCapacityChanged(double value) const;
    void noPasswdLoginChanged(bool value);
    void ScheduledShutdownStateChanged(bool value);
    void ShutdownRepetitionChanged(int repetition);
    void CustomShutdownWeekDaysChanged(const QByteArray &value);
    void LowPowerActionChanged(int action);

public slots:
    // SystemPower
    void SetMode(const QString &mode);
    // PowerManager
    bool CanSuspend();
    bool CanHibernate();
    // login1Manager
    bool login1ManagerCanSuspend();
    bool login1ManagerCanHibernate();

private:
    DDBusInterface *m_accountRootInter;
    DDBusInterface *m_currentAccountInter;
    DDBusInterface *m_powerInter;
    DDBusInterface *m_sysPowerInter;
    DDBusInterface *m_login1ManagerInter;
    DDBusInterface *m_upowerInter;
};

#endif // POWERDBUSPROXY_H
