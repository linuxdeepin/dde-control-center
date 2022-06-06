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
#include "usebatterymodule.h"
#include "powermodel.h"
#include "powerworker.h"
#include "widgets/widgetmodule.h"
#include "widgets/comboxwidget.h"
#include "widgets/titledslideritem.h"
#include "widgets/dccslider.h"
#include "widgets/settingsgroup.h"
#include "widgets/switchwidget.h"

#include <QLayout>

DCC_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

const static QMap<int, int> g_sldLowBatteryMap = { { 0, 10 }, { 1, 15 }, { 2, 20 }, { 3, 25 } };

UseBatteryModule::UseBatteryModule(PowerModel *model, PowerWorker *work, QObject *parent)
    : ModuleObject("onBattery", tr("UseBattery"), QIcon::fromTheme("dcc_battery"), parent)
    , m_model(model)
    , m_work(work)
    , m_annos({ "1m", "5m", "10m", "15m", "30m", "1h", tr("Never") })
{
    setChildType(ModuleObject::Page);
    deactive();

    connect(this, &UseBatteryModule::requestSetScreenBlackDelayOnBattery, m_work, &PowerWorker::setScreenBlackDelayOnBattery);
    connect(this, &UseBatteryModule::requestSetSleepDelayOnBattery, m_work, &PowerWorker::setSleepDelayOnBattery);
    connect(this, &UseBatteryModule::requestSetAutoLockScreenOnBattery, m_work, &PowerWorker::setLockScreenDelayOnBattery);

    connect(this, &UseBatteryModule::requestSetBatteryPressPowerBtnAction, m_work, &PowerWorker::setBatteryPressPowerBtnAction);
    connect(this, &UseBatteryModule::requestSetBatteryLidClosedAction, m_work, &PowerWorker::setBatteryLidClosedAction);
    connect(this, &UseBatteryModule::requestSetLowPowerNotifyEnable, m_work, &PowerWorker::setLowPowerNotifyEnable);
    connect(this, &UseBatteryModule::requestSetLowPowerNotifyThreshold, m_work, &PowerWorker::setLowPowerNotifyThreshold);
    connect(this, &UseBatteryModule::requestSetLowPowerAutoSleepThreshold, m_work, &PowerWorker::setLowPowerAutoSleepThreshold);

    //
    appendChild(new WidgetModule<TitledSliderItem>("monitorSleepOnBattery", tr("Monitor will suspend after"), this, &UseBatteryModule::initMonitorSleep));
    appendChild(new WidgetModule<TitledSliderItem>("computerSleepOnBattery", QString(), this, &UseBatteryModule::initComputerSleepOnBattery));
    appendChild(new WidgetModule<TitledSliderItem>("autoLockScreen", QString(), this, &UseBatteryModule::initAutoLockScreen));
    appendChild(new WidgetModule<ComboxWidget>("cmbCloseLid", QString(), this, &UseBatteryModule::initCmbCloseLid));
    appendChild(new WidgetModule<ComboxWidget>("cmbPowerBtn", QString(), this, &UseBatteryModule::initCmbPowerBtn));
    //　低电量设置
    appendChild(new WidgetModule<SettingsGroup>("lowBattery", QString(), this, &UseBatteryModule::initLowBattery));
    appendChild(new WidgetModule<TitledSliderItem>("sldAutoSuspend", QString(), this, &UseBatteryModule::initSldAutoSuspend));

    setModel(m_model);
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
    if (m_work->getCurCanSuspend()) {
        m_Options.append({ tr("Suspend"), 1 });
    }
    if (m_work->getCurCanHibernate()) {
        m_Options.append({ tr("Hibernate"), 2 });
    }
    m_Options.append({ tr("Turn off the monitor"), 3 });
    m_Options.append({ tr("Do nothing"), 4 });
}

void UseBatteryModule::deactive()
{
    m_monitorSleepOnBattery = nullptr;
    m_computerSleepOnBattery = nullptr;
    m_autoLockScreen = nullptr;
    m_cmbCloseLid = nullptr;
    m_cmbPowerBtn = nullptr;
    m_swBatteryHint = nullptr;
    m_sldLowBatteryHint = nullptr;
    m_sldAutoSuspend = nullptr;
}

