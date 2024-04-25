//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "usebatterymodule.h"
#include "dcombobox.h"
#include "dswitchbutton.h"
#include "itemmodule.h"
#include "powermodel.h"
#include "powerworker.h"
#include "settingsgroupmodule.h"
#include "titlemodule.h"
#include "titlevalueitem.h"
#include "widgets/comboxwidget.h"
#include "widgets/titledslideritem.h"
#include "widgets/dccslider.h"

#include <DConfig>
#include <DIconTheme>

#include <QLayout>

using namespace DCC_NAMESPACE;
DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

const static QMap<int, int> g_sldLowBatteryMap = { { 0, 10 }, { 1, 15 }, { 2, 20 }, { 3, 25 } };

UseBatteryModule::UseBatteryModule(PowerModel *model, PowerWorker *work, QObject *parent)
    : PageModule("onBattery", tr("On Battery"), tr("On Battery"), DIconTheme::findQIcon("dcc_battery"), parent)
    , m_model(model)
    , m_work(work)
    , m_annos({ "1m", "5m", "10m", "15m", "30m", "1h", tr("Never") })
{
    connect(this, &UseBatteryModule::requestSetScreenBlackDelayOnBattery, m_work, &PowerWorker::setScreenBlackDelayOnBattery);
    connect(this, &UseBatteryModule::requestSetSleepDelayOnBattery, m_work, &PowerWorker::setSleepDelayOnBattery);
    connect(this, &UseBatteryModule::requestSetAutoLockScreenOnBattery, m_work, &PowerWorker::setLockScreenDelayOnBattery);

    connect(this, &UseBatteryModule::requestSetBatteryPressPowerBtnAction, m_work, &PowerWorker::setBatteryPressPowerBtnAction);
    connect(this, &UseBatteryModule::requestSetBatteryLidClosedAction, m_work, &PowerWorker::setBatteryLidClosedAction);
    connect(this, &UseBatteryModule::requestSetLowPowerNotifyEnable, m_work, &PowerWorker::setLowPowerNotifyEnable);
    connect(this, &UseBatteryModule::requestSetLowPowerNotifyThreshold, m_work, &PowerWorker::setLowPowerNotifyThreshold);
    connect(this, &UseBatteryModule::requestSetLowPowerAutoSleepThreshold, m_work, &PowerWorker::setLowPowerAutoSleepThreshold);

    initUI();
}

UseBatteryModule::~UseBatteryModule()
{
}

void UseBatteryModule::active()
{
    m_Options.clear();
    /*** 笔记本合盖功能与按电源按钮功能 ***/
    if (m_model->getShutdown()) {
        m_Options.append({ tr("Shut down"), 0 });
    }
    if (m_model->canSuspend()) {
        m_Options.append({ tr("Suspend"), 1 });
    }
    if (m_model->canHibernate()) {
        m_Options.append({ tr("Hibernate"), 2 });
    }
    m_Options.append({ tr("Turn off the monitor"), 3 });
    m_Options.append({ tr("Do nothing"), 4 });
}

void UseBatteryModule::deactive()
{
}

