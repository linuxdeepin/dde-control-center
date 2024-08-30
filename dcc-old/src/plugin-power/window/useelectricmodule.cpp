//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "useelectricmodule.h"
#include "powermodel.h"
#include "powerworker.h"
#include "utils.h"
#include "widgets/comboxwidget.h"
#include "widgets/titledslideritem.h"
#include "widgets/dccslider.h"
#include "titlemodule.h"
#include "settingsgroupmodule.h"
#include "itemmodule.h"

#include <DComboBox>
#include <DIconTheme>

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE
DCORE_USE_NAMESPACE

UseElectricModule::UseElectricModule(PowerModel *model, PowerWorker *work, QObject *parent)
    : PageModule("pluggedIn", tr("Plugged In"), DIconTheme::findQIcon("dcc_using_electric"), parent)
    , m_model(model)
    , m_work(work)
{
    connect(this, &UseElectricModule::requestSetScreenBlackDelayOnPower, m_work, &PowerWorker::setScreenBlackDelayOnPower);
    connect(this, &UseElectricModule::requestSetSleepDelayOnPower, m_work, &PowerWorker::setSleepDelayOnPower);
    connect(this, &UseElectricModule::requestSetSleepOnLidOnPowerClosed, m_work, &PowerWorker::setSleepOnLidOnPowerClosed); //Suspend on lid close  not using???
    connect(this, &UseElectricModule::requestSetAutoLockScreenOnPower, m_work, &PowerWorker::setLockScreenDelayOnPower);

    connect(this, &UseElectricModule::requestSetLinePowerPressPowerBtnAction, m_work, &PowerWorker::setLinePowerPressPowerBtnAction);
    connect(this, &UseElectricModule::requestSetLinePowerLidClosedAction, m_work, &PowerWorker::setLinePowerLidClosedAction);

    initUI();
}

UseElectricModule::~UseElectricModule()
{
}

void UseElectricModule::deactive()
{
}