void UseBatteryModule::setModel(PowerModel *model)
{
    m_model = model;
    connect(m_model, &PowerModel::sleepDelayChangedOnBattery, this, &UseBatteryModule::setSleepDelayOnBattery);
    connect(m_model, &PowerModel::screenBlackDelayChangedOnBattery, this, &UseBatteryModule::setScreenBlackDelayOnBattery);
    connect(m_model, &PowerModel::batteryLockScreenDelayChanged, this, &UseBatteryModule::setAutoLockScreenOnBattery);

    connect(m_model, &PowerModel::lowPowerNotifyThresholdChanged, this, &UseBatteryModule::onLowPowerNotifyThreshold);
    connect(m_model, &PowerModel::lowPowerAutoSleepThresholdChanged, this, &UseBatteryModule::onLowPowerAutoSleepThreshold);

    connect(m_model, &PowerModel::sleepDelayChangedOnBattery, this, &UseBatteryModule::setSleepDelayOnBattery);
    connect(m_model, &PowerModel::screenBlackDelayChangedOnBattery, this, &UseBatteryModule::setScreenBlackDelayOnBattery);
    connect(m_model, &PowerModel::batteryLockScreenDelayChanged, this, &UseBatteryModule::setAutoLockScreenOnBattery);
    connect(m_model, &PowerModel::batteryLidClosedActionChanged, this, [=](const int reply) {
        if (m_cmbCloseLid && reply - 1 < m_cmbCloseLid->comboBox()->count() && reply >= 1) {
            setComboBoxValue(m_cmbCloseLid, reply);
        }
    });
    connect(m_model, &PowerModel::batteryPressPowerBtnActionChanged, this, [=](const int reply) {
        if (m_cmbPowerBtn && reply - 1 < m_cmbPowerBtn->comboBox()->count()) {
            setComboBoxValue(m_cmbPowerBtn, reply);
        }
    });
}

void UseBatteryModule::initMonitorSleep(DCC_NAMESPACE::TitledSliderItem *monitorSleepOnBattery)
{
    m_monitorSleepOnBattery = monitorSleepOnBattery;
    m_monitorSleepOnBattery->setTitle(tr("Monitor will suspend after"));
    m_monitorSleepOnBattery->setAccessibleName(tr("Monitor will suspend after"));
    m_monitorSleepOnBattery->slider()->setType(DCCSlider::Vernier);
    m_monitorSleepOnBattery->slider()->setRange(1, 7);
    m_monitorSleepOnBattery->slider()->setTickPosition(QSlider::TicksBelow);
    m_monitorSleepOnBattery->slider()->setTickInterval(1);
    m_monitorSleepOnBattery->slider()->setPageStep(1);
    m_monitorSleepOnBattery->setAnnotations(m_annos);
    m_monitorSleepOnBattery->addBackground();

    setScreenBlackDelayOnBattery(m_model->screenBlackDelayOnBattery());
    connect(m_monitorSleepOnBattery->slider(), &DCCSlider::valueChanged, this, &UseBatteryModule::requestSetScreenBlackDelayOnBattery);
}

void UseBatteryModule::initComputerSleepOnBattery(DCC_NAMESPACE::TitledSliderItem *computerSleepOnBattery)
{
    m_computerSleepOnBattery = computerSleepOnBattery;
    m_computerSleepOnBattery->setTitle(tr("Computer will suspend after"));
    m_computerSleepOnBattery->setAccessibleName(tr("Computer will suspend after"));
    m_computerSleepOnBattery->slider()->setType(DCCSlider::Vernier);
    m_computerSleepOnBattery->slider()->setRange(1, 7);
    m_computerSleepOnBattery->slider()->setTickPosition(QSlider::TicksBelow);
    m_computerSleepOnBattery->slider()->setTickInterval(1);
    m_computerSleepOnBattery->slider()->setPageStep(1);
    m_computerSleepOnBattery->setAnnotations(m_annos);
    m_computerSleepOnBattery->addBackground();
    setSleepDelayOnBattery(m_model->sleepDelayOnBattery());
    m_computerSleepOnBattery->setVisible(m_model->canSuspend() && m_model->getSuspend());
    connect(m_computerSleepOnBattery->slider(), &DCCSlider::valueChanged, this, &UseBatteryModule::requestSetSleepDelayOnBattery);
}

void UseBatteryModule::initAutoLockScreen(DCC_NAMESPACE::TitledSliderItem *autoLockScreen)
{
    m_autoLockScreen = autoLockScreen;
    m_autoLockScreen->setTitle(tr("Lock screen after"));
    m_autoLockScreen->setAccessibleName(tr("Lock screen after"));
    m_autoLockScreen->slider()->setType(DCCSlider::Vernier);
    m_autoLockScreen->slider()->setRange(1, 7);
    m_autoLockScreen->slider()->setTickPosition(QSlider::TicksBelow);
    m_autoLockScreen->slider()->setTickInterval(1);
    m_autoLockScreen->slider()->setPageStep(1);
    m_autoLockScreen->setAnnotations(m_annos);
    m_autoLockScreen->addBackground();
    setAutoLockScreenOnBattery(m_model->getBatteryLockScreenDelay());
    connect(m_autoLockScreen->slider(), &DCCSlider::valueChanged, this, &UseBatteryModule::requestSetAutoLockScreenOnBattery);
}

