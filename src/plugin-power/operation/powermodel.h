//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef POWERMODEL_H
#define POWERMODEL_H

#include <QObject>

class PowerWorker;
class PowerModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool lidPresent READ lidPresent WRITE setLidPresent NOTIFY lidPresentChanged)
    Q_PROPERTY(bool sleepOnLidOnPowerClose READ sleepOnLidOnPowerClose WRITE setSleepOnLidOnPowerClose NOTIFY sleepOnLidOnPowerCloseChanged)
    Q_PROPERTY(bool sleepOnLidOnBatteryClose READ sleepOnLidOnBatteryClose WRITE setSleepOnLidOnBatteryClose NOTIFY sleepOnLidOnBatteryCloseChanged)
    Q_PROPERTY(bool screenBlackLock READ screenBlackLock WRITE setScreenBlackLock NOTIFY screenBlackLockChanged)
    Q_PROPERTY(bool sleepLock READ sleepLock WRITE setSleepLock NOTIFY sleepLockChanged)
    Q_PROPERTY(bool canSuspend READ canSuspend WRITE setCanSuspend NOTIFY suspendChanged)
    Q_PROPERTY(bool canHibernate READ canHibernate WRITE setCanHibernate NOTIFY canHibernateChanged)
    Q_PROPERTY(int screenBlackDelayOnPower READ screenBlackDelayOnPower WRITE setScreenBlackDelayOnPower NOTIFY screenBlackDelayChangedOnPower)
    Q_PROPERTY(int sleepDelayOnPower READ sleepDelayOnPower WRITE setSleepDelayOnPower NOTIFY sleepDelayChangedOnPower)
    Q_PROPERTY(int screenBlackDelayOnBattery READ screenBlackDelayOnBattery WRITE setScreenBlackDelayOnBattery NOTIFY screenBlackDelayChangedOnBattery)
    Q_PROPERTY(int sleepDelayOnBattery READ sleepDelayOnBattery WRITE setSleepDelayOnBattery NOTIFY sleepDelayChangedOnBattery)
    Q_PROPERTY(bool autoPowerSaveMode READ autoPowerSaveMode WRITE setAutoPowerSaveMode NOTIFY autoPowerSavingModeChanged)
    Q_PROPERTY(bool powerSaveMode READ powerSaveMode WRITE setPowerSaveMode NOTIFY powerSaveModeChanged)
    Q_PROPERTY(bool haveBettary READ haveBettary WRITE setHaveBettary NOTIFY haveBettaryChanged)
    Q_PROPERTY(int batteryLockScreenDelay READ getBatteryLockScreenDelay WRITE setBatteryLockScreenDelay NOTIFY batteryLockScreenDelayChanged)
    Q_PROPERTY(int powerLockScreenDelay READ getPowerLockScreenDelay WRITE setPowerLockScreenDelay NOTIFY powerLockScreenDelayChanged)
    Q_PROPERTY(bool powerSavingModeAutoWhenQuantifyLow READ powerSavingModeAutoWhenQuantifyLow WRITE setPowerSavingModeAutoWhenQuantifyLow NOTIFY powerSavingModeAutoWhenQuantifyLowChanged)
    Q_PROPERTY(bool powerSavingModeAuto READ powerSavingModeAuto WRITE setPowerSavingModeAuto NOTIFY powerSavingModeAutoChanged)
    Q_PROPERTY(uint powerSavingModeLowerBrightnessThreshold READ powerSavingModeLowerBrightnessThreshold WRITE setPowerSavingModeLowerBrightnessThreshold NOTIFY powerSavingModeLowerBrightnessThresholdChanged)
    Q_PROPERTY(uint powerSavingModeAutoBatteryPercentage READ powerSavingModeAutoBatteryPercentage WRITE setPowerSavingModeAutoBatteryPercentage NOTIFY powerSavingModeAutoBatteryPercentageChanged)
    Q_PROPERTY(int linePowerPressPowerBtnAction READ linePowerPressPowerBtnAction WRITE setLinePowerPressPowerBtnAction NOTIFY linePowerPressPowerBtnActionChanged)
    Q_PROPERTY(int linePowerLidClosedAction READ linePowerLidClosedAction WRITE setLinePowerLidClosedAction NOTIFY linePowerLidClosedActionChanged)
    Q_PROPERTY(int batteryPressPowerBtnAction READ batteryPressPowerBtnAction WRITE setBatteryPressPowerBtnAction NOTIFY batteryPressPowerBtnActionChanged)
    Q_PROPERTY(int batteryLidClosedAction READ batteryLidClosedAction WRITE setBatteryLidClosedAction NOTIFY batteryLidClosedActionChanged)
    Q_PROPERTY(bool lowPowerNotifyEnable READ lowPowerNotifyEnable WRITE setLowPowerNotifyEnable NOTIFY lowPowerNotifyEnableChanged)
    Q_PROPERTY(int lowPowerNotifyThreshold READ lowPowerNotifyThreshold WRITE setLowPowerNotifyThreshold NOTIFY lowPowerNotifyThresholdChanged)
    Q_PROPERTY(int lowPowerAutoSleepThreshold READ lowPowerAutoSleepThreshold WRITE setLowPowerAutoSleepThreshold NOTIFY lowPowerAutoSleepThresholdChanged)
    Q_PROPERTY(bool isSuspend READ getSuspend WRITE setSuspend NOTIFY suspendChanged)
    Q_PROPERTY(bool isHibernate READ getHibernate WRITE setHibernate NOTIFY hibernateChanged)
    Q_PROPERTY(bool isShutdown READ getShutdown WRITE setShutdown NOTIFY shutdownChanged)
    Q_PROPERTY(QString powerPlan READ getPowerPlan WRITE setPowerPlan NOTIFY powerPlanChanged)
    Q_PROPERTY(bool isHighPerformanceSupported READ isHighPerformanceSupported WRITE setHighPerformanceSupported NOTIFY highPerformaceSupportChanged)
    Q_PROPERTY(bool isBalancePerformanceSupported READ isBalancePerformanceSupported WRITE setBalancePerformanceSupported NOTIFY highPerformaceSupportChanged)
    Q_PROPERTY(bool isNoPasswdLogin READ isNoPasswdLogin WRITE setNoPasswdLogin NOTIFY noPasswdLoginChanged)
    Q_PROPERTY(int batteryCapacity READ batteryCapacity WRITE setBatteryCapacity NOTIFY batteryCapacityChanged)
    Q_PROPERTY(bool showBatteryTimeToFull READ showBatteryTimeToFull WRITE setShowBatteryTimeToFull NOTIFY showBatteryTimeToFullChanged)
    Q_PROPERTY(bool scheduledShutdownState READ scheduledShutdownState WRITE setScheduledShutdownState NOTIFY scheduledShutdownStateChanged)
    Q_PROPERTY(QString shutdownTime READ shutdownTime WRITE setShutdownTime NOTIFY shutdownTimeChanged)
    Q_PROPERTY(int shutdownRepetition READ shutdownRepetition WRITE setShutdownRepetition NOTIFY shutdownRepetitionChanged)
    Q_PROPERTY(int weekBegins READ weekBegins WRITE setWeekBegins NOTIFY weekBeginsChanged)
    Q_PROPERTY(int lowPowerAction READ lowPowerAction WRITE setLowPowerAction NOTIFY lowPowerActionChanged)
    Q_PROPERTY(QVariantList customShutdownWeekDays READ customShutdownWeekDays WRITE setCustomShutdownWeekDays NOTIFY customShutdownWeekDaysChanged)
    Q_PROPERTY(bool isVirtualEnvironment READ isVirtualEnvironment WRITE setIsVirtualEnvironment NOTIFY isVirtualEnvironmentChanged)

    Q_PROPERTY(QVariantList batteryLockDelayModel READ batteryLockDelayModel WRITE setBatteryLockDelayModel NOTIFY batteryLockDelayModelChanged)
    Q_PROPERTY(QVariantList batteryScreenBlackDelayModel READ batteryScreenBlackDelayModel WRITE setBatteryScreenBlackDelayModel NOTIFY batteryScreenBlackDelayModelChanged)
    Q_PROPERTY(QVariantList batterySleepDelayModel READ batterySleepDelayModel WRITE setBatterySleepDelayModel NOTIFY batterySleepDelayModelChanged)
    Q_PROPERTY(QVariantList linePowerLockDelayModel READ linePowerLockDelayModel WRITE setLinePowerLockDelayModel NOTIFY linePowerLockDelayModelChanged)
    Q_PROPERTY(QVariantList linePowerScreenBlackDelayModel READ linePowerScreenBlackDelayModel WRITE setLinePowerScreenBlackDelayModel NOTIFY linePowerScreenBlackDelayModelChanged)
    Q_PROPERTY(QVariantList linePowerSleepDelayModel READ linePowerSleepDelayModel WRITE setLinePowerSleepDelayModel NOTIFY linePowerSleepDelayModelChanged)
    Q_PROPERTY(QString enableScheduledShutdown READ enableScheduledShutdown WRITE setEnableScheduledShutdown NOTIFY enableScheduledShutdownChanged)

    friend class PowerWorker;

