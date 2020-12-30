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

#include "usebatterywidget.h"
#include "widgets/titledslideritem.h"
#include "widgets/dccslider.h"
#include "widgets/dccsliderannotated.h"
#include "widgets/optionitem.h"
#include "widgets/switchwidget.h"
#include "widgets/settingsgroup.h"
#include "modules/power/powermodel.h"
#include "window/utils.h"
#include "widgets/comboxwidget.h"
#include "widgets/contentwidget.h"

#include <QPushButton>
#include <QComboBox>

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::power;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::power;

UseBatteryWidget::UseBatteryWidget(PowerModel *model, QWidget *parent)
    : QWidget(parent)
    , m_layout(new QVBoxLayout)
    , m_monitorSleepOnBattery(new TitledSliderItem(tr("Monitor will suspend after")))
    , m_computerSleepOnBattery(new TitledSliderItem(tr("Computer will suspend after")))
    , m_autoLockScreen(new TitledSliderItem(tr("Lock screen after")))
    , m_sldLowBatteryHint(new TitledSliderItem(tr("Low battery level")))
    , m_sldAutoSuspend(new TitledSliderItem(tr("Auto suspend battery level")))
//    , m_suspendOnLidClose(new SwitchWidget(tr("Suspend on lid close")))
    , m_swBatteryHint(new SwitchWidget(tr("Low Battery Notification")))
    , m_cmbPowerBtn(new ComboxWidget(tr("When pressing the power button")))
    , m_cmbCloseLid(new ComboxWidget(tr("When the lid is closed")))
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    //~ contents_path /power/On Battery
    //~ child_page On Battery
    m_monitorSleepOnBattery->setAccessibleName(tr("Monitor will suspend after"));
    //~ contents_path /power/On Battery
    //~ child_page On Battery
    m_computerSleepOnBattery->setAccessibleName(tr("Computer will suspend after"));
    //~ contents_path /power/On Battery
    //~ child_page On Battery
    m_autoLockScreen->setAccessibleName(tr("Lock screen after"));
    //~ contents_path /power/On Battery
    //~ child_page On Battery
//    m_suspendOnLidClose->setAccessibleName(tr("Suspend on lid close"));


    QStringList options;
    options << tr("Shut down");
    if (model->getSuspend()) {
        options << tr("Suspend");
    }
    if (!IsServerSystem && model->canHibernate()) {
        options << tr("Hibernate");
    }
    options << tr("Turn off the monitor") << tr("Do nothing");

    m_cmbPowerBtn->setComboxOption(options);
    //TODO klu临时去掉此功能
    m_cmbPowerBtn->hide();
    options.pop_front();

    //合盖电源选项
    QStringList optionsLidClose;
    int lidComboIndex = 0;
    if (model->getSuspend()) {
        optionsLidClose << tr("Suspend");
        m_mCombox2PowerAction4CloseId[lidComboIndex] = 1;
        m_mPower2ComboxIndex4CloseLid[1] = lidComboIndex;
        lidComboIndex++;
    }
    if (!IsServerSystem && false) {
        optionsLidClose << tr("Hibernate");
        m_mCombox2PowerAction4CloseId[lidComboIndex] = 2;
        m_mPower2ComboxIndex4CloseLid[2] = lidComboIndex;
        lidComboIndex++;
    }
