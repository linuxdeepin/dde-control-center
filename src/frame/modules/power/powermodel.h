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

#ifndef POWERMODEL_H
#define POWERMODEL_H

#include <QObject>

namespace dcc {
namespace power {

class PowerWorker;
class PowerModel : public QObject
{
    Q_OBJECT

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

#ifndef DCC_DISABLE_POWERSAVE
    inline bool autoPowerSaveMode() const { return m_autoPowerSaveMode; }
    void setAutoPowerSaveMode(bool autoPowerSavingMode);

    inline bool powerSaveMode() const { return m_powerSaveMode; }
    void setPowerSaveMode(bool powerSaveMode);
#endif

    inline bool haveBettary() const { return m_haveBettary; }
    void setHaveBettary(bool haveBettary);
    void setBatteryPercentage(double batteryPercentage);

    bool getDoubleCompare(const double value1, const double value2);

    //--------------sp2 add---------------------------
    inline bool powerSavingModeAutoWhenQuantifyLow() const { return m_bPowerSavingModeAutoWhenQuantifyLow; }
    void setPowerSavingModeAutoWhenQuantifyLow(bool bLowBatteryAutoIntoSaveEnergyMode);

    inline bool powerSavingModeAuto() const { return m_bPowerSavingModeAuto; }
    void setPowerSavingModeAuto(bool bAutoIntoSaveEnergyMode);

    inline int powerSavingModeLowerBrightnessThreshold() const { return m_dPowerSavingModeLowerBrightnessThreshold; }
    void setPowerSavingModeLowerBrightnessThreshold(uint dPowerSavingModeLowerBrightnessThreshold);

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
#ifndef DCC_DISABLE_POWERSAVE
    void autoPowerSavingModeChanged(bool autoPowerSaveMode);
    void powerSaveModeChanged(bool powerSaveMode);
#endif
    void haveBettaryChanged(bool haveBettary);
    void batteryLockScreenDelayChanged(const int batteryLockScreenTime);
    void powerLockScreenDelayChanged(const int powerLockScreenTime);
    void batteryPercentageChanged(double batteryPercentage);
    //------------------------sp2 add-------------------------------
    void powerSavingModeAutoWhenQuantifyLowChanged(const bool state);
    void powerSavingModeAutoChanged(const bool state);
    void powerSavingModeLowerBrightnessThresholdChanged(const uint level);
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
    void highPerformaceChanged(bool value);

private:
    bool m_lidPresent;//以此判断是否为笔记本
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
#ifndef DCC_DISABLE_POWERSAVE
    bool m_autoPowerSaveMode{false};
    bool m_powerSaveMode{false};
#endif
    bool m_haveBettary;
    int m_batteryLockScreenDelay;
    int m_powerLockScreenDelay;
    double m_batteryPercentage;
    //---------------sp2 add------------------
    bool m_bPowerSavingModeAutoWhenQuantifyLow;
    bool m_bPowerSavingModeAuto;
    uint m_dPowerSavingModeLowerBrightnessThreshold;
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
};

}
}


#endif // POWERMODEL_H