void UseBatteryModule::initUI()
{
    auto delayToLiteralString = [] (const int delay) -> QString const {
        QString strData = "";

        switch (delay) {
        case 1:
            strData = tr("1 Minute");
            break;
        case 2:
            strData = tr("%1 Minutes").arg(5);
            break;
        case 3:
            strData = tr("%1 Minutes").arg(10);
            break;
        case 4:
            strData = tr("%1 Minutes").arg(15);
            break;
        case 5:
            strData = tr("%1 Minutes").arg(30);
            break;
        case 6:
            strData = tr("1 Hour");
            break;
        case 7:
            strData = tr("Never");
            break;
        default:
            strData = tr("%1 Minutes").arg(15);
            break;
        }

        return strData;
    };

    appendChild(new TitleModule("screenAndSuspendTitle", tr("Screen and Suspend")));
    SettingsGroupModule *group = new SettingsGroupModule("screenAndSuspendGroup", tr("Screen and Suspend"));
    group->setSpacing(10);
    appendChild(group);
    group->appendChild(new ItemModule("turnOffTheMonitorAfter", tr("Turn off the monitor after"),
        [this, delayToLiteralString] (ModuleObject *module) -> QWidget*{
            Q_UNUSED(module)
            TitledSliderItem *monitorSleepOnBattery = new TitledSliderItem(tr("Turn off the monitor after"));
            monitorSleepOnBattery->setTitle(tr("Turn off the monitor after"));
            monitorSleepOnBattery->setAccessibleName(tr("Turn off the monitor after"));
            monitorSleepOnBattery->slider()->setType(DCCSlider::Vernier);
            monitorSleepOnBattery->slider()->setRange(1, 7);
            monitorSleepOnBattery->slider()->setTickPosition(QSlider::TicksBelow);
            monitorSleepOnBattery->slider()->setTickInterval(1);
            monitorSleepOnBattery->slider()->setPageStep(1);
            monitorSleepOnBattery->setAnnotations(m_annos);
            auto setScreenBlackDelayOnBattery = [monitorSleepOnBattery, &delayToLiteralString] (const int delay) {
                monitorSleepOnBattery->slider()->blockSignals(true);
                monitorSleepOnBattery->slider()->setValue(delay);
                monitorSleepOnBattery->setValueLiteral(delayToLiteralString(delay));
                monitorSleepOnBattery->slider()->blockSignals(false);
            };
            connect(m_model, &PowerModel::screenBlackDelayChangedOnBattery, monitorSleepOnBattery, setScreenBlackDelayOnBattery);
            setScreenBlackDelayOnBattery(m_model->screenBlackDelayOnBattery());
            connect(monitorSleepOnBattery->slider(), &DCCSlider::valueChanged, this, &UseBatteryModule::requestSetScreenBlackDelayOnBattery);
            return monitorSleepOnBattery;
        }, false));

    group->appendChild(new ItemModule("lockScreenAfter", tr("Lock screen after"),
        [this, delayToLiteralString] (ModuleObject *module) -> QWidget*{
            Q_UNUSED(module)
            TitledSliderItem *autoLockScreen = new TitledSliderItem(tr("Lock screen after"));
            autoLockScreen->setTitle(tr("Lock screen after"));
            autoLockScreen->setAccessibleName(tr("Lock screen after"));
            autoLockScreen->slider()->setType(DCCSlider::Vernier);
            autoLockScreen->slider()->setRange(1, 7);
            autoLockScreen->slider()->setTickPosition(QSlider::TicksBelow);
            autoLockScreen->slider()->setTickInterval(1);
            autoLockScreen->slider()->setPageStep(1);
            autoLockScreen->setAnnotations(m_annos);
            auto setAutoLockScreenOnBattery = [autoLockScreen, &delayToLiteralString] (const int delay) {
                autoLockScreen->slider()->blockSignals(true);
                autoLockScreen->slider()->setValue(delay);
                autoLockScreen->setValueLiteral(delayToLiteralString(delay));
                autoLockScreen->slider()->blockSignals(false);
            };
            connect(m_model, &PowerModel::batteryLockScreenDelayChanged, autoLockScreen, setAutoLockScreenOnBattery);
            setAutoLockScreenOnBattery(m_model->getBatteryLockScreenDelay());
            connect(autoLockScreen->slider(), &DCCSlider::valueChanged, this, &UseBatteryModule::requestSetAutoLockScreenOnBattery);
            return autoLockScreen;
        }, false));

    group->appendChild(new ItemModule("computerSuspendsAfter", tr("Computer suspends after"),
        [this, delayToLiteralString] (ModuleObject *module) -> QWidget*{
            Q_UNUSED(module)
            TitledSliderItem *computerSleepOnBattery = new TitledSliderItem(tr("Computer suspends after"));
            computerSleepOnBattery->setTitle(tr("Computer suspends after"));
            computerSleepOnBattery->setAccessibleName(tr("Computer will suspend after"));
            computerSleepOnBattery->slider()->setType(DCCSlider::Vernier);
            computerSleepOnBattery->slider()->setRange(1, 7);
            computerSleepOnBattery->slider()->setTickPosition(QSlider::TicksBelow);
            computerSleepOnBattery->slider()->setTickInterval(1);
            computerSleepOnBattery->slider()->setPageStep(1);
            computerSleepOnBattery->setAnnotations(m_annos);
            auto setScreenBlackDelayOnBattery = [computerSleepOnBattery, &delayToLiteralString] (const int delay) {
                computerSleepOnBattery->slider()->blockSignals(true);
                computerSleepOnBattery->slider()->setValue(delay);
                computerSleepOnBattery->setValueLiteral(delayToLiteralString(delay));
                computerSleepOnBattery->slider()->blockSignals(false);
            };
            setScreenBlackDelayOnBattery(m_model->sleepDelayOnBattery());
            connect(m_model, &PowerModel::sleepDelayChangedOnBattery, computerSleepOnBattery, setScreenBlackDelayOnBattery);
            computerSleepOnBattery->setVisible(m_model->canSuspend() && m_model->getSuspend());
            connect(computerSleepOnBattery->slider(), &DCCSlider::valueChanged, this, &UseBatteryModule::requestSetSleepDelayOnBattery);
            return computerSleepOnBattery;
        }, false));

    ItemModule *itemLidIsClosed =
        new ItemModule("whenTheLidIsClosed", tr("When the lid is closed"),
        [this] (ModuleObject *module) -> QWidget*{
            Q_UNUSED(module)
            AlertComboBox *cmbCloseLid = new AlertComboBox();
            auto setCloseLidData = [this, cmbCloseLid] () {
                updateComboxActionList();
                cmbCloseLid->blockSignals(true);
                cmbCloseLid->clear();
                for (const auto &it : qAsConst(m_Options)) {
                    if (it == m_Options.first())
                        continue;
                    cmbCloseLid->addItem(it.first, it.second);
                }
                for (int i = 0; i < cmbCloseLid->count(); i++) {
                    if (cmbCloseLid->itemData(i).toInt() == m_model->batteryLidClosedAction()) {
                        cmbCloseLid->setCurrentIndex(i);
                        break;
                    }
                }
                cmbCloseLid->blockSignals(false);
            };

            setCloseLidData();
            connect(cmbCloseLid, QOverload<int>::of(&AlertComboBox::currentIndexChanged), this, [this, cmbCloseLid](int index) {
                Q_EMIT requestSetBatteryLidClosedAction(cmbCloseLid->itemData(index).toInt());
            });
            connect(m_model, &PowerModel::batteryLidClosedActionChanged, cmbCloseLid, setCloseLidData);
            return cmbCloseLid;
        });
    itemLidIsClosed->setVisible(m_model->lidPresent());
    connect(m_model, &PowerModel::lidPresentChanged, itemLidIsClosed, &ItemModule::setVisible);
    group->appendChild(itemLidIsClosed);
    group->appendChild(new ItemModule("whenThePowerButtonIsPressed", tr("When the power button is pressed"),
        [this] (ModuleObject *module) -> QWidget*{
            Q_UNUSED(module)
            AlertComboBox *cmbPowerBtn = new AlertComboBox();
            auto setPowerButtonData = [this, cmbPowerBtn] () {
                updateComboxActionList();
                cmbPowerBtn->blockSignals(true);
                cmbPowerBtn->clear();
                for (const auto &it : qAsConst(m_Options)) {
                    cmbPowerBtn->addItem(it.first, it.second);
                }
                for (int i = 0; i < cmbPowerBtn->count(); i++) {
                    if (cmbPowerBtn->itemData(i).toInt() == m_model->batteryPressPowerBtnAction()) {
                        cmbPowerBtn->setCurrentIndex(i);
                        break;
                    }
                }
                cmbPowerBtn->blockSignals(false);
            };
            setPowerButtonData();
            connect(cmbPowerBtn, QOverload<int>::of(&AlertComboBox::currentIndexChanged), this, [this, cmbPowerBtn](int index) {
                Q_EMIT requestSetBatteryPressPowerBtnAction(cmbPowerBtn->itemData(index).toInt());
            });
            connect(m_model, &PowerModel::batteryPressPowerBtnActionChanged, cmbPowerBtn, setPowerButtonData);
            return cmbPowerBtn;
        }));

    //　低电量设置
    appendChild(new TitleModule("lowBatteryTitle", tr("Low Battery")));
    group = new SettingsGroupModule("lowBatteryGroup", tr("Low Battery"));
    group->setSpacing(10);
    appendChild(group);
    group->appendChild(new ItemModule("lowBatteryNotification", tr("Low battery notification"),
        [this] (ModuleObject *module) -> QWidget*{
            Q_UNUSED(module)
            DSwitchButton *swBatteryHint = new DSwitchButton();
            swBatteryHint->setChecked(m_model->lowPowerNotifyEnable());
            connect(m_model, &PowerModel::lowPowerNotifyEnableChanged, swBatteryHint, &DSwitchButton::setChecked);

            connect(swBatteryHint, &DSwitchButton::checkedChanged, this, [=](bool bLowPowerNotifyEnable) {
                Q_EMIT requestSetLowPowerNotifyEnable(bLowPowerNotifyEnable);
            });
//            Q_EMIT swBatteryHint->checkedChanged(m_model->lowPowerNotifyEnable());
            return swBatteryHint;
        }));

    ItemModule *itemLowBatteryHint =
        new ItemModule("lowBatteryLevel", tr("Low battery level"),
                       [this] (ModuleObject *module) -> QWidget*{
                            Q_UNUSED(module)
                           DComboBox *cmbLowBatteryHint = new DComboBox();
                           cmbLowBatteryHint->setAccessibleName("Low battery level");
                           QStringList levels;
                           levels << "10%"
                                  << "15%"
                                  << "20%"
                                  << "25%";
                           QVector<int> bindValues{10, 15, 20, 25};
                           cmbLowBatteryHint->addItems(levels);
                           if (bindValues.indexOf(m_model->lowPowerNotifyThreshold()) < cmbLowBatteryHint->count()) {
                               cmbLowBatteryHint->setCurrentIndex(bindValues.indexOf(m_model->lowPowerNotifyThreshold()));
                           }
                           connect(cmbLowBatteryHint, QOverload<int>::of(&DComboBox::currentIndexChanged), this, [=](int index) {
                               if (bindValues.count() > index)
                                   Q_EMIT requestSetLowPowerNotifyThreshold(bindValues.value(index));
                           });
                           connect(m_model, &PowerModel::lowPowerNotifyThresholdChanged, cmbLowBatteryHint, [=](int value) {
                               if (bindValues.contains(value))
                                   cmbLowBatteryHint->setCurrentIndex(bindValues.indexOf(value));
                           });
                           return cmbLowBatteryHint;
                       });
    itemLowBatteryHint->setHidden(!(m_model->haveBettary() && m_model->lowPowerNotifyEnable()));
    connect(m_model, &PowerModel::lowPowerNotifyEnableChanged, itemLowBatteryHint, [itemLowBatteryHint, this] (/*const bool state*/) {
        itemLowBatteryHint->setHidden(!(m_model->haveBettary() && m_model->lowPowerNotifyEnable()));
    });
    group->appendChild(itemLowBatteryHint);

    ItemModule *itemAutoSuspendBatteryLevel =
        new ItemModule("autoSuspendBatteryLevel", tr("Auto suspend battery level"),
            [this] (ModuleObject *module) -> QWidget*{
                Q_UNUSED(module)
                DComboBox *cmbAutoSuspend = new DComboBox();
                cmbAutoSuspend->setAccessibleName("Auto suspend battery level");
                QStringList levels;
                for (int i = 0; i < 9; i++) {
                    levels.append(QString("%1\%").arg(i + 1));
                }
                cmbAutoSuspend->addItems(levels);
                if (m_model->lowPowerAutoSleepThreshold() <= cmbAutoSuspend->count()) {
                    cmbAutoSuspend->setCurrentIndex(m_model->lowPowerAutoSleepThreshold() - 1);
                }
                connect(cmbAutoSuspend, QOverload<int>::of(&DComboBox::currentIndexChanged), this, [=](int index) {
                    if (index < cmbAutoSuspend->count())
                        Q_EMIT requestSetLowPowerAutoSleepThreshold(index + 1);
                });
                connect(m_model, &PowerModel::lowPowerAutoSleepThresholdChanged, cmbAutoSuspend, [=] (const int value){
                    cmbAutoSuspend->setCurrentIndex(value - 1);
                });
                return cmbAutoSuspend;
            });
    itemAutoSuspendBatteryLevel->setVisible(m_model->getSuspend());
    connect(m_model, &PowerModel::suspendChanged, itemAutoSuspendBatteryLevel, &ItemModule::setVisible);
    group->appendChild(itemAutoSuspendBatteryLevel);
    //　电池管理
    appendChild(new TitleModule("batteryManagementTitle", tr("Battery Management")));
    group = new SettingsGroupModule("batteryManagementGroup", tr("Battery Management"));
    group->setSpacing(10);
    appendChild(group);
    group->appendChild(new ItemModule("displayRemainingUsingAndChargingTime", tr("Display remaining using and charging time"),
        [] (ModuleObject *module) -> QWidget*{
            Q_UNUSED(module)
            DSwitchButton *powerShowTimeToFull = new DSwitchButton();
            // depend dock dconfig setting "showtimetofull"
            DConfig *cfgDock = DConfig::create("org.deepin.ds.dock", "org.deepin.ds.dock.power", QString(), powerShowTimeToFull);
            powerShowTimeToFull->setChecked(cfgDock->value("showtimetofull").toBool());
            connect(powerShowTimeToFull, &DSwitchButton::checkedChanged, powerShowTimeToFull, [cfgDock, powerShowTimeToFull] (){
                // 保存设置值
                if (!cfgDock->value("showtimetofull").isNull()) {
                    cfgDock->setValue("showtimetofull", powerShowTimeToFull->isChecked());
                }
                connect(cfgDock, &DConfig::valueChanged, powerShowTimeToFull, [cfgDock, powerShowTimeToFull] (const QString &key) {
                    if ("showtimetofull" == key) {
                        powerShowTimeToFull->setChecked(cfgDock->value("showtimetofull").toBool());
                    }
                });
            });
            return powerShowTimeToFull;
        }));

    group->appendChild(new ItemModule("maximumCapacity", tr("Maximum capacity"),
        [this] (ModuleObject *module) -> QWidget*{
            Q_UNUSED(module)
            TitleValueItem *ShowTimeToFullTips = new TitleValueItem();
            ShowTimeToFullTips->setValue(QString::number(int(m_work->getBatteryCapacity())) + "%");
            return ShowTimeToFullTips;
        }));
}