//    optionsLidClose << tr("Turn off the monitor");
//    m_mCombox2PowerAction4CloseId[lidComboIndex] = 3;
//    m_mPower2ComboxIndex4CloseLid[3] = lidComboIndex;
//    lidComboIndex++;

    optionsLidClose << tr("Do nothing");
    m_mCombox2PowerAction4CloseId[lidComboIndex] = 4;
    m_mPower2ComboxIndex4CloseLid[4] = lidComboIndex;
    lidComboIndex++;
    m_cmbCloseLid->setComboxOption(optionsLidClose);


    options.clear();
    for (int i = 0; i <= 9; i++) {
        options.append(QString());
    }
    options[0] = "16%";
    options[4] = "20%";
    options[9] = "25%";
    m_sldLowBatteryHint->setAnnotations(options);
    m_sldLowBatteryHint->slider()->setRange(16, 25);
    m_sldLowBatteryHint->slider()->setType(DCCSlider::Vernier);
    m_sldLowBatteryHint->slider()->setTickPosition(QSlider::NoTicks);

    options.clear();
    for (int i = 4; i <= 8; i++) {
        options.append(QString("%1%").arg(i + 1));
    }
    m_sldAutoSuspend->setAnnotations(options);
    m_sldAutoSuspend->slider()->setRange(5, 9);
    m_sldAutoSuspend->slider()->setType(DCCSlider::Vernier);
    m_sldAutoSuspend->slider()->setTickPosition(QSlider::NoTicks);

    SettingsGroup *batterySettingsGrp = new SettingsGroup;
    batterySettingsGrp->setSpacing(List_Interval);
    batterySettingsGrp->appendItem(m_monitorSleepOnBattery);
    batterySettingsGrp->appendItem(m_computerSleepOnBattery);
    batterySettingsGrp->appendItem(m_autoLockScreen);
//    batterySettingsGrp->appendItem(m_suspendOnLidClose);
    batterySettingsGrp->appendItem(m_cmbCloseLid);
    batterySettingsGrp->appendItem(m_cmbPowerBtn);
    batterySettingsGrp->appendItem(m_swBatteryHint);
    batterySettingsGrp->appendItem(m_sldLowBatteryHint);
    batterySettingsGrp->appendItem(m_sldAutoSuspend);


    m_layout->setMargin(0);
    m_layout->addWidget(batterySettingsGrp);
    m_layout->setAlignment(Qt::AlignTop);
    m_layout->setSpacing(10);
//    setLayout(m_layout);
    //add scroll
    ContentWidget *contentWgt = new ContentWidget;
    QWidget *mainWgt = new TranslucentFrame;
    mainWgt->setLayout(m_layout);
    contentWgt->setContent(mainWgt);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->addWidget(contentWgt);
    setLayout(mainLayout);

    QStringList annos;
    annos << "1m" << "5m" << "10m" << "15m" << "30m" << "1h" << tr("Never");

    m_monitorSleepOnBattery->slider()->setType(DCCSlider::Vernier);
    m_monitorSleepOnBattery->slider()->setRange(1, 7);
    m_monitorSleepOnBattery->slider()->setTickPosition(QSlider::TicksBelow);
    m_monitorSleepOnBattery->slider()->setTickInterval(1);
    m_monitorSleepOnBattery->slider()->setPageStep(1);
    m_monitorSleepOnBattery->setAnnotations(annos);

    m_computerSleepOnBattery->slider()->setType(DCCSlider::Vernier);
    m_computerSleepOnBattery->slider()->setRange(1, 7);
    m_computerSleepOnBattery->slider()->setTickPosition(QSlider::TicksBelow);
    m_computerSleepOnBattery->slider()->setTickInterval(1);
    m_computerSleepOnBattery->slider()->setPageStep(1);
    m_computerSleepOnBattery->setAnnotations(annos);

    m_autoLockScreen->slider()->setType(DCCSlider::Vernier);
    m_autoLockScreen->slider()->setRange(1, 7);
    m_autoLockScreen->slider()->setTickPosition(QSlider::TicksBelow);
    m_autoLockScreen->slider()->setTickInterval(1);
    m_autoLockScreen->slider()->setPageStep(1);
    m_autoLockScreen->setAnnotations(annos);

    connect(m_monitorSleepOnBattery->slider(), &DCCSlider::valueChanged, this, &UseBatteryWidget::requestSetScreenBlackDelayOnBattery);
    connect(m_computerSleepOnBattery->slider(), &DCCSlider::valueChanged, this, &UseBatteryWidget::requestSetSleepDelayOnBattery);
    connect(m_autoLockScreen->slider(), &DCCSlider::valueChanged, this, &UseBatteryWidget::requestSetAutoLockScreenOnBattery);
