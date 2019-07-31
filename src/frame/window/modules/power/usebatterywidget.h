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

class QSlider;

namespace dcc {
    namespace widgets {
        class TitledSliderItem;
    }

    namespace power {
        class PowerModel;
    }
}

using namespace dcc::widgets;
using namespace dcc::power;

namespace DCC_NAMESPACE {
namespace power {

class UseBatteryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UseBatteryWidget(QWidget *parent = nullptr);
    virtual ~UseBatteryWidget();

    void setModel(const PowerModel *model);

private:
    QString delayToLiteralString(const int delay) const;

private:
    QVBoxLayout *m_layout;
    TitledSliderItem *m_monitorSleepOnBattery;
    TitledSliderItem *m_computerSleepOnBattery;
    TitledSliderItem *m_autoLockScreen;

Q_SIGNALS:
    void requestSetScreenBlackDelayOnBattery(const int delay) const;
    void requestSetSleepDelayOnBattery(const int delay) const;
    void requestSetAutoLockScreenOnBattery(const bool state) const;

public Q_SLOTS:
    void setScreenBlackDelayOnBattery(const int delay);
    void setSleepDelayOnBattery(const int delay);
    void setAutoLockScreenOnBattery(const int delay);
};

}// namespace datetime
}// namespace DCC_NAMESPACE