void UseBatteryModule::updateComboxActionList()
{
    m_Options.clear();
    if (m_model->getShutdown()) {
        m_Options.append({ tr("Shut down"), 0 });
    }
    if (m_model->canSuspend()) {
        m_Options.append({ tr("Suspend"), 1 });
    }
    if (m_model->canHibernate()) {
        m_Options.append({ tr("Hibernate"), 2 });
    }
    m_Options.append({ tr("Turn off the monitor"), 3 });
    m_Options.append({ tr("Show the shutdown Interface"), 4 });
    m_Options.append({ tr("Do nothing"), 5 });
}

QString UseBatteryModule::delayToLiteralString(const int delay) const
{
    QString strData = "";

    switch (delay) {
    case 1:
        strData = tr("1 Minute");
        break;
    case 2:
        strData = tr("%1 Minutes").arg(5);
        break;
    case 3:
        strData = tr("%1 Minutes").arg(10);
        break;
    case 4:
        strData = tr("%1 Minutes").arg(15);
        break;
    case 5:
        strData = tr("%1 Minutes").arg(30);
        break;
    case 6:
        strData = tr("1 Hour");
        break;
    case 7:
        strData = tr("Never");
        break;
    default:
        strData = tr("%1 Minutes").arg(15);
        break;
    }

    return strData;
}