//    connect(m_suspendOnLidClose, &SwitchWidget::checkedChanged, this, &UseBatteryWidget::requestSetSleepOnLidOnBatteryClosed);
    connect(m_swBatteryHint, &SwitchWidget::checkedChanged, this, [ = ](bool bLowPowerNotifyEnable) {
        Q_EMIT requestSetLowPowerNotifyEnable(bLowPowerNotifyEnable);
        m_sldLowBatteryHint->setVisible(bLowPowerNotifyEnable);
    });
    connect(m_cmbPowerBtn, &ComboxWidget::onIndexChanged, this, [ = ](int nIndex) {
        if (!model->getSuspend()) {
            if (IsServerSystem || !model->canHibernate()) {
                Q_EMIT requestSetBatteryPressPowerBtnAction(nIndex > 0 ? nIndex + 2 : nIndex);
            } else {
                Q_EMIT requestSetBatteryPressPowerBtnAction(nIndex > 0 ? nIndex + 1 : nIndex);
            }
        } else {
            if (IsServerSystem || !model->canHibernate()) {
                Q_EMIT requestSetBatteryPressPowerBtnAction(nIndex > 1 ? nIndex + 1 : nIndex);
            } else {
                Q_EMIT requestSetBatteryPressPowerBtnAction(nIndex);
            }
        }
    });
    connect(m_cmbCloseLid, &ComboxWidget::onIndexChanged, [ = ](int nIndex) {
        if (m_mCombox2PowerAction4CloseId.contains(nIndex)) {
            int powerAction = m_mCombox2PowerAction4CloseId[nIndex];
            Q_EMIT requestSetBatteryLidClosedAction(powerAction);
        }
    });
}

UseBatteryWidget::~UseBatteryWidget()
{

}

void UseBatteryWidget::setModel(const PowerModel *model)
{
    connect(model, &PowerModel::sleepDelayChangedOnBattery, this, &UseBatteryWidget::setSleepDelayOnBattery);
    connect(model, &PowerModel::screenBlackDelayChangedOnBattery, this, &UseBatteryWidget::setScreenBlackDelayOnBattery);
//    connect(model, &PowerModel::sleepOnLidOnBatteryCloseChanged, m_suspendOnLidClose, &SwitchWidget::setChecked);
    connect(model, &PowerModel::batteryLockScreenDelayChanged, this, &UseBatteryWidget::setAutoLockScreenOnBattery);

    setScreenBlackDelayOnBattery(model->screenBlackDelayOnBattery());
    setSleepDelayOnBattery(model->sleepDelayOnBattery());

    connect(m_swBatteryHint, &SwitchWidget::checkedChanged, this, [ = ](bool bLowPowerNotifyEnable) {
        Q_EMIT requestSetLowPowerNotifyEnable(bLowPowerNotifyEnable);
        m_sldLowBatteryHint->setVisible(bLowPowerNotifyEnable);
    });

    Q_EMIT m_swBatteryHint->checkedChanged(model->lowPowerNotifyEnable());

//    m_suspendOnLidClose->setChecked(model->sleepOnLidOnBatteryClose());
    setAutoLockScreenOnBattery(model->getBatteryLockScreenDelay());

    m_computerSleepOnBattery->setVisible(model->canSleep());
//    m_suspendOnLidClose->setVisible(model->canSleep());


    //--------------sp2 add-----------------
    m_cmbCloseLid->setVisible(model->lidPresent());
    const int powerAction = model->batteryLidClosedAction();
    if (m_mPower2ComboxIndex4CloseLid.contains(powerAction)) {
        int comboIndex = m_mPower2ComboxIndex4CloseLid[powerAction];
        m_cmbCloseLid->comboBox()->setCurrentIndex(comboIndex);
    }
    connect(model, &PowerModel::batteryLidClosedActionChanged, this, [ = ](const int reply) {
        if (m_mPower2ComboxIndex4CloseLid.contains(reply)) {
            int comboIndex = m_mPower2ComboxIndex4CloseLid[reply];
            m_cmbCloseLid->comboBox()->setCurrentIndex(comboIndex);
        }
    });

    int powIndex = model->batteryPressPowerBtnAction();
    if (!model->getSuspend()) {
        if (IsServerSystem || !model->canHibernate()) {
            m_cmbPowerBtn->comboBox()->setCurrentIndex(powIndex > 0 ? powIndex - 2 : powIndex);
        } else {
            m_cmbPowerBtn->comboBox()->setCurrentIndex(powIndex > 0 ? powIndex - 1 : powIndex);
        }
    } else {
        if (IsServerSystem || !model->canHibernate()) {
            m_cmbPowerBtn->comboBox()->setCurrentIndex(powIndex > 2 ? powIndex - 1 : powIndex);
        } else {
            m_cmbPowerBtn->comboBox()->setCurrentIndex(model->batteryPressPowerBtnAction());
        }
    }
    connect(model, &PowerModel::batteryPressPowerBtnActionChanged, this, [ = ](const int reply) {
        if (reply < m_cmbPowerBtn->comboBox()->count()) {
            if (!model->getSuspend()) {
                if (IsServerSystem || !model->canHibernate()) {
                    m_cmbPowerBtn->comboBox()->setCurrentIndex(reply > 0 ? reply - 2 : reply);
                } else {
                    m_cmbPowerBtn->comboBox()->setCurrentIndex(reply > 0 ? reply - 1 : reply);
                }
            } else {
                if (IsServerSystem || !model->canHibernate()) {
                    m_cmbPowerBtn->comboBox()->setCurrentIndex(reply > 2 ? reply - 1 : reply);
                } else {
                    m_cmbPowerBtn->comboBox()->setCurrentIndex(reply);
                }
            }
        }
    });

    m_swBatteryHint->setChecked(model->lowPowerNotifyEnable());
    connect(model, &PowerModel::lowPowerNotifyEnableChanged, m_swBatteryHint, &SwitchWidget::setChecked);

    onLowPowerNotifyThreshold(model->lowPowerNotifyThreshold());
    connect(model, &PowerModel::lowPowerNotifyThresholdChanged, this, &UseBatteryWidget::onLowPowerNotifyThreshold);
    connect(m_sldLowBatteryHint->slider(), &DCCSlider::valueChanged, this, [ = ](int value) {
        Q_EMIT  requestSetLowPowerNotifyThreshold(value);
    });

    onLowPowerAutoSleepThreshold(model->lowPowerAutoSleepThreshold());
    connect(model, &PowerModel::lowPowerAutoSleepThresholdChanged, this, &UseBatteryWidget::onLowPowerAutoSleepThreshold);
    connect(m_sldAutoSuspend->slider(), &DCCSlider::valueChanged, this, [ = ](int value) {
        Q_EMIT  requestSetLowPowerAutoSleepThreshold(value);
    });
    //--------------------------------------
}

