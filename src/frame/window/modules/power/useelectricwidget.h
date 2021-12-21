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
#include "modules/power/powermodel.h"

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>

QT_BEGIN_NAMESPACE
class QSlider;
QT_END_NAMESPACE

namespace dcc {
namespace widgets {
class TitledSliderItem;
class SwitchWidget;
class ComboxWidget;
}

namespace power {
class PowerModel;
class PowerWorker;
}
}

namespace DCC_NAMESPACE {
namespace power {

class UseElectricWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UseElectricWidget(dcc::power::PowerModel *model, QWidget *parent = nullptr, dcc::power::PowerWorker *work = nullptr);
    virtual ~UseElectricWidget();

    void setModel(const dcc::power::PowerModel *model);
    void setLidClose(bool state);

Q_SIGNALS:
    void requestSetScreenBlackDelayOnPower(const int delay) const;
    void requestSetSleepDelayOnPower(const int delay) const;
    void requestSetAutoLockScreenOnPower(const int delay) const;
    void requestSetSleepOnLidOnPowerClosed(const bool sleep) const;
    void requestSetLinePowerPressPowerBtnAction(const int reply) const;//按下电源
    void requestSetLinePowerLidClosedAction(const int reply) const;//合上盖子

public Q_SLOTS:
    void setScreenBlackDelayOnPower(const int delay);
    void setSleepDelayOnPower(const int delay);
    void setAutoLockScreenOnPower(const int delay);
    void setLockScreenAfter(const int delay);
    void setCloseLid(const dcc::power::PowerModel *model, int lidIndex);
    void setPowerBtn(const dcc::power::PowerModel *model, int powIndex);
    void updatePowerButtonActionList();

private:
    QString delayToLiteralString(const int delay) const;

private:
    dcc::power::PowerModel *m_model;
    dcc::power::PowerWorker *m_work;
    QVBoxLayout *m_layout = nullptr;
    dcc::widgets::TitledSliderItem *m_monitorSleepOnPower = nullptr;
    dcc::widgets::TitledSliderItem *m_computerSleepOnPower = nullptr;
    dcc::widgets::TitledSliderItem *m_autoLockScreen = nullptr;

    dcc::widgets::ComboxWidget *m_cmbPowerBtn;    //电源按钮
    dcc::widgets::ComboxWidget *m_cmbCloseLid;    //盒盖
};

}// namespace datetime
}// namespace DCC_NAMESPACE
