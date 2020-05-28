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

#include <QWidget>
#include <QVBoxLayout>

namespace dcc {
namespace widgets {
class SwitchWidget;
class NormalLabel;
}

namespace power {
class PowerModel;
}
}

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
    QVBoxLayout *m_layout;
    dcc::widgets::SwitchWidget *m_lowBatteryMode;
    dcc::widgets::SwitchWidget *m_autoIntoSaveEnergyMode;
    dcc::widgets::SwitchWidget *m_wakeComputerNeedPassword;
    dcc::widgets::SwitchWidget *m_wakeDisplayNeedPassword;

Q_SIGNALS:
    void requestSetLowBatteryMode(const bool &state);
    void requestSetAutoIntoSaveEnergyMode(const bool &state);
    void requestSetWakeComputer(const bool &state);
    void requestSetWakeDisplay(const bool &state);

public Q_SLOTS:
};

}// namespace datetime
}// namespace DCC_NAMESPACE
