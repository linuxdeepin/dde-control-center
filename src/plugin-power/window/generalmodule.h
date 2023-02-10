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
#ifndef GENERALMODULE_H
#define GENERALMODULE_H

#include "interface/pagemodule.h"

#include <DListView>

namespace DCC_NAMESPACE {
class SettingsGroup;
class TitledSliderItem;
class DCCListView;
} // namespace DCC_NAMESPACE

namespace DTK_WIDGET_NAMESPACE {
class DComboBox;
}
class PowerModel;
class PowerWorker;

class GeneralModule : public DCC_NAMESPACE::PageModule
{
    Q_OBJECT
public:
    explicit GeneralModule(PowerModel *model, PowerWorker *work, QObject *parent = nullptr);
    ~GeneralModule();
    void deactive() override;

Q_SIGNALS:
    void requestSetLowBatteryMode(const bool &state);                     // 同节能模式
    void requestSetPowerSavingModeAutoWhenQuantifyLow(const bool &state); // 低电量自动切换节能模式
    void requestSetPowerSavingModeAuto(const bool &state); // 自动切换节能模式
    void requestSetWakeComputer(const bool &state);        // 待机恢复输入密码
    void requestSetWakeDisplay(const bool &state);         // 唤醒显示器输入密码
    void requestSetPowerSaveMode(const bool &state);       // 节能模式
    void requestSetPowerSavingModeLowerBrightnessThreshold(const int &level); // 节能模式亮度降低
    void requestSetPowerPlan(const QString &powerPlan); // 性能模式的设置
public Q_SLOTS:

private:
    void initUI();

private:
    QMap<QString, QString> m_powerPlanMap; // Performance mode dictionary
    QStandardItemModel *m_powerPlanModel;

    PowerModel *m_model;
    PowerWorker *m_work;

private:
    enum {
        PowerPlanRole = Dtk::UserRole + 1,
    };
};

#endif // GENERALMODULE_H