void UseBatteryModule::initCmbCloseLid(DCC_NAMESPACE::ComboxWidget *cmbCloseLid)
{
    m_cmbCloseLid = cmbCloseLid;
    m_cmbCloseLid->setTitle(tr("When the lid is closed"));
    m_cmbCloseLid->setVisible(m_model->lidPresent());
    setComboBox(m_cmbCloseLid,m_Options.begin()+1,m_Options.end());
    setComboBoxValue(m_cmbCloseLid, m_model->batteryLidClosedAction());
    connect(m_cmbCloseLid->comboBox(), &AlertComboBox::clicked, this, [=]() {
        setComboBox(m_cmbCloseLid,m_Options.begin()+1,m_Options.end());
        setComboBoxValue(m_cmbPowerBtn, m_model->batteryPressPowerBtnAction());
        setComboBoxValue(m_cmbCloseLid, m_model->batteryLidClosedAction());
    });
    connect(m_cmbCloseLid, &ComboxWidget::dataChanged, this, [=](const QVariant data) {
        Q_EMIT requestSetBatteryLidClosedAction(data.toInt());
    });
}

void UseBatteryModule::initCmbPowerBtn(DCC_NAMESPACE::ComboxWidget *cmbPowerBtn)
{
    m_cmbPowerBtn = cmbPowerBtn;
    m_cmbPowerBtn->setTitle(tr("When pressing the power button"));

    setComboBox(m_cmbPowerBtn,m_Options.begin(),m_Options.end());
    setComboBoxValue(m_cmbPowerBtn, m_model->batteryPressPowerBtnAction());
    connect(m_cmbPowerBtn->comboBox(), &AlertComboBox::clicked, this, [=]() {
        setComboBox(m_cmbPowerBtn,m_Options.begin(),m_Options.end());
        setComboBoxValue(m_cmbPowerBtn, m_model->batteryPressPowerBtnAction());
        setComboBoxValue(m_cmbCloseLid, m_model->batteryLidClosedAction());
    });
    connect(m_cmbPowerBtn, &ComboxWidget::dataChanged, this, [=](const QVariant data) {
        Q_EMIT requestSetBatteryPressPowerBtnAction(data.toInt());
    });
}

void UseBatteryModule::initLowBattery(DCC_NAMESPACE::SettingsGroup *lowBatteryGrp)
{
    lowBatteryGrp->setBackgroundStyle(SettingsGroup::GroupBackground);
    lowBatteryGrp->layout()->setContentsMargins(0, 0, 0, 0);
    m_swBatteryHint = new SwitchWidget(tr("Low Battery Notification"));
    m_sldLowBatteryHint = new TitledSliderItem(tr("Low battery level"));
    m_swBatteryHint->setChecked(m_model->lowPowerNotifyEnable());
    connect(m_model, &PowerModel::lowPowerNotifyEnableChanged, m_swBatteryHint, &SwitchWidget::setChecked);

    connect(m_swBatteryHint, &SwitchWidget::checkedChanged, this, [=](bool bLowPowerNotifyEnable) {
        Q_EMIT requestSetLowPowerNotifyEnable(bLowPowerNotifyEnable);
        m_sldLowBatteryHint->setVisible(bLowPowerNotifyEnable);
    });
    Q_EMIT m_swBatteryHint->checkedChanged(m_model->lowPowerNotifyEnable());

    QStringList options;
    for (auto v : g_sldLowBatteryMap) {
        options.append(QString("%1%").arg(v));
    }
    m_sldLowBatteryHint->setAnnotations(options);
    m_sldLowBatteryHint->slider()->setRange(0, 3);
    m_sldLowBatteryHint->slider()->setType(DCCSlider::Vernier);
    m_sldLowBatteryHint->slider()->setTickPosition(QSlider::NoTicks);
    onLowPowerNotifyThreshold(m_model->lowPowerNotifyThreshold());
    connect(m_sldLowBatteryHint->slider(), &DCCSlider::valueChanged, this, [=](int value) {
        if (g_sldLowBatteryMap.contains(value)) {
            Q_EMIT requestSetLowPowerNotifyThreshold(g_sldLowBatteryMap[value]);
        }
    });

    lowBatteryGrp->appendItem(m_swBatteryHint);
    lowBatteryGrp->appendItem(m_sldLowBatteryHint);
}