public:
    explicit PowerModel(QObject *parent = 0);

    inline bool screenBlackLock() const { return m_screenBlackLock; }
    void setScreenBlackLock(const bool lock);

    inline bool sleepLock() const { return m_sleepLock; }
    void setSleepLock(bool sleepLock);

    inline bool canSuspend() const { return m_canSuspend; }
    void setCanSuspend(bool canSuspend);

    inline bool lidPresent() const { return m_lidPresent; }
    void setLidPresent(bool lidPresent);

    inline int screenBlackDelayOnPower() const { return m_screenBlackDelayOnPower; }
    void setScreenBlackDelayOnPower(const int screenBlackDelayOnPower);

    inline int sleepDelayOnPower() const { return m_sleepDelayOnPower; }
    void setSleepDelayOnPower(const int sleepDelayOnPower);

    inline int screenBlackDelayOnBattery() const { return m_screenBlackDelayOnBattery; }
    void setScreenBlackDelayOnBattery(const int screenBlackDelayOnBattery);

    inline int sleepDelayOnBattery() const { return m_sleepDelayOnBattery; }
    void setSleepDelayOnBattery(const int sleepDelayOnBattery);

    inline bool sleepOnLidOnPowerClose() const { return m_sleepOnLidOnPowerClose; }
    void setSleepOnLidOnPowerClose(bool sleepOnLidClose);

    inline bool sleepOnLidOnBatteryClose() const { return m_sleepOnLidOnBatteryClose; }
    void setSleepOnLidOnBatteryClose(bool sleepOnLidOnBatteryClose);

    inline int getBatteryLockScreenDelay() const { return m_batteryLockScreenDelay; }
    void setBatteryLockScreenDelay(const int value);

    inline int getPowerLockScreenDelay() const { return m_powerLockScreenDelay; }
    void setPowerLockScreenDelay(const int value);

    inline bool autoPowerSaveMode() const { return m_autoPowerSaveMode; }
    void setAutoPowerSaveMode(bool autoPowerSavingMode);

    inline bool powerSaveMode() const { return m_powerSaveMode; }
    void setPowerSaveMode(bool powerSaveMode);

    inline bool haveBettary() const
    {
        return m_haveBettary;
    }
    void setHaveBettary(bool haveBettary);

    bool getDoubleCompare(const double value1, const double value2);

    //--------------sp2 add---------------------------
    inline bool powerSavingModeAutoWhenQuantifyLow() const { return m_bPowerSavingModeAutoWhenQuantifyLow; }
    void setPowerSavingModeAutoWhenQuantifyLow(bool bLowBatteryAutoIntoSaveEnergyMode);

    inline bool powerSavingModeAuto() const { return m_bPowerSavingModeAuto; }
    void setPowerSavingModeAuto(bool bAutoIntoSaveEnergyMode);

    inline int powerSavingModeLowerBrightnessThreshold() const { return m_dPowerSavingModeLowerBrightnessThreshold; }
    void setPowerSavingModeLowerBrightnessThreshold(uint dPowerSavingModeLowerBrightnessThreshold);

    inline int powerSavingModeAutoBatteryPercentage() const { return m_dPowerSavingModeAutoBatteryPercentage; }
    void setPowerSavingModeAutoBatteryPercentage(uint dPowerSavingModeAutoBatteryPercentage);

    inline int linePowerPressPowerBtnAction() const { return m_nLinePowerPressPowerBtnAction; }
    void setLinePowerPressPowerBtnAction(int nLinePowerPressPowerBtnAction);

    inline int linePowerLidClosedAction() const { return m_nLinePowerLidClosedAction; }
    void setLinePowerLidClosedAction(int nLinePowerLidClosedAction);

    inline int batteryPressPowerBtnAction() const { return m_nBatteryPressPowerBtnAction; }
    void setBatteryPressPowerBtnAction(int nBatteryPressPowerBtnAction);

    inline int batteryLidClosedAction() const { return m_nBatteryLidClosedAction; }
    void setBatteryLidClosedAction(int nBatteryLidClosedAction);

    inline bool lowPowerNotifyEnable() const { return m_bLowPowerNotifyEnable; }
    void setLowPowerNotifyEnable(bool bLowPowerNotifyEnable);

    inline int lowPowerNotifyThreshold() const { return m_dLowPowerNotifyThreshold; }
    void setLowPowerNotifyThreshold(int dLowPowerNotifyThreshold);

    inline int lowPowerAutoSleepThreshold() const { return m_dLowPowerAutoSleepThreshold; }
    void setLowPowerAutoSleepThreshold(int dLowPowerAutoSleepThreshold);

    void setLowPowerAction(int action);
    int lowPowerAction() const { return m_lowPowerAction; }

    //-----------------------------------------------
    inline bool getSuspend() const { return m_isSuspend; }
    void setSuspend(bool suspend);

    inline bool canHibernate() const { return m_canHibernate; }
    void setCanHibernate(bool value);

    inline bool getHibernate() const { return m_isHibernate; }
    void setHibernate(bool hibernate);

    inline bool getShutdown() const { return m_isShutdown; }
    void setShutdown(bool shutdown);

    inline QString getPowerPlan() const { return m_powerPlan; }
    void setPowerPlan(const QString &powerPlan);

    inline bool isHighPerformanceSupported() const { return m_isHighPerformanceSupported; }
    void setHighPerformanceSupported(bool isHighSupport);

    inline bool isBalancePerformanceSupported() const { return m_isBalancePerformanceSupported; }
    void setBalancePerformanceSupported(bool isBalancePerformanceSupported);
    // ----
    inline bool isNoPasswdLogin() const { return m_noPasswdLogin; }

    void setNoPasswdLogin(bool value);

    inline int batteryCapacity() const { return m_batteryCapacity; }
    void setBatteryCapacity(int batterCapacity);

    inline bool showBatteryTimeToFull() const { return m_showBatteryTimeToFull; }
    void setShowBatteryTimeToFull(bool showBatteryTimeToFull);

    inline bool scheduledShutdownState() const { return m_scheduledShutdownState; }
    void setScheduledShutdownState(bool value);

    inline QString shutdownTime() const { return m_shutdownTime; }
    void setShutdownTime(const QString &time);

    inline int shutdownRepetition() const { return m_shutdownRepetition; }
    void setShutdownRepetition(int repetition);
    Q_INVOKABLE void refreshShutdownRepetition() { Q_EMIT shutdownRepetitionChanged(m_shutdownRepetition); }

    inline int weekBegins() const { return m_weekBegins; }
    void setWeekBegins(int value);

    inline QVariantList customShutdownWeekDays() const { return m_customShutdownWeekDays; }
    void setCustomShutdownWeekDays(const QVariantList &value);

    inline QVariantList batteryLockDelayModel() const { return m_batteryLockDelayModel; };
    void setBatteryLockDelayModel(const QVariantList &value);

    inline QVariantList batteryScreenBlackDelayModel() const { return m_batteryScreenBlackDelayModel; };
    void setBatteryScreenBlackDelayModel(const QVariantList& value);

    inline QVariantList batterySleepDelayModel() const { return m_batterySleepDelayModel; };
    void setBatterySleepDelayModel(const QVariantList &value);

    inline QVariantList linePowerLockDelayModel() const { return m_linePowerLockDelayModel; };
    void setLinePowerLockDelayModel(const QVariantList &value);

    inline QVariantList linePowerScreenBlackDelayModel() const { return m_linePowerScreenBlackDelayModel; };
    void setLinePowerScreenBlackDelayModel(const QVariantList &value);

    inline QVariantList linePowerSleepDelayModel() const { return m_linePowerSleepDelayModel; };
    void setLinePowerSleepDelayModel(const QVariantList &value);

    inline QString enableScheduledShutdown() const { return m_enableScheduledShutdown; };
    void setEnableScheduledShutdown(const QString &value);

    inline bool isVirtualEnvironment() const { return m_isVirtualEnvironment; }
    void setIsVirtualEnvironment(bool isVirtualEnvironment);

