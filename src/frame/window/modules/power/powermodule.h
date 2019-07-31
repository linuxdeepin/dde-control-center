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
#include <QTimer>

namespace dcc {
    namespace power {
        class PowerWorker;
        class PowerModel;
    }
}

using namespace dcc;
using namespace dcc::power;

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
    virtual void reset() override;
    virtual const QString name() const override;
    virtual void showPage(const QString &pageName) override;
    virtual QWidget *moduleWidget() override;
    virtual void contentPopped(QWidget *const w) override;

private:
    void showGeneral();
    void showUseElectric();
    void showUseBattery();

private:
    PowerWidget *m_mainWidget;
    PowerModel *m_model;
    PowerWorker *m_work;
    QTimer *m_timer;

Q_SIGNALS:

public Q_SLOTS:
    void onPushWidget(int index);

private:

};


}// namespace datetime
}// namespace DCC_NAMESPACE