void UseBatteryModule::initSldAutoSuspend(DCC_NAMESPACE::TitledSliderItem *sldAutoSuspend)
{
    m_sldAutoSuspend = sldAutoSuspend;
    m_sldAutoSuspend->setTitle(tr("Auto suspend battery level"));
    QStringList options;
    for (int i = 0; i <= 8; i++) {
        options.append(QString("%1%").arg(i + 1));
    }
    m_sldAutoSuspend->setAnnotations(options);
    m_sldAutoSuspend->slider()->setRange(1, 9);
    m_sldAutoSuspend->slider()->setType(DCCSlider::Vernier);
    m_sldAutoSuspend->slider()->setTickPosition(QSlider::NoTicks);
    m_sldAutoSuspend->addBackground();
    onLowPowerAutoSleepThreshold(m_model->lowPowerAutoSleepThreshold());

    m_sldAutoSuspend->setVisible(m_model->getSuspend());
    connect(m_sldAutoSuspend->slider(), &DCCSlider::valueChanged, this, &UseBatteryModule::requestSetLowPowerAutoSleepThreshold);
}

void UseBatteryModule::setScreenBlackDelayOnBattery(const int delay)
{
    if (!m_monitorSleepOnBattery)
        return;
    m_monitorSleepOnBattery->slider()->blockSignals(true);
    m_monitorSleepOnBattery->slider()->setValue(delay);
    m_monitorSleepOnBattery->setValueLiteral(delayToLiteralString(delay));
    m_monitorSleepOnBattery->slider()->blockSignals(false);
}

void UseBatteryModule::setSleepDelayOnBattery(const int delay)
{
    if (!m_computerSleepOnBattery)
        return;
    m_computerSleepOnBattery->slider()->blockSignals(true);
    m_computerSleepOnBattery->slider()->setValue(delay);
    m_computerSleepOnBattery->setValueLiteral(delayToLiteralString(delay));
    m_computerSleepOnBattery->slider()->blockSignals(false);
}

void UseBatteryModule::setAutoLockScreenOnBattery(const int delay)
{
    if (!m_autoLockScreen)
        return;
    m_autoLockScreen->slider()->blockSignals(true);
    m_autoLockScreen->slider()->setValue(delay);
    m_autoLockScreen->setValueLiteral(delayToLiteralString(delay));
    m_autoLockScreen->slider()->blockSignals(false);
}

void UseBatteryModule::onLowPowerNotifyThreshold(const int value)
{
    if (!m_sldLowBatteryHint)
        return;
    int index = -1;
    QMap<int, int>::const_iterator iter = g_sldLowBatteryMap.begin();
    while (iter != g_sldLowBatteryMap.end()) {
        if (iter.value() == value) {
            index = iter.key();
            break;
        }
        ++iter;
    }
    if (index >= 0) {
        m_sldLowBatteryHint->slider()->blockSignals(true);
        m_sldLowBatteryHint->slider()->setValue(index);
        m_sldLowBatteryHint->setValueLiteral(QString("%1%").arg(value));
        m_sldLowBatteryHint->slider()->blockSignals(false);
    }
}

void UseBatteryModule::onLowPowerAutoSleepThreshold(const int value)
{
    if (!m_sldAutoSuspend)
        return;
    m_sldAutoSuspend->slider()->blockSignals(true);
    m_sldAutoSuspend->slider()->setValue(value);
    m_sldAutoSuspend->setValueLiteral(QString("%1%").arg(value));
    m_sldAutoSuspend->slider()->blockSignals(false);
}

void UseBatteryModule::setComboBox(ComboxWidget *combox, QList<QPair<QString, int>>::iterator first, QList<QPair<QString, int>>::iterator last)
{
    AlertComboBox *box = combox->comboBox();
    box->blockSignals(true);
    box->clear();
    for (auto it = first; it != last; it++) {
        box->addItem(it->first, it->second);
    }
    box->blockSignals(false);
    combox->addBackground();
}

void UseBatteryModule::setComboBoxValue(ComboxWidget *combox, int data)
{
    if  (!combox)
        return;
    AlertComboBox *alertComboBox = combox->comboBox();
    for (int i = 0; i < alertComboBox->count(); i++) {
        if (alertComboBox->itemData(i).toInt() == data) {
            alertComboBox->setCurrentIndex(i);
            break;
        }
    }
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
