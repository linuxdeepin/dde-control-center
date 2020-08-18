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

#include "interface/moduleinterface.h"

#include <QObject>
#include <QGSettings>
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
    virtual const QString displayName() const override;
    virtual void active() override;
    virtual int load(const QString &path) override;
    QStringList availPage() const override;

Q_SIGNALS:

public Q_SLOTS:
    void onBatteryPercentageChanged(const double value);

private:
    void showGeneral();
    void showUseElectric();
    void showUseBattery();
    bool getDoubleCompare(const double value1, const double value2);

private:
    dcc::power::PowerModel *m_model;
    dcc::power::PowerWorker *m_work;
    QTimer *m_timer;
    QPointer<PowerWidget> m_widget;
    double m_nBatteryPercentage;
    QGSettings *m_powerSetting;
    bool m_isSuspend;
};


}// namespace datetime
}// namespace DCC_NAMESPACE
