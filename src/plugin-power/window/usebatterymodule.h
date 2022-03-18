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
#ifndef USEBATTERYMODULE_H
#define USEBATTERYMODULE_H

#include "interface/moduleobject.h"

#include <DListView>

DCC_BEGIN_NAMESPACE
class SettingsGroup;
class ComboxWidget;
class TitledSliderItem;
class SwitchWidget;
DCC_END_NAMESPACE

class PowerModel;
class PowerWorker;

class UseBatteryModule : public DCC_NAMESPACE::ModuleObject
{
    Q_OBJECT
public:
    explicit UseBatteryModule(PowerModel *model, PowerWorker *work, QObject *parent = nullptr);
    ~UseBatteryModule() override;
    void setModel(PowerModel *model);
    void deactive() override;

Q_SIGNALS:
    void requestSetScreenBlackDelayOnBattery(const int delay) const;
    void requestSetSleepDelayOnBattery(const int delay) const;
    void requestSetAutoLockScreenOnBattery(const int delay) const;
    void requestSetLowPowerNotifyEnable(const bool bState) const;     // 低电量通知
    void requestSetBatteryPressPowerBtnAction(const int reply) const; // 按下电源
    void requestSetBatteryLidClosedAction(const int reply) const;     // 合上盖子
    void requestSetLowPowerNotifyThreshold(const int dValue);         // 低电量通知阈值
    void requestSetLowPowerAutoSleepThreshold(const int dValue);      // 进入待机模式阈值

public Q_SLOTS:
    void setScreenBlackDelayOnBattery(const int delay);
    void setSleepDelayOnBattery(const int delay);
    void setAutoLockScreenOnBattery(const int delay);
    void onLowPowerNotifyThreshold(const int value);
    void onLowPowerAutoSleepThreshold(const int value);
    void setCloseLid(int lidIndex);
    void setPowerBtn(int powIndex);
    void updatePowerButtonActionList();

private:
    void initMonitorSleep(DCC_NAMESPACE::TitledSliderItem *monitorSleepOnBattery);
    void initComputerSleepOnBattery(DCC_NAMESPACE::TitledSliderItem *computerSleepOnBattery);
    void initAutoLockScreen(DCC_NAMESPACE::TitledSliderItem *autoLockScreen);
    void initCmbCloseLid(DCC_NAMESPACE::ComboxWidget *cmbCloseLid);
    void initCmbPowerBtn(DCC_NAMESPACE::ComboxWidget *cmbPowerBtn);
    void initLowBattery(DCC_NAMESPACE::SettingsGroup *lowBatteryGrp);
    void initSldAutoSuspend(DCC_NAMESPACE::TitledSliderItem *sldAutoSuspend);

    QString delayToLiteralString(const int delay) const;

private:
    PowerModel *m_model;
    PowerWorker *m_work;

    const QStringList m_annos;

    DCC_NAMESPACE::TitledSliderItem *m_monitorSleepOnBattery;
    DCC_NAMESPACE::TitledSliderItem *m_computerSleepOnBattery;
    DCC_NAMESPACE::TitledSliderItem *m_autoLockScreen;
    DCC_NAMESPACE::ComboxWidget *m_cmbCloseLid; // 合盖
    DCC_NAMESPACE::ComboxWidget *m_cmbPowerBtn; // 电源按钮
    DCC_NAMESPACE::SwitchWidget *m_swBatteryHint;
    DCC_NAMESPACE::TitledSliderItem *m_sldLowBatteryHint;
    DCC_NAMESPACE::TitledSliderItem *m_sldAutoSuspend;
};
#endif // USEBATTERYMODULE_H
