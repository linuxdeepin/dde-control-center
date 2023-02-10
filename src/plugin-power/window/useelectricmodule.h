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
    void requestSetLinePowerPressPowerBtnAction(const int reply) const; // 按下电源
    void requestSetLinePowerLidClosedAction(const int reply) const;     // 合上盖子

public Q_SLOTS:
    void setScreenBlackDelayOnPower(const int delay);
    void setSleepDelayOnPower(const int delay);
    void setLockScreenAfter(const int delay);
    void setCloseLid(int lidIndex);
    void setPowerBtn(int powIndex);

private:
    void initUI();
    void updateComboxActionList();

private:
    PowerModel *m_model;
    PowerWorker *m_work;
    QList<QPair<QString, int>> m_comboxOptions;
};

#endif // USEELECTRICMODULE_H
