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

#include "window/namespace.h"

#include <QObject>
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

using namespace dcc::widgets;
using namespace dcc::power;

namespace DCC_NAMESPACE {
namespace power {

class GeneralWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GeneralWidget(QWidget *parent = nullptr);
    virtual ~GeneralWidget();

    void setModel(const PowerModel *model);

private:
    QVBoxLayout *m_layout;
    SwitchWidget *m_lowBatteryMode;
    SwitchWidget *m_autoIntoSaveEnergyMode;
    SwitchWidget *m_wakeComputerNeedPassword;
    SwitchWidget *m_wakeDisplayNeedPassword;

Q_SIGNALS:
    void requestSetLowBatteryMode(const bool &state);
    void requestSetAutoIntoSaveEnergyMode(const bool &state);
    void requestSetWakeComputer(const bool &state);
    void requestSetWakeDisplay(const bool &state);

public Q_SLOTS:
};

}// namespace datetime
}// namespace DCC_NAMESPACE
