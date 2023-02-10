//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef USEBATTERYMODULE_H
#define USEBATTERYMODULE_H

#include "interface/pagemodule.h"

#include <DListView>

namespace DCC_NAMESPACE {
class SettingsGroup;
class ComboxWidget;
class TitledSliderItem;
class SwitchWidget;
}

class PowerModel;
class PowerWorker;

class UseBatteryModule : public DCC_NAMESPACE::PageModule
{
    Q_OBJECT
public:
    explicit UseBatteryModule(PowerModel *model, PowerWorker *work, QObject *parent = nullptr);
    ~UseBatteryModule() override;
    void active() override;
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

private:
    void initUI();
    void updateComboxActionList();

    QString delayToLiteralString(const int delay) const;
    void setComboBox(DCC_NAMESPACE::ComboxWidget *combox, QList<QPair<QString, int>>::iterator first, QList<QPair<QString, int>>::iterator last);
    void setComboBoxValue(DCC_NAMESPACE::ComboxWidget *combox, int data);

private:
    PowerModel *m_model;
    PowerWorker *m_work;
    const QStringList m_annos;
    QList<QPair<QString, int>> m_Options;
};
#endif // USEBATTERYMODULE_H
