// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef POWERWORKER_H
#define POWERWORKER_H

#include <com_deepin_daemon_power.h>
#include <com_deepin_system_systempower.h>
#include <org_freedesktop_login1.h>
#include <com_deepin_daemon_powermanager.h>
#include <QObject>

using PowerInter = com::deepin::daemon::Power;
using SysPowerInter = com::deepin::system::Power;
using Login1ManagerInter = org::freedesktop::login1::Manager;
using PowerManager = com::deepin::daemon::PowerManager;

namespace dcc{
namespace power {
class PowerModel;
class PowerWorker : public QObject
{
    Q_OBJECT

public:
    explicit PowerWorker(PowerModel *model, QObject *parent = 0);

    void active();
    void deactive();

public Q_SLOTS:
    void setScreenBlackLock(const bool lock);
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
    //------------sp2 add-----------------------
    void setPowerSavingModeAutoWhenQuantifyLow(bool bLowBatteryAutoIntoSaveEnergyMode);
    void setPowerSavingModeAuto(bool bAutoIntoSaveEnergyMode);
    void setPowerSavingModeLowerBrightnessThreshold(uint dPowerSavingModeLowerBrightnessThreshold);
    void setLinePowerPressPowerBtnAction(int nLinePowerPressPowerBtnAction);
    void setLinePowerLidClosedAction(int nLinePowerLidClosedAction);
    void setBatteryPressPowerBtnAction(int nBatteryPressPowerBtnAction);
    void setBatteryLidClosedAction(int nBatteryLidClosedAction);
    void setLowPowerNotifyEnable(bool bLowPowerNotifyEnable);
    void setLowPowerNotifyThreshold(int dLowPowerNotifyThreshold);
    void setLowPowerAutoSleepThreshold(int dLowPowerAutoSleepThreshold);
    //------------------------------------------
    void setPowerPlan(const QString &powerPlan);

    bool getCurCanSuspend();
    bool getCurCanHibernate();

#ifndef DCC_DISABLE_POWERSAVE
    void setEnablePowerSave(const bool isEnable);
    void setAutoEnablePowerSave(const bool isEnable);
#endif

private:
    int  converToDelayModel(int value);
    int  converToDelayDBus(int value);

private:
    PowerModel *m_powerModel;
    PowerInter *m_powerInter;
    SysPowerInter *m_sysPowerInter;
    Login1ManagerInter *m_login1ManagerInter;
    PowerManager *m_powerManager;
};

}
}


#endif // POWERWORKER_H
