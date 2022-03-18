/*
* Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
*
* Author:     caixiangrong <caixiangrong@uniontech.com>
*
* Maintainer: caixiangrong <caixiangrong@uniontech.com>
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
#include "useelectricmodule.h"
#include "widgetmodule.h"
#include "powermodel.h"
#include "powerworker.h"
#include "utils.h"
#include "widgets/comboxwidget.h"
#include "widgets/titledslideritem.h"
#include "widgets/dccslider.h"
#include "widgets/settingsgroup.h"

DCC_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

UseElectricModule::UseElectricModule(PowerModel *model, PowerWorker *work, QObject *parent)
    : ModuleObject("power::useelectric", tr("UseElectric"), QIcon::fromTheme("dcc_using_electric"), parent)
    , m_model(model)
    , m_work(work)
{
    setChildType(ModuleObject::ChildType::Page);
    deactive();
    connect(this, &UseElectricModule::requestSetScreenBlackDelayOnPower, m_work, &PowerWorker::setScreenBlackDelayOnPower);
    connect(this, &UseElectricModule::requestSetSleepDelayOnPower, m_work, &PowerWorker::setSleepDelayOnPower);
    connect(this, &UseElectricModule::requestSetSleepOnLidOnPowerClosed, m_work, &PowerWorker::setSleepOnLidOnPowerClosed); //Suspend on lid close
    connect(this, &UseElectricModule::requestSetAutoLockScreenOnPower, m_work, &PowerWorker::setLockScreenDelayOnPower);

    connect(this, &UseElectricModule::requestSetLinePowerPressPowerBtnAction, m_work, &PowerWorker::setLinePowerPressPowerBtnAction);
    connect(this, &UseElectricModule::requestSetLinePowerLidClosedAction, m_work, &PowerWorker::setLinePowerLidClosedAction);

    //　电源设置
    appendChild(new WidgetModule<SettingsGroup>("powerLowerBrightness", tr("Power Saving Settings"), this, &UseElectricModule::initPowerSettings));
    appendChild(new ModuleObject); // 多个项时才会加弹簧

    setModel(m_model);
}

UseElectricModule::~UseElectricModule()
{
}

void UseElectricModule::deactive()
{
    m_cmbPowerBtn = nullptr;
    m_cmbCloseLid = nullptr;
    m_autoLockScreen = nullptr;
    m_computerSleepOnPower = nullptr;
    m_monitorSleepOnPower = nullptr;
}

void UseElectricModule::setModel(PowerModel *model)
{
    m_model = model;
    connect(m_model, &PowerModel::screenBlackDelayChangedOnPower, this, &UseElectricModule::setScreenBlackDelayOnPower);
    connect(m_model, &PowerModel::powerLockScreenDelayChanged, this, &UseElectricModule::setLockScreenAfter);
    connect(m_model, &PowerModel::hibernateChanged, this, [=] {
        updatePowerButtonActionList();
        setPowerBtn(model->linePowerPressPowerBtnAction());
        setCloseLid(model->linePowerLidClosedAction());
    });
    connect(m_model, &PowerModel::suspendChanged, this, [=] {
        updatePowerButtonActionList();
        setPowerBtn(model->linePowerPressPowerBtnAction());
        setCloseLid(model->linePowerLidClosedAction());
    });

    connect(m_model, &PowerModel::shutdownChanged, this, [=] {
        updatePowerButtonActionList();
    });

    if (!IsServerSystem) {
        connect(m_model, &PowerModel::sleepDelayChangedOnPower, this, &UseElectricModule::setSleepDelayOnPower);
    }

    //--------------sp2 add-----------------

    connect(m_model, &PowerModel::linePowerLidClosedActionChanged, this, [=](const int reply) {
        if (m_cmbCloseLid && reply - 1 < m_cmbCloseLid->comboBox()->count()) {
            setCloseLid(reply);
        }
    });
    //    setCloseLid(model->linePowerLidClosedAction());
    connect(m_model, &PowerModel::linePowerPressPowerBtnActionChanged, this, [=](const int reply) {
        if (m_cmbPowerBtn && reply - 1 < m_cmbPowerBtn->comboBox()->count()) {
            setPowerBtn(reply);
        }
    });
}

void UseElectricModule::initPowerSettings(DCC_NAMESPACE::SettingsGroup *powerSettingsGrp)
{
    powerSettingsGrp->setSpacing(10); // List_Interval

    m_monitorSleepOnPower = new TitledSliderItem(tr("Monitor will suspend after"));
    m_monitorSleepOnPower->setAccessibleName(tr("Monitor will suspend after"));
    m_monitorSleepOnPower->slider()->setType(DCCSlider::Vernier);
    m_monitorSleepOnPower->slider()->setRange(1, 7);
    m_monitorSleepOnPower->slider()->setTickPosition(QSlider::TicksBelow);
    m_monitorSleepOnPower->slider()->setTickInterval(1);
    m_monitorSleepOnPower->slider()->setPageStep(1);
    connect(m_monitorSleepOnPower->slider(), &DCCSlider::valueChanged, this, &UseElectricModule::requestSetScreenBlackDelayOnPower);
    powerSettingsGrp->appendItem(m_monitorSleepOnPower);

    setScreenBlackDelayOnPower(m_model->screenBlackDelayOnPower());

    QStringList annos;
    annos << "1m"
          << "5m"
          << "10m"
          << "15m"
          << "30m"
          << "1h" << tr("Never");

    if (!IsServerSystem) {
        m_computerSleepOnPower = new TitledSliderItem(tr("Computer will suspend after"));
        m_computerSleepOnPower->setAccessibleName(tr("Computer will suspend after"));
        m_computerSleepOnPower->slider()->setType(DCCSlider::Vernier);
        m_computerSleepOnPower->slider()->setRange(1, 7);
        m_computerSleepOnPower->slider()->setTickPosition(QSlider::TicksBelow);
        m_computerSleepOnPower->slider()->setTickInterval(1);
        m_computerSleepOnPower->slider()->setPageStep(1);
        m_computerSleepOnPower->setAnnotations(annos);
        setSleepDelayOnPower(m_model->sleepDelayOnPower());
        connect(m_model, &PowerModel::sleepDelayChangedOnPower, this, &UseElectricModule::setSleepDelayOnPower);
        connect(m_computerSleepOnPower->slider(), &DCCSlider::valueChanged, this, &UseElectricModule::requestSetSleepDelayOnPower);
        powerSettingsGrp->appendItem(m_computerSleepOnPower);
        m_computerSleepOnPower->setVisible(m_model->canSuspend() && m_model->getSuspend());
    }

    m_autoLockScreen = new TitledSliderItem(tr("Lock screen after"));
    m_autoLockScreen->setAccessibleName(tr("Lock screen after"));

    m_cmbCloseLid = new ComboxWidget(tr("When the lid is closed"));
    m_cmbPowerBtn = new ComboxWidget(tr("When pressing the power button"));

    updatePowerButtonActionList();

    powerSettingsGrp->appendItem(m_autoLockScreen);
    powerSettingsGrp->appendItem(m_cmbCloseLid);
    powerSettingsGrp->appendItem(m_cmbPowerBtn);

    m_monitorSleepOnPower->setAnnotations(annos);

    m_autoLockScreen->slider()->setType(DCCSlider::Vernier);
    m_autoLockScreen->slider()->setRange(1, 7);
    m_autoLockScreen->slider()->setTickPosition(QSlider::TicksBelow);
    m_autoLockScreen->slider()->setTickInterval(1);
    m_autoLockScreen->slider()->setPageStep(1);
    m_autoLockScreen->setAnnotations(annos);
    setLockScreenAfter(m_model->getPowerLockScreenDelay());

    //    setModel(m_model);
    setPowerBtn(m_model->linePowerPressPowerBtnAction());
    connect(m_autoLockScreen->slider(), &DCCSlider::valueChanged, this, &UseElectricModule::requestSetAutoLockScreenOnPower);

    connect(m_cmbPowerBtn->comboBox(), &AlertComboBox::clicked, this, [=]() {
        updatePowerButtonActionList();
        setPowerBtn(m_model->linePowerPressPowerBtnAction());
        setCloseLid(m_model->linePowerLidClosedAction());
    });
    connect(m_cmbPowerBtn, &ComboxWidget::dataChanged, this, [=](const QVariant data) {
        Q_EMIT requestSetLinePowerPressPowerBtnAction(data.toInt());
    });
    setCloseLid(m_model->linePowerLidClosedAction());
    m_cmbCloseLid->setVisible(m_model->lidPresent());
    connect(m_model, &PowerModel::lidPresentChanged, m_cmbCloseLid, &ComboxWidget::setVisible);
    connect(m_cmbCloseLid->comboBox(), &AlertComboBox::clicked, this, [=]() {
        updatePowerButtonActionList();
        setPowerBtn(m_model->linePowerPressPowerBtnAction());
        setCloseLid(m_model->linePowerLidClosedAction());
    });
    connect(m_cmbCloseLid, &ComboxWidget::dataChanged, this, [=](const QVariant data) {
        Q_EMIT requestSetLinePowerLidClosedAction(data.toInt());
    });
}

void UseElectricModule::setScreenBlackDelayOnPower(const int delay)
{
    if (!m_monitorSleepOnPower)
        return;
    m_monitorSleepOnPower->slider()->blockSignals(true);
    m_monitorSleepOnPower->slider()->setValue(delay);
    m_monitorSleepOnPower->setValueLiteral(delayToLiteralString(delay));
    m_monitorSleepOnPower->slider()->blockSignals(false);
}

void UseElectricModule::setSleepDelayOnPower(const int delay)
{
    if (!m_computerSleepOnPower)
        return;
    m_computerSleepOnPower->slider()->blockSignals(true);
    m_computerSleepOnPower->slider()->setValue(delay);
    m_computerSleepOnPower->setValueLiteral(delayToLiteralString(delay));
    m_computerSleepOnPower->slider()->blockSignals(false);
}

void UseElectricModule::setLockScreenAfter(const int delay)
{
    if (!m_autoLockScreen)
        return;
    m_autoLockScreen->slider()->blockSignals(true);
    m_autoLockScreen->slider()->setValue(delay);
    m_autoLockScreen->setValueLiteral(delayToLiteralString(delay));
    m_autoLockScreen->slider()->blockSignals(false);
}

void UseElectricModule::setCloseLid(int lidIndex)
{
    if (!m_cmbCloseLid)
        return;
    AlertComboBox *cmbPower = m_cmbCloseLid->comboBox();
    for (int i = 0; i < cmbPower->count(); i++) {
        if (cmbPower->itemData(i).toInt() == lidIndex) {
            m_cmbCloseLid->setCurrentIndex(i);
            break;
        }
    }
}

void UseElectricModule::setPowerBtn(int powIndex)
{
    if (!m_cmbPowerBtn)
        return;
    AlertComboBox *cmbPower = m_cmbPowerBtn->comboBox();
    for (int i = 0; i < cmbPower->count(); i++) {
        if (cmbPower->itemData(i).toInt() == powIndex) {
            m_cmbPowerBtn->setCurrentIndex(i);
            break;
        }
    }
}

void UseElectricModule::updatePowerButtonActionList()
{
    if (!m_model || !m_cmbPowerBtn || !m_cmbCloseLid)
        return;
    QList<QPair<QString, int>> options;
    if (m_model->getShutdown()) {
        options.append({ tr("Shut down"), 0 });
    }
    if (m_work->getCurCanSuspend()) {
        options.append({ tr("Suspend"), 1 });
    }
    if (m_work->getCurCanHibernate()) {
        options.append({ tr("Hibernate"), 2 });
    }
    options.append({ tr("Turn off the monitor"), 3 });
    options.append({ tr("Do nothing"), 4 });

    auto setComboBox = [](ComboxWidget *combox, QList<QPair<QString, int>> options) {
        AlertComboBox *box = combox->comboBox();
        box->blockSignals(true);
        box->clear();
        for (auto it : options) {
            box->addItem(it.first, it.second);
        }
        box->blockSignals(false);
    };
    setComboBox(m_cmbPowerBtn, options);
    options.pop_front();
    setComboBox(m_cmbCloseLid, options);
}

QString UseElectricModule::delayToLiteralString(const int delay) const
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
