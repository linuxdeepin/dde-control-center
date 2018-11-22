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

#ifndef POWERWIDGET_H
#define POWERWIDGET_H

#include "modules/modulewidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/switchwidget.h"
#include "powermodel.h"


class QSlider;

namespace dcc {
namespace widgets {
class TitledSliderItem;
}

namespace power {
class PowerWidget : public dcc::ModuleWidget
{
    Q_OBJECT

public:
    explicit PowerWidget();

    void setModel(PowerModel * const model);

Q_SIGNALS:
    void requestSetScreenBlackLock(const bool lock) const;
    void requestSetSleepLock(const bool lock) const;
    void requestSetSleepOnLidClosed(const bool sleep) const;
    void requestSetScreenBlackDelayOnPower(const int delay) const;
    void requestSetScreenBlackDelayOnBattery(const int delay) const;
    void requestSetSleepDelayOnPoewr(const int delay) const;
    void requestSetSleepDelayOnBattery(const int delay) const;
#ifndef DCC_DISABLE_POWERSAVE
    void requestSetPowerSaveMode(const bool enablePowerSave) const;
    void requestSetEnableAutoPSM(const bool enableAuto) const;
#endif

private:
    QString delayToLiteralString(const int delay) const;

private Q_SLOTS:
    void setScreenBlackDelayOnPower(const int delay);
    void setScreenBlackDelayOnBattery(const int delay);
    void setSleepDelayOnPower(const int delay);
    void setSleepDelayOnBattery(const int delay);

private:
#ifndef DCC_DISABLE_POWERSAVE
    widgets::SettingsGroup *m_powerSaveGrp;
    widgets::SwitchWidget *m_powerSaveMode;
    widgets::SwitchWidget *m_autoPowerSave;
#endif

    widgets::SettingsGroup *m_sleepTimeoutSettingsOnPower;
    widgets::SettingsGroup *m_sleepTimeoutSettingsOnBattery;
    widgets::TitledSliderItem *m_monitorSleepOnPower;
    widgets::TitledSliderItem *m_computerSleepOnPower;
    widgets::TitledSliderItem *m_monitorSleepOnBattery;
    widgets::TitledSliderItem *m_computerSleepOnBattery;
    widgets::SettingsGroup *m_passwordSettings;
    widgets::SwitchWidget *m_displayNeedPassword;
    widgets::SwitchWidget *m_wakeNeedPassword;
    widgets::SettingsGroup *m_notebookSettings;
    widgets::SwitchWidget *m_sleepOnLidOff;
};
}

}



#endif // POWERWIDGET_H
