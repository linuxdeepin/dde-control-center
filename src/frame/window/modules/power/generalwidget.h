/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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
#pragma once

#include "interface/namespace.h"
#include "widgets/dccslider.h"
#include "widgets/powerdisplaywidget.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

#include <DListView>

class TitleLabel;
namespace dcc {
namespace widgets {
class SwitchWidget;
class TitledSliderItem;
class PowerDisplayWidget;
class NormalLabel;
class TitleValueItem;
class SettingsGroup;
}



namespace power {
class PowerModel;
}
}

class QLabel;
namespace DCC_NAMESPACE {
namespace power {

class GeneralWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GeneralWidget(QWidget *parent = nullptr, bool bIsBattery = false);
    virtual ~GeneralWidget();

    void setModel(const dcc::power::PowerModel *model);

private:
    void initUi();     // 初始化界面
    void initSlider(); // 初始化 slider 控件

private:
    bool m_bIsBattery;
    QMap<QString, QString> m_powerPlanMap; // Performance mode dictionary
    QVBoxLayout *m_layout;


    /* Power Plan Settings */
    TitleLabel *m_powerPlansLabel;
    DTK_WIDGET_NAMESPACE::DListView *m_powerplanListview = nullptr;
    QStandardItemModel *m_powerPlanModel = nullptr;

    /* Energy Saving Settings */
    dcc::widgets::SwitchWidget *m_swLowPowerAutoIntoSaveEnergyMode;
    dcc::widgets::SwitchWidget *m_autoIntoSaveEnergyMode;
    dcc::widgets::TitledSliderItem *m_sldLowerBrightness = nullptr;

    /* Wakeup Settings */
    dcc::widgets::SwitchWidget *m_wakeComputerNeedPassword;
    dcc::widgets::SwitchWidget *m_wakeDisplayNeedPassword;

    /* Battery Settings */
    TitleLabel *m_batteryLabel;
    dcc::widgets::SwitchWidget *m_powerShowTimeToFull;
    dcc::widgets::PowerDisplayWidget *m_ShowTimeToFullTips;
    dcc::widgets::SwitchWidget *m_showBatteryCapacity;
    dcc::widgets::TitleValueItem *m_batteryCapacity = nullptr;

    dcc::widgets::TitledSliderItem *m_monitorSleepOnPower = nullptr;

Q_SIGNALS:
    void requestSetLowBatteryMode(const bool &state);                         // 同节能模式
    void requestSetPowerSavingModeAutoWhenQuantifyLow(const bool &state);     // 低电量自动切换节能模式
    void requestSetPowerSavingModeAuto(const bool &state);                    // 自动切换节能模式
    void requestSetWakeComputer(const bool &state);                           // 待机恢复输入密码
    void requestSetWakeDisplay(const bool &state);                            // 唤醒显示器输入密码
    void requestSetPowerSaveMode(const bool &state);                          // 节能模式
    void requestSetPowerSavingModeLowerBrightnessThreshold(const int &level); // 节能模式亮度降低
    void requestSetPowerPlan(const QString &powerPlan);                       // 性能模式的设置

public Q_SLOTS:
    void setPowerDisplay(const bool &state);
    void onGSettingsChanged(const QString &key);
    void onBatteryChanged(const bool &state);
    void onPowerPlanChanged(const QModelIndex &index);
    void onCurPowerPlanChanged(const QString &curPowerPlan);
    void onHighPerformanceSupportChanged(const bool isSupport);

private:
    enum {
        PowerPlanRole = Dtk::UserRole + 1,
    };
};
}// namespace datetime
}// namespace DCC_NAMESPACE
