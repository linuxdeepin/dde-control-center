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

#include "generalwidget.h"
#include "modules/power/powermodel.h"
#include "modules/power/powerworker.h"
#include "powerwidget.h"
#include "usebatterywidget.h"
#include "useelectricwidget.h"
#include "window/mainwindow.h"
#include "window/utils.h"
#include "window/gsettingwatcher.h"

#include <DNotifySender>

using namespace dcc;
using namespace dcc::power;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::power;
#define GSETTING_SHOW_SUSPEND "show-suspend"
#define GSETTING_SHOW_HIBERNATE "show-hibernate"
#define GSETTING_SHOW_SHUTDOWN "show-shutdown"

PowerModule::PowerModule(dccV20::FrameProxyInterface *frameProxy, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frameProxy)
    , m_model(nullptr)
    , m_work(nullptr)
    , m_timer(new QTimer(this))
    , m_widget(nullptr)
{
    m_pMainWindow = dynamic_cast<MainWindow *>(m_frameProxy);
    GSettingWatcher::instance()->insertState("general");
    GSettingWatcher::instance()->insertState("pluggedIn");
    GSettingWatcher::instance()->insertState("onBattery");
}

void PowerModule::preInitialize(bool sync, FrameProxyInterface::PushType pushtype)
{
    Q_UNUSED(sync)
    Q_UNUSED(pushtype);
    //添加此判断是因为公共功能可能泄露。在分配指针“m_model”之前未释放它
    if (m_model) {
        delete m_model;
    }
    m_model = new PowerModel;
    m_work = new PowerWorker(m_model);
    m_work->moveToThread(qApp->thread());
    m_model->moveToThread(qApp->thread());
    m_work->active(); //refresh data

    initSearchData();
}

void PowerModule::initialize()
{
}

const QString PowerModule::name() const
{
    return QStringLiteral("power");
}

const QString PowerModule::displayName() const
{
    return tr("Power");
}

void PowerModule::active()
{
    m_widget = new PowerWidget;
    m_widget->setVisible(false);

    m_powerSetting = new QGSettings("com.deepin.dde.control-center", QByteArray(), this);
    m_isSuspend = m_powerSetting->get(GSETTING_SHOW_SUSPEND).toBool();
    m_model->setSuspend(m_isSuspend && m_model->canSleep());

    bool hibernate = m_powerSetting->get(GSETTING_SHOW_HIBERNATE).toBool();
    m_model->setHibernate(!IsServerSystem && hibernate && m_model->canHibernate());
    connect(m_model, &PowerModel::canHibernateChanged, this, [=](const bool &value) {
        m_model->setHibernate(!IsServerSystem && hibernate && value);
    });

    bool isShutdown = m_powerSetting->get(GSETTING_SHOW_SHUTDOWN).toBool();
    m_model->setShutdown(isShutdown);

    connect(m_model, &PowerModel::haveBettaryChanged, m_widget, &PowerWidget::removeBattery);
    connect(m_model, &PowerModel::batteryPercentageChanged, this, &PowerModule::onBatteryPercentageChanged);
    connect(m_widget, &PowerWidget::requestShowGeneral, this, &PowerModule::showGeneral);
    connect(m_widget, &PowerWidget::requestShowUseBattery, this, &PowerModule::showUseBattery);
    connect(m_widget, &PowerWidget::requestShowUseElectric, this, &PowerModule::showUseElectric);
    connect(m_widget, &PowerWidget::requestUpdateSecondMenu, this, [=](bool needPop) {
        if (m_pMainWindow->getcontentStack().size() >= 2 && needPop) {
            m_frameProxy->popWidget(this);
        }
        m_widget->showDefaultWidget();
    });

    m_frameProxy->pushWidget(this, m_widget);
    m_widget->initialize(m_model->haveBettary());
    m_widget->setVisible(true);
    m_widget->showDefaultWidget();
}

int PowerModule::load(const QString &path)
{
    if (!m_widget) {
        active();
    }

    QListView *list = m_widget->getListViewPointer();
    powerType type = DEFAULT;
    powerServerType serverType = SERVER_DEFAULT;

    if (!list) {
        return 0;
    }

    if (IsServerSystem) {
        if (path == "Plugged In") {
            serverType = SERVER_USE_ELECTRIC;
        } else if (path == "On Battery") {
            serverType = SERVER_USE_BATTERY;
        }

        if (serverType > SERVER_DEFAULT && serverType < SERVER_COUNT) {
            QModelIndex index = list->model()->index(serverType, 0);
            list->setCurrentIndex(index);
            list->clicked(index);
        }
    } else {
        if (path == "General") {
            type = GENERAL;
        } else if (path == "Plugged In") {
            type = USE_ELECTRIC;
        } else if (path == "On Battery") {
            type = USE_BATTERY;
        }

        if (type > DEFAULT && type < COUNT) {
            QModelIndex index = list->model()->index(type, 0);
            list->setCurrentIndex(index);
            list->clicked(index);
        }
    }
    return 0;
}