void UseBatteryWidget::setScreenBlackDelayOnBattery(const int delay)
{
    m_monitorSleepOnBattery->slider()->blockSignals(true);
    m_monitorSleepOnBattery->slider()->setValue(delay);
    m_monitorSleepOnBattery->setValueLiteral(delayToLiteralString(delay));
    m_monitorSleepOnBattery->slider()->blockSignals(false);
}

void UseBatteryWidget::setSleepDelayOnBattery(const int delay)
{
    m_computerSleepOnBattery->slider()->blockSignals(true);
    m_computerSleepOnBattery->slider()->setValue(delay);
    m_computerSleepOnBattery->setValueLiteral(delayToLiteralString(delay));
    m_computerSleepOnBattery->slider()->blockSignals(false);
}

void UseBatteryWidget::setAutoLockScreenOnBattery(const int delay)
{
    m_autoLockScreen->slider()->blockSignals(true);
    m_autoLockScreen->slider()->setValue(delay);
    m_autoLockScreen->setValueLiteral(delayToLiteralString(delay));
    m_autoLockScreen->slider()->blockSignals(false);
}

void UseBatteryWidget::onLowPowerNotifyThreshold(const int value)
{
    m_sldLowBatteryHint->slider()->blockSignals(true);
    m_sldLowBatteryHint->slider()->setValue(value);
    m_sldLowBatteryHint->setValueLiteral(QString("%1%").arg(value));
    m_sldLowBatteryHint->slider()->blockSignals(false);
}

void UseBatteryWidget::onLowPowerAutoSleepThreshold(const int value)
{
    m_sldAutoSuspend->slider()->blockSignals(true);
    m_sldAutoSuspend->slider()->setValue(value);
    m_sldAutoSuspend->setValueLiteral(QString("%1%").arg(value));
    m_sldAutoSuspend->slider()->blockSignals(false);
}

QString UseBatteryWidget::delayToLiteralString(const int delay) const
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