Q_SIGNALS:
    void sleepLockChanged(const bool sleepLock);
    void canSleepChanged(const bool canSleep);
    void screenBlackLockChanged(const bool screenBlackLock);
    void lidPresentChanged(const bool lidPresent);
    void sleepOnLidOnPowerCloseChanged(const bool sleepOnLidClose);
    void sleepOnLidOnBatteryCloseChanged(const bool sleepOnLidClose);
    void screenBlackDelayChangedOnPower(const int screenBlackDelay);
    void sleepDelayChangedOnPower(const int sleepDelay);
    void screenBlackDelayChangedOnBattery(const int screenBlackDelay);
    void sleepDelayChangedOnBattery(const int sleepDelay);
    void canHibernateChanged(const bool canHibernate);
    void hibernateChanged(const bool hibernate);
    void shutdownChanged(const bool shutdown);

    void autoPowerSavingModeChanged(bool autoPowerSaveMode);
    void powerSaveModeChanged(bool powerSaveMode);

    void haveBettaryChanged(bool haveBettary);
    void batteryLockScreenDelayChanged(const int batteryLockScreenTime);
    void powerLockScreenDelayChanged(const int powerLockScreenTime);
    //------------------------sp2 add-------------------------------
    void powerSavingModeAutoWhenQuantifyLowChanged(const bool state);
    void powerSavingModeAutoChanged(const bool state);
    void powerSavingModeLowerBrightnessThresholdChanged(const uint level);
    void powerSavingModeAutoBatteryPercentageChanged(const uint level);
    //electric
    void linePowerPressPowerBtnActionChanged(const int reply);
    void linePowerLidClosedActionChanged(const int reply);
    //battery
    void batteryPressPowerBtnActionChanged(const int reply);
    void batteryLidClosedActionChanged(const int reply);
    void lowPowerNotifyEnableChanged(const bool state);
    void lowPowerNotifyThresholdChanged(const int value);
    void lowPowerAutoSleepThresholdChanged(const int value);
    //--------------------------------------------------------------
    void suspendChanged(bool suspendState);
    void powerPlanChanged(const QString &value);
    void highPerformaceSupportChanged(bool value);

    void noPasswdLoginChanged(bool value);

    void batteryCapacityChanged(double value);
    void showBatteryTimeToFullChanged(bool value);

    void scheduledShutdownStateChanged(bool value);
    void shutdownTimeChanged(const QString &time);
    void shutdownRepetitionChanged(int repetition);
    void weekBeginsChanged(int value);
    void customShutdownWeekDaysChanged(const QVariantList &value);
    void isVirtualEnvironmentChanged(bool isVirtualEnvironment);

    void batteryLockDelayModelChanged(const QVariantList &value);
    void batteryScreenBlackDelayModelChanged(const QVariantList &value);
    void batterySleepDelayModelChanged(const QVariantList &value);
    void linePowerLockDelayModelChanged(const QVariantList &value);
    void linePowerScreenBlackDelayModelChanged(const QVariantList &value);
    void linePowerSleepDelayModelChanged(const QVariantList &value);
    void lowPowerActionChanged(int action);
    void enableScheduledShutdownChanged(const QString &value);