QStringList PowerModule::availPage() const
{
    QStringList list;
    list << "General" << "Plugged In" << "On Battery";

    return list;
}

void PowerModule::showGeneral()
{
    qDebug() << Q_FUNC_INFO;

    GeneralWidget *general = new GeneralWidget(m_widget, m_widget->getIsUseBattety());
    general->setVisible(false);
    general->setModel(m_model);
    m_frameProxy->pushWidget(this, general);
    general->setVisible(true);

    connect(general, &GeneralWidget::requestSetWakeDisplay, m_work, &PowerWorker::setScreenBlackLock);
    connect(general, &GeneralWidget::requestSetWakeComputer, m_work, &PowerWorker::setSleepLock);

#ifndef DCC_DISABLE_POWERSAVE
    connect(general, &GeneralWidget::requestSetPowerSavingModeAuto, m_work, &PowerWorker::setAutoEnablePowerSave);
    connect(general, &GeneralWidget::requestSetLowBatteryMode, m_work, &PowerWorker::setEnablePowerSave);
    connect(general, &GeneralWidget::requestSetPowerSaveMode, m_work, &PowerWorker::setEnablePowerSave);
#endif

    //-----------------sp2 add-------------------
    connect(general, &GeneralWidget::requestSetPowerSavingModeAutoWhenQuantifyLow, m_work, &PowerWorker::setPowerSavingModeAutoWhenQuantifyLow);
    connect(general, &GeneralWidget::requestSetPowerSavingModeAuto, m_work, &PowerWorker::setPowerSavingModeAuto);
    connect(general, &GeneralWidget::requestSetPowerSavingModeLowerBrightnessThreshold, m_work, &PowerWorker::setPowerSavingModeLowerBrightnessThreshold);
    //-------------------------------------------
    connect(general, &GeneralWidget::requestSetPowerPlan, m_work, &PowerWorker::setPowerPlan);
}

void PowerModule::showUseElectric()
{
    qDebug() << Q_FUNC_INFO;

    UseElectricWidget *electric = new UseElectricWidget(m_model, m_widget);
    electric->setVisible(false);
    //When use power : false -> hide (default : show)
    if (!m_widget->getIsUseBattety()) {
        electric->setLidClose(m_widget->getIsUseBattety());
    }

    electric->setAutoLockScreenOnPower(m_model->getPowerLockScreenDelay());
    connect(electric, &UseElectricWidget::requestSetScreenBlackDelayOnPower, m_work, &PowerWorker::setScreenBlackDelayOnPower);
    connect(electric, &UseElectricWidget::requestSetSleepDelayOnPower, m_work, &PowerWorker::setSleepDelayOnPower);
    connect(electric, &UseElectricWidget::requestSetSleepOnLidOnPowerClosed, m_work, &PowerWorker::setSleepOnLidOnPowerClosed); //Suspend on lid close
    connect(electric, &UseElectricWidget::requestSetAutoLockScreenOnPower, m_work, &PowerWorker::setLockScreenDelayOnPower);

    //-----------------sp2 add-------------------
    connect(electric, &UseElectricWidget::requestSetLinePowerPressPowerBtnAction, m_work, &PowerWorker::setLinePowerPressPowerBtnAction);
    connect(electric, &UseElectricWidget::requestSetLinePowerLidClosedAction, m_work, &PowerWorker::setLinePowerLidClosedAction);
    //-------------------------------------------

    m_frameProxy->pushWidget(this, electric);
    electric->setVisible(true);
}

void PowerModule::showUseBattery()
{
    qDebug() << Q_FUNC_INFO;

    UseBatteryWidget *battery = new UseBatteryWidget(m_model);
    battery->setVisible(false);
    m_frameProxy->pushWidget(this, battery);
    battery->setVisible(true);

    connect(battery, &UseBatteryWidget::requestSetScreenBlackDelayOnBattery, m_work, &PowerWorker::setScreenBlackDelayOnBattery);
    connect(battery, &UseBatteryWidget::requestSetSleepDelayOnBattery, m_work, &PowerWorker::setSleepDelayOnBattery);
    connect(battery, &UseBatteryWidget::requestSetAutoLockScreenOnBattery, m_work, &PowerWorker::setLockScreenDelayOnBattery);

    //-----------------sp2 add-------------------
    connect(battery, &UseBatteryWidget::requestSetBatteryPressPowerBtnAction, m_work, &PowerWorker::setBatteryPressPowerBtnAction);
    connect(battery, &UseBatteryWidget::requestSetBatteryLidClosedAction, m_work, &PowerWorker::setBatteryLidClosedAction);
    connect(battery, &UseBatteryWidget::requestSetLowPowerNotifyEnable, m_work, &PowerWorker::setLowPowerNotifyEnable);
    connect(battery, &UseBatteryWidget::requestSetLowPowerNotifyThreshold, m_work, &PowerWorker::setLowPowerNotifyThreshold);
    connect(battery, &UseBatteryWidget::requestSetLowPowerAutoSleepThreshold, m_work, &PowerWorker::setLowPowerAutoSleepThreshold);
    //-------------------------------------------
}