void UseElectricModule::initUI()
{
    auto delayToLiteralString = [] (const int delay) ->QString const {
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

    //　电源设置
    appendChild(new TitleModule("screenAndSuspendTitle", tr("Screen and Suspend")));
    SettingsGroupModule *group = new SettingsGroupModule("screenAndSuspendGroup", tr("Screen and Suspend"));
    group->setSpacing(10);
    appendChild(group);
    QStringList annos;
    annos << "1m"
          << "5m"
          << "10m"
          << "15m"
          << "30m"
          << "1h"
          << tr("Never");
    group->appendChild(new ItemModule("turnOffTheMonitorAfter", tr("Turn off the monitor after"),
        [this, annos, &delayToLiteralString] (ModuleObject *module) -> QWidget*{
            Q_UNUSED(module)
            TitledSliderItem *monitorSleepOnPower = new TitledSliderItem(tr("Turn off the monitor after"));
            monitorSleepOnPower->setAccessibleName(tr("Turn off the monitor after"));
            monitorSleepOnPower->slider()->setType(DCCSlider::Vernier);
            monitorSleepOnPower->slider()->setRange(1, 7);
            monitorSleepOnPower->slider()->setTickPosition(QSlider::TicksBelow);
            monitorSleepOnPower->slider()->setTickInterval(1);
            monitorSleepOnPower->slider()->setPageStep(1);
            monitorSleepOnPower->setAnnotations(annos);
            connect(monitorSleepOnPower->slider(), &DCCSlider::valueChanged, this, &UseElectricModule::requestSetScreenBlackDelayOnPower);
            auto setScreenBlackDelayOnPower = [monitorSleepOnPower, &delayToLiteralString] (const int delay) {
                monitorSleepOnPower->slider()->blockSignals(true);
                monitorSleepOnPower->slider()->setValue(delay);
                monitorSleepOnPower->setValueLiteral(delayToLiteralString(delay));
                monitorSleepOnPower->slider()->blockSignals(false);
            };
            setScreenBlackDelayOnPower(m_model->screenBlackDelayOnPower());
            connect(m_model, &PowerModel::screenBlackDelayChangedOnPower, monitorSleepOnPower, setScreenBlackDelayOnPower);
            return monitorSleepOnPower;
        }, false));

    group->appendChild(new ItemModule("lockScreenAfter", tr("Lock screen after"),
        [this, annos, &delayToLiteralString] (ModuleObject *module) -> QWidget*{
            Q_UNUSED(module)
            TitledSliderItem *autoLockScreen = new TitledSliderItem(tr("Lock screen after"));
            autoLockScreen->setAccessibleName(tr("Lock screen after"));
            autoLockScreen->slider()->setType(DCCSlider::Vernier);
            autoLockScreen->slider()->setRange(1, 7);
            autoLockScreen->slider()->setTickPosition(QSlider::TicksBelow);
            autoLockScreen->slider()->setTickInterval(1);
            autoLockScreen->slider()->setPageStep(1);
            autoLockScreen->setAnnotations(annos);
            auto setLockScreenAfter = [autoLockScreen, &delayToLiteralString] (const int delay) {
                autoLockScreen->slider()->blockSignals(true);
                autoLockScreen->slider()->setValue(delay);
                autoLockScreen->setValueLiteral(delayToLiteralString(delay));
                autoLockScreen->slider()->blockSignals(false);
            };
            setLockScreenAfter(m_model->getPowerLockScreenDelay());
            connect(m_model, &PowerModel::powerLockScreenDelayChanged, autoLockScreen, setLockScreenAfter);
            connect(autoLockScreen->slider(), &DCCSlider::valueChanged, this, &UseElectricModule::requestSetAutoLockScreenOnPower);
            return autoLockScreen;
        }, false));

    if (!IsServerSystem) {
        group->appendChild(new ItemModule("computerSuspendsAfter", tr("Computer suspends after"),
            [this, annos, &delayToLiteralString] (ModuleObject *module) -> QWidget*{
                Q_UNUSED(module)
                TitledSliderItem *computerSleepOnPower = new TitledSliderItem(tr("Computer suspends after"));
                computerSleepOnPower->setAccessibleName(tr("Computer suspends after"));
                computerSleepOnPower->slider()->setType(DCCSlider::Vernier);
                computerSleepOnPower->slider()->setRange(1, 7);
                computerSleepOnPower->slider()->setTickPosition(QSlider::TicksBelow);
                computerSleepOnPower->slider()->setTickInterval(1);
                computerSleepOnPower->slider()->setPageStep(1);
                computerSleepOnPower->setAnnotations(annos);
                connect(computerSleepOnPower->slider(), &DCCSlider::valueChanged, this, &UseElectricModule::requestSetSleepDelayOnPower);
                auto setSleepDelayOnPower = [computerSleepOnPower, &delayToLiteralString] (const int delay) {
                    computerSleepOnPower->slider()->blockSignals(true);
                    computerSleepOnPower->slider()->setValue(delay);
                    computerSleepOnPower->setValueLiteral(delayToLiteralString(delay));
                    computerSleepOnPower->slider()->blockSignals(false);
                };
                setSleepDelayOnPower(m_model->sleepDelayOnPower());
                connect(m_model, &PowerModel::sleepDelayChangedOnPower, computerSleepOnPower, setSleepDelayOnPower);
                computerSleepOnPower->setVisible(m_model->canSuspend() && m_model->getSuspend());
                return computerSleepOnPower;
            }, false));
    }

    //combox
    ItemModule *itemLidIsClosed =
            new ItemModule("whenTheLidIsClosed", tr("When the lid is closed"),
                           [this](ModuleObject *module) -> QWidget * {
                               Q_UNUSED(module)
                               AlertComboBox *cmbCloseLid = new AlertComboBox();

                               auto setCloseLidData = [this, cmbCloseLid]() {
                                   updateComboxActionList();
                                   cmbCloseLid->blockSignals(true);
                                   cmbCloseLid->clear();
                                   for (const auto &it : qAsConst(m_comboxOptions)) {
                                       if (it == m_comboxOptions.first())
                                           continue;
                                       cmbCloseLid->addItem(it.first, it.second);
                                   }
                                   for (int i = 0; i < cmbCloseLid->count(); i++) {
                                       if (cmbCloseLid->itemData(i).toInt() == m_model->linePowerLidClosedAction()) {
                                           cmbCloseLid->setCurrentIndex(i);
                                           break;
                                       }
                                   }
                                   cmbCloseLid->blockSignals(false);
                               };

                               setCloseLidData();
                               connect(m_model, &PowerModel::hibernateChanged, cmbCloseLid, setCloseLidData);
                               connect(m_model, &PowerModel::suspendChanged, cmbCloseLid, setCloseLidData);
                               connect(m_model, &PowerModel::linePowerLidClosedActionChanged, cmbCloseLid, setCloseLidData);
                               connect(cmbCloseLid, QOverload<int>::of(&AlertComboBox::currentIndexChanged), this, [this, cmbCloseLid](int index) {
                                   Q_EMIT requestSetLinePowerLidClosedAction(cmbCloseLid->itemData(index).toInt());
                               });
                               return cmbCloseLid;
                           });
    itemLidIsClosed->setVisible(m_model->lidPresent());
    connect(m_model, &PowerModel::lidPresentChanged, itemLidIsClosed, &ItemModule::setVisible);
    group->appendChild(itemLidIsClosed);

    group->appendChild(new ItemModule("whenThePowerButtonIsPressed", tr("When the power button is pressed"),
        [this] (ModuleObject *module) -> QWidget*{
            Q_UNUSED(module)
            AlertComboBox *cmbPowerButton = new AlertComboBox();
            auto setPowerButtonData = [this, cmbPowerButton] () {
                updateComboxActionList();
                cmbPowerButton->blockSignals(true);
                cmbPowerButton->clear();
                for (const auto &it : qAsConst(m_comboxOptions)) {
                    cmbPowerButton->addItem(it.first, it.second);
                }
                for (int i = 0; i < cmbPowerButton->count(); i++) {
                    if (cmbPowerButton->itemData(i).toInt() == m_model->linePowerPressPowerBtnAction()) {
                        cmbPowerButton->setCurrentIndex(i);
                        break;
                    }
                }
                cmbPowerButton->blockSignals(false);
            };

            setPowerButtonData();
            connect(m_model, &PowerModel::hibernateChanged, cmbPowerButton, setPowerButtonData);
            connect(m_model, &PowerModel::suspendChanged, cmbPowerButton, setPowerButtonData);
            connect(m_model, &PowerModel::shutdownChanged, cmbPowerButton, setPowerButtonData);
            connect(m_model, &PowerModel::linePowerPressPowerBtnActionChanged, cmbPowerButton, setPowerButtonData);
            connect(cmbPowerButton, QOverload<int>::of(&AlertComboBox::currentIndexChanged), this, [this, cmbPowerButton](int index) {
                Q_EMIT requestSetLinePowerPressPowerBtnAction(cmbPowerButton->itemData(index).toInt());
            });
            return cmbPowerButton;
        }));


    appendChild(new ModuleObject); // 多个项时才会加弹簧
}

void UseElectricModule::updateComboxActionList()
{
    m_comboxOptions.clear();
    if (m_model->getShutdown()) {
        m_comboxOptions.append({ tr("Shut down"), 0 });
    }
    if (m_model->canSuspend()) {
        m_comboxOptions.append({ tr("Suspend"), 1 });
    }
    if (m_model->canHibernate()) {
        m_comboxOptions.append({ tr("Hibernate"), 2 });
    }
    m_comboxOptions.append({ tr("Turn off the monitor"), 3 });
    m_comboxOptions.append({ tr("Show the shutdown Interface"), 4 });
    m_comboxOptions.append({ tr("Do nothing"), 5 });
}