private:
    bool m_lidPresent; //以此判断是否为笔记本
    bool m_sleepOnLidOnPowerClose;
    bool m_sleepOnLidOnBatteryClose;
    bool m_screenBlackLock;
    bool m_sleepLock;
    bool m_canSuspend;
    bool m_canHibernate;
    int m_screenBlackDelayOnPower;
    int m_sleepDelayOnPower;
    int m_screenBlackDelayOnBattery;
    int m_sleepDelayOnBattery;

    bool m_autoPowerSaveMode { false };
    bool m_powerSaveMode { false };

    bool m_haveBettary;
    int m_batteryLockScreenDelay;
    int m_powerLockScreenDelay;
    //---------------sp2 add------------------
    bool m_bPowerSavingModeAutoWhenQuantifyLow;
    bool m_bPowerSavingModeAuto;
    uint m_dPowerSavingModeLowerBrightnessThreshold;
    uint m_dPowerSavingModeAutoBatteryPercentage;
    int m_nLinePowerPressPowerBtnAction;
    int m_nLinePowerLidClosedAction;
    int m_nBatteryPressPowerBtnAction;
    int m_nBatteryLidClosedAction;
    bool m_bLowPowerNotifyEnable;
    int m_dLowPowerNotifyThreshold;
    int m_dLowPowerAutoSleepThreshold;
    //--------------------------------------
    bool m_isSuspend;
    bool m_isHibernate;
    bool m_isShutdown;

    QString m_powerPlan;
    bool m_isHighPerformanceSupported;
    bool m_isBalancePerformanceSupported;
    bool m_isVirtualEnvironment;

    // Account
    bool m_noPasswdLogin;

    double m_batteryCapacity;
    bool m_showBatteryTimeToFull;

    // scheduledShutdown
    bool m_scheduledShutdownState;
    QString m_shutdownTime;
    QVariantList m_customShutdownWeekDays;
    int m_shutdownRepetition;
    int m_weekBegins;
    int m_lowPowerAction;
    QString m_enableScheduledShutdown;

    QVariantList m_batteryLockDelayModel;
    QVariantList m_batteryScreenBlackDelayModel;
    QVariantList m_batterySleepDelayModel;
    QVariantList m_linePowerLockDelayModel;
    QVariantList m_linePowerScreenBlackDelayModel;
    QVariantList m_linePowerSleepDelayModel;
};

#endif // POWERMODEL_H