void PowerModule::initSearchData()
{
    QString module = tr("Power");
    QString generalWidget = tr("General");
    QString pluggedInWidget = tr("Plugged In");
    QString onBatteayWidget = tr("On Battery");

    static QMap<QString, bool> gsettingsMap;

    auto func_is_visible = [=](const QString &gsettings, QString value = "Hidden") {
        bool ret = GSettingWatcher::instance()->getStatus(gsettings) != value;
        gsettingsMap.insert(gsettings, ret);
        return ret;
    };

    auto func_battary_Changed = [=](bool haveBettary, bool haveLib) {

        m_frameProxy->setDetailVisible(module, generalWidget, tr("Auto power saving on low battery"), haveBettary && func_is_visible("powerAutointoSaveenergyBattery"));
        m_frameProxy->setDetailVisible(module, generalWidget, tr("Auto power saving on battery"), haveBettary && func_is_visible("powerAutointoSaveenergy"));
        m_frameProxy->setDetailVisible(module, generalWidget, tr("Display remaining using and charging time"), haveBettary && func_is_visible("powerShowtimeTofull"));
        m_frameProxy->setDetailVisible(module, generalWidget, tr("Maximum capacity"), haveBettary && func_is_visible("powerShowtimeTofulltips"));
        m_frameProxy->setDetailVisible(module, generalWidget, tr("Battery"), haveBettary);

        m_frameProxy->setDetailVisible(module, pluggedInWidget, tr("When the lid is closed"), haveBettary && haveLib && func_is_visible("powerLidPresent"));

        m_frameProxy->setWidgetVisible(module, onBatteayWidget, haveBettary && func_is_visible("onBattery"));
        m_frameProxy->setDetailVisible(module, onBatteayWidget, tr("When pressing the power button"), haveBettary && func_is_visible("powerPressPowerbtn"));
        m_frameProxy->setDetailVisible(module, onBatteayWidget, tr("When the lid is closed"), haveLib && haveBettary && func_is_visible("powerLidPresent"));
        m_frameProxy->setDetailVisible(module, onBatteayWidget, tr("Low Battery Notification"), haveBettary);
        m_frameProxy->setDetailVisible(module, onBatteayWidget, tr("Auto suspend battery level"), haveBettary && func_is_visible("systemSuspend"));
        m_frameProxy->setDetailVisible(module, onBatteayWidget, tr("Monitor will suspend after"), haveBettary);
        m_frameProxy->setDetailVisible(module, onBatteayWidget, tr("Computer will suspend after"), haveBettary && func_is_visible("systemSuspend"));
        m_frameProxy->setDetailVisible(module, onBatteayWidget, tr("Lock screen after"), haveBettary && func_is_visible("powerAutoLockscreen"));

        m_frameProxy->updateSearchData(module);
    };

    auto func_process_all = [=](bool battaty, bool haveLib) {
        m_frameProxy->setModuleVisible(module, true);

        m_frameProxy->setWidgetVisible(module, generalWidget, func_is_visible("general"));
        m_frameProxy->setDetailVisible(module, generalWidget, tr("Password is required to wake up the monitor"), true);
        m_frameProxy->setDetailVisible(module, generalWidget, tr("Password is required to wake up the computer"), func_is_visible("systemSuspend"));
        m_frameProxy->setDetailVisible(module, generalWidget, tr("Decrease brightness"), func_is_visible("powerLowerBrightness"));
        m_frameProxy->setDetailVisible(module, generalWidget, tr("Balanced"), true);
        m_frameProxy->setDetailVisible(module, generalWidget, tr("High Performance"), true);
        m_frameProxy->setDetailVisible(module, generalWidget, tr("Power Saver"), true);
        m_frameProxy->setDetailVisible(module, generalWidget, tr("Power Plans"), func_is_visible("powerPlansLabel"));
        m_frameProxy->setDetailVisible(module, generalWidget, tr("Power Saving Settings"), true);
        m_frameProxy->setDetailVisible(module, generalWidget, tr("Wakeup Settings"), true);

        m_frameProxy->setWidgetVisible(module, pluggedInWidget, func_is_visible("pluggedIn"));
        m_frameProxy->setDetailVisible(module, pluggedInWidget, tr("When pressing the power button"), func_is_visible("powerPressPowerbtn"));
        m_frameProxy->setDetailVisible(module, pluggedInWidget, tr("Lock screen after"), func_is_visible("powerAutoLockscreen"));
        m_frameProxy->setDetailVisible(module, pluggedInWidget, tr("Monitor will suspend after"), true);
        m_frameProxy->setDetailVisible(module, pluggedInWidget, tr("Computer will suspend after"), func_is_visible("systemSuspend"));

        func_battary_Changed(battaty, haveLib);
    };

    connect(m_model, &PowerModel::haveBettaryChanged, this, [=](bool state) {
        func_battary_Changed(state, m_model->lidPresent());
    });

    connect(GSettingWatcher::instance(), &GSettingWatcher::notifyGSettingsChanged, this, [=](const QString &gsetting, const QString &state) {
        if (!gsettingsMap.contains(gsetting)) {
            return;
        }

        if (gsettingsMap.value(gsetting) == state) {
            return;
        }

        bool isVisible = state != "Hidden";

        if ("powerAutointoSaveenergy" == gsetting) {
            m_frameProxy->setDetailVisible(module, generalWidget, tr("Auto power saving on battery"), m_model->haveBettary() && isVisible);
        } else if ("powerAutointoSaveenergyBattery" == gsetting) {
            m_frameProxy->setDetailVisible(module, generalWidget, tr("Auto power saving on low battery"), m_model->haveBettary() && isVisible);
        } else if ("powerLowerBrightness" == gsetting) {
            m_frameProxy->setDetailVisible(module, generalWidget, tr("Decrease brightness"), isVisible);
        } else if ("powerPlansLabel" == gsetting) {
            m_frameProxy->setDetailVisible(module, generalWidget, tr("Power Plans"), isVisible);
        } else if ("systemSuspend" == gsetting) {
            m_frameProxy->setDetailVisible(module, generalWidget, tr("Password is required to wake up the computer"), isVisible);
            m_frameProxy->setDetailVisible(module, pluggedInWidget, tr("Computer will suspend after"), isVisible);
            m_frameProxy->setDetailVisible(module, onBatteayWidget, tr("Auto suspend battery level"), isVisible);
        } else if ("powerShowtimeTofull" == gsetting) {
            m_frameProxy->setDetailVisible(module, generalWidget, tr("Display remaining using and charging time"), isVisible);
        } else if ("powerShowtimeTofulltips" == gsetting) {
            m_frameProxy->setDetailVisible(module, generalWidget, tr("Maximum capacity"), m_model->haveBettary() && isVisible);
        } else if ("general" == gsetting) {
            m_frameProxy->setWidgetVisible(module, generalWidget, isVisible);
        } else if ("pluggedIn" == gsetting) {
            m_frameProxy->setWidgetVisible(module, pluggedInWidget, isVisible);
        } else if ("onBattery" == gsetting) {
            m_frameProxy->setWidgetVisible(module, onBatteayWidget, isVisible);
        } else if ("powerPressPowerbtn" == gsetting) {
            m_frameProxy->setDetailVisible(module, onBatteayWidget, tr("When pressing the power button"), m_model->haveBettary() && func_is_visible("powerPressPowerbtn"));
            m_frameProxy->setDetailVisible(module, pluggedInWidget, tr("When pressing the power button"), func_is_visible("powerPressPowerbtn"));
        } else if ("powerLidPresent" == gsetting) {
            m_frameProxy->setDetailVisible(module, pluggedInWidget, tr("When the lid is closed"), m_model->haveBettary() && m_model->lidPresent() && func_is_visible("powerLidPresent"));
            m_frameProxy->setDetailVisible(module, onBatteayWidget, tr("When the lid is closed"), m_model->haveBettary() && m_model->lidPresent() && func_is_visible("powerLidPresent"));
        } else if ("powerAutoLockscreen" == gsetting) {
            m_frameProxy->setDetailVisible(module, pluggedInWidget, tr("Lock screen after"), func_is_visible("powerAutoLockscreen"));
            m_frameProxy->setDetailVisible(module, onBatteayWidget, tr("Lock screen after"), m_model->haveBettary() && func_is_visible("powerAutoLockscreen"));
        } else {
            qDebug() << " not contains the gsettings : " << gsetting << state;
            return;
        }

        qWarning() << " [notifyGSettingsChanged]  gsetting, state :" << gsetting << state;
        m_frameProxy->updateSearchData(module);
    });

    func_process_all(m_model->haveBettary(), m_model->lidPresent());
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
