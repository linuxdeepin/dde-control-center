//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef POWERWORKER_H
#define POWERWORKER_H

#include "powerdbusproxy.h"
#include <QObject>
#include <DConfig>
#include "powermodel.h"

class PowerWorker : public QObject
{
    Q_OBJECT

public:
    explicit PowerWorker(PowerModel *model, QObject *parent = 0);

    void active();
    void deactive();

public Q_SLOTS:
    void setScreenBlackLock(const bool lock);
    void setScheduledShutdownState(const bool state);
    void setShutdownTime(const QString &time);
    void setShutdownRepetition(int repetition);
    void setCustomShutdownWeekDays(const QString &weekdays);
    void setSleepLock(const bool lock);
    void setSleepOnLidOnPowerClosed(const bool sleep);
    void setSleepDelayOnPower(const int delay);
    void setSleepDelayOnBattery(const int delay);
    void setScreenBlackDelayOnPower(const int delay);
    void setScreenBlackDelayOnBattery(const int delay);
    void setSleepDelayToModelOnPower(const int delay);
    void setScreenBlackDelayToModelOnPower(const int delay);
    void setSleepDelayToModelOnBattery(const int delay);
    void setScreenBlackDelayToModelOnBattery(const int delay);
    void setLockScreenDelayOnBattery(const int delay);
    void setLockScreenDelayOnPower(const int delay);
    void setResponseBatteryLockScreenDelay(const int delay);
    void setResponsePowerLockScreenDelay(const int delay);
    void setHighPerformanceSupported(bool state);
    void setBalancePerformanceSupported(bool state);
    //------------sp2 add-----------------------
    void setPowerSavingModeAutoWhenQuantifyLow(bool bLowBatteryAutoIntoSaveEnergyMode);
    void setPowerSavingModeAuto(bool bAutoIntoSaveEnergyMode);
    void setPowerSavingModeLowerBrightnessThreshold(uint dPowerSavingModeLowerBrightnessThreshold);
    void setPowerSavingModeAutoBatteryPercentage(uint dPowerSavingModebatteryPentage);
    void setLinePowerPressPowerBtnAction(int nLinePowerPressPowerBtnAction);
    void setLinePowerLidClosedAction(int nLinePowerLidClosedAction);
    void setBatteryPressPowerBtnAction(int nBatteryPressPowerBtnAction);
    void setBatteryLidClosedAction(int nBatteryLidClosedAction);
    void setLowPowerNotifyEnable(bool bLowPowerNotifyEnable);
    void setLowPowerNotifyThreshold(int dLowPowerNotifyThreshold);
    void setLowPowerAutoSleepThreshold(int dLowPowerAutoSleepThreshold);
    void setLowPowerAction(int action);
    //------------------------------------------
    void setPowerPlan(const QString &powerPlan);
    void setShowBatteryTimeToFull(bool value);

    bool getCurCanSuspend();
    bool getCurCanHibernate();

    void setEnablePowerSave(const bool isEnable);

    int getMaxBacklightBrightness();

private:
    void readConfig(const QString &key, std::function<void(const QVariantList &value)> callback);
    void readConfig(const QString &key, std::function<void(const bool value)> callback);
    void readConfig(const QString &key, std::function<void(const QString &value)> callback);
    QVariantList converToDataMap(const QStringList& conf);
private slots:
    void onCustomShutdownWeekDaysChanged(const QByteArray &value);

private:
    PowerModel *m_powerModel;
    PowerDBusProxy *m_powerDBusProxy;

    Dtk::Core::DConfig *m_cfgDock;
    Dtk::Core::DConfig *m_cfgPower;
    Dtk::Core::DConfig *m_cfgTime;
};

#endif // POWERWORKER_H
