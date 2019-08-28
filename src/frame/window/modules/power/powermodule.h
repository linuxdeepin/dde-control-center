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

#include "window/interface/moduleinterface.h"

#include <QObject>
#include <types/zoneinfo.h>

namespace dcc {
namespace power {
class PowerWorker;
class PowerModel;
}
}

namespace DCC_NAMESPACE {
namespace power {

class PowerWidget;

class PowerModule : public QObject, public ModuleInterface
{
    Q_OBJECT
public:
    enum powerType {
        DEFAULT = -1,
        GENERAL,
        USE_ELECTRIC,
        USE_BATTERY,
        COUNT
    };

    PowerModule(FrameProxyInterface *frameProxy, QObject *parent = nullptr);

    virtual void initialize() override;
    virtual const QString name() const override;
    virtual void active() override;
    virtual void load(QString path) override;

private:
    void showGeneral();
    void showUseElectric();
    void showUseBattery();

private:
    dcc::power::PowerModel *m_model;
    dcc::power::PowerWorker *m_work;
    QTimer *m_timer;
    PowerWidget *m_widget;
    int m_nPowerLockScreenDelay;
    int m_nBatteryLockScreenDelay;

Q_SIGNALS:

public Q_SLOTS:
    void onPushWidget(int index);
    void onSetBatteryDefault(const int value);
    void onSetPowerDefault(const int value);
};


}// namespace datetime
}// namespace DCC_NAMESPACE
