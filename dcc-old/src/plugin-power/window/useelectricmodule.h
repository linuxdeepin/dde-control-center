//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef USEELECTRICMODULE_H
#define USEELECTRICMODULE_H

#include "interface/pagemodule.h"

#include <DListView>

class PowerModel;
class PowerWorker;

class UseElectricModule : public DCC_NAMESPACE::PageModule
{
    Q_OBJECT
public:
    explicit UseElectricModule(PowerModel *model, PowerWorker *work, QObject *parent = nullptr);
    ~UseElectricModule() override;
    void deactive() override;

Q_SIGNALS:
    void requestSetScreenBlackDelayOnPower(const int delay) const;
    void requestSetSleepDelayOnPower(const int delay) const;
    void requestSetAutoLockScreenOnPower(const int delay) const;
    void requestSetSleepOnLidOnPowerClosed(const bool sleep) const;
    void requestSetLinePowerPressPowerBtnAction(const int reply) const; //按下电源
    void requestSetLinePowerLidClosedAction(const int reply) const;     //合上盖子

private:
    void initUI();
    void updateComboxActionList();

private:
    PowerModel *m_model;
    PowerWorker *m_work;
    QList<QPair<QString, int>> m_comboxOptions;
};

#endif // USEELECTRICMODULE_H
