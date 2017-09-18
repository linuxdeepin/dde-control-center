/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
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

#include "modulewidget.h"
#include "settingsgroup.h"
#include "switchwidget.h"
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

signals:
    void requestSetScreenBlackLock(const bool lock) const;
    void requestSetSleepLock(const bool lock) const;
    void requestSetSleepOnLidClosed(const bool sleep) const;
    void requestSetScreenBlackDelay(const int delay) const;
    void requestSetSleepDelay(const int delay) const;

private:
    QString delayToLiteralString(const int delay) const;

private slots:
    void setScreenBlackDelay(const int delay);
    void setSleepDelay(const int delay);

private:
    widgets::SettingsGroup *m_sleepTimeoutSettings;
    widgets::TitledSliderItem *m_monitorSleep;
    widgets::TitledSliderItem *m_computerSleep;
    widgets::SettingsGroup *m_passwordSettings;
    widgets::SwitchWidget *m_displayNeedPassword;
    widgets::SwitchWidget *m_wakeNeedPassword;
    widgets::SettingsGroup *m_notebookSettings;
    widgets::SwitchWidget *m_sleepOnLidOff;
    QSlider *m_monitorSlider;
    QSlider *m_sleepSlider;
};
}

}



#endif // POWERWIDGET_H
