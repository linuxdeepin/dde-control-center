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

#include "powermodule.h"
#include "modules/power/powermodel.h"
#include "modules/power/powerworker.h"
#include "powerwidget.h"

#include "generalwidget.h"
#include "useelectricwidget.h"
#include "usebatterywidget.h"

#include <DNotifySender>

using namespace dcc;
using namespace dcc::power;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::power;

PowerModule::PowerModule(dccV20::FrameProxyInterface *frameProxy, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frameProxy)
    , m_model(nullptr)
    , m_work(nullptr)
    , m_timer(new QTimer(this))
    , m_widget(nullptr)
    , m_nPowerLockScreenDelay(0)
    , m_nBatteryLockScreenDelay(0)
{

}

void PowerModule::initialize()
{
    m_model = new PowerModel;
    connect(m_model, &PowerModel::batteryLockScreenDelayChanged, this, &PowerModule::onSetBatteryDefault);
    connect(m_model, &PowerModel::powerLockScreenDelayChanged, this, &PowerModule::onSetPowerDefault);

    m_work = new PowerWorker(m_model);
    m_work->active(); //refresh data

    m_work->moveToThread(qApp->thread());
    m_model->moveToThread(qApp->thread());
}

const QString PowerModule::name() const
{
    return QStringLiteral("power");
}

void PowerModule::active()
{
    m_widget = new PowerWidget;;
    m_widget->initialize(m_model->haveBettary());

    connect(m_widget, &PowerWidget::requestPushWidget, this, &PowerModule::onPushWidget);
    connect(m_model, &PowerModel::haveBettaryChanged, m_widget, &PowerWidget::requestRemoveBattery);
    connect(m_model, &PowerModel::batteryPercentageChanged, this, &PowerModule::onBatteryPercentageChanged);

    m_frameProxy->pushWidget(this, m_widget);
}

void PowerModule::load(QString path)
{
    if (!m_widget) {
        active();
        return;
    }

    QListView *list = m_widget->getListViewPointer();
    powerType type = DEFAULT;

    if (!list) {
        return;
    }

    if (path == "General") {
        type = GENERAL;
    } else if (path == "Plugged In") {
        type = USE_ELECTRIC;
    } else if (path == "On Battery") {
        type = USE_BATTERY;

        //true : use battrty
        //false: not use battery  ->  not into "On Battery"
        if (!m_widget->getIsUseBattety()) {
            qWarning(" Now not use battery.");
            return;
        }
    }

    if (type > DEFAULT && type < COUNT) {
        QModelIndex index = list->model()->index(type, 0);
        list->setCurrentIndex(index);
        list->clicked(index);
    }
}

void PowerModule::showGeneral()
{
    qDebug() << Q_FUNC_INFO;

    GeneralWidget *general = new GeneralWidget;
    general->setModel(m_model);
    m_frameProxy->pushWidget(this, general);

    connect(general, &GeneralWidget::requestSetWakeDisplay, m_work, &PowerWorker::setScreenBlackLock);
    connect(general, &GeneralWidget::requestSetWakeComputer, m_work, &PowerWorker::setSleepLock);

#ifndef DCC_DISABLE_POWERSAVE
    connect(general, &GeneralWidget::requestSetAutoIntoSaveEnergyMode, m_work, &PowerWorker::setAutoEnablePowerSave);
    connect(general, &GeneralWidget::requestSetLowBatteryMode, m_work, &PowerWorker::setEnablePowerSave);
#endif
}

void PowerModule::showUseElectric()
{
    qDebug() << Q_FUNC_INFO;

    UseElectricWidget *electric = new UseElectricWidget;
    electric->setModel(m_model);
    m_frameProxy->pushWidget(this, electric);

    //When use power : false -> hide (default : show)
    if (!m_widget->getIsUseBattety()) {
        electric->setLidClose(m_widget->getIsUseBattety());
    }
    electric->setAutoLockScreenOnBattery(m_nPowerLockScreenDelay);
    connect(electric, &UseElectricWidget::requestSetScreenBlackDelayOnPower, m_work, &PowerWorker::setScreenBlackDelayOnPower);
    connect(electric, &UseElectricWidget::requestSetSleepDelayOnPower, m_work, &PowerWorker::setSleepDelayOnPower);
    connect(electric, &UseElectricWidget::requestSetSleepOnLidOnPowerClosed, m_work, &PowerWorker::setSleepOnLidOnPowerClosed);//Suspend on lid close
    connect(electric, &UseElectricWidget::requestSetAutoLockScreenOnPower, m_work, &PowerWorker::setLockScreenDelayOnPower);
}

void PowerModule::showUseBattery()
{
    qDebug() << Q_FUNC_INFO;

    UseBatteryWidget *battery = new UseBatteryWidget;
    battery->setModel(m_model);
    m_frameProxy->pushWidget(this, battery);

    battery->setAutoLockScreenOnBattery(m_nBatteryLockScreenDelay);
    connect(battery, &UseBatteryWidget::requestSetScreenBlackDelayOnBattery, m_work, &PowerWorker::setScreenBlackDelayOnBattery);
    connect(battery, &UseBatteryWidget::requestSetSleepDelayOnBattery, m_work, &PowerWorker::setSleepDelayOnBattery);
    connect(battery, &UseBatteryWidget::requestSetAutoLockScreenOnBattery, m_work, &PowerWorker::setLockScreenDelayOnBattery);
}

void PowerModule::onPushWidget(int index)
{
    switch (static_cast<powerType>(index)) {
    case GENERAL:
        showGeneral();
        break;
    case USE_ELECTRIC:
        showUseElectric();
        break;
    case USE_BATTERY:
        showUseBattery();
        break;
    default:
        break;
    }
}

void PowerModule::onSetBatteryDefault(const int value)
{
    if (m_nBatteryLockScreenDelay != value) {
        m_nBatteryLockScreenDelay = value;
    }
}

void PowerModule::onSetPowerDefault(const int value)
{
    if (m_nPowerLockScreenDelay != value) {
        m_nPowerLockScreenDelay = value;
    }
}

void PowerModule::onBatteryPercentageChanged(const double value)
{
    if (!m_model->getDoubleCompare(m_nBatteryPercentage, value)) {
        m_nBatteryPercentage = value;

        QString remindData = "";
        if (m_model->getDoubleCompare(value, 20.0)
                || m_model->getDoubleCompare(value, 15.0)
                || m_model->getDoubleCompare(value, 10.0)) {
            remindData = tr("Battery low, please plug in");
        } else if (m_model->getDoubleCompare(value, 5.0)) {
            remindData = tr("Battery critically low");
        }

        //send system info
        if ("" != remindData) {
            Dtk::Core::DUtil::DNotifySender(remindData.toLatin1().data());
        }
    }
}
