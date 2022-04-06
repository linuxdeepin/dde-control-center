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
#include "useelectricwidget.h"

#include <QPushButton>
#include <QtMath>
#include <QComboBox>

#include "widgets/titledslideritem.h"
#include "widgets/dccslider.h"
#include "widgets/dccsliderannotated.h"
#include "widgets/optionitem.h"
#include "widgets/switchwidget.h"
#include "widgets/settingsgroup.h"
#include "modules/power/powermodel.h"
#include "window/utils.h"
#include "widgets/comboxwidget.h"

using namespace dcc::widgets;
using namespace dcc::power;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::power;

UseElectricWidget::UseElectricWidget(PowerModel *model, QWidget *parent)
    : QWidget(parent)
    , m_layout(new QVBoxLayout)
    , m_autoLockScreen(new TitledSliderItem(tr("Lock screen after")))
//    , m_suspendOnLidClose(new SwitchWidget(tr("Suspend on lid close")))
    , m_cmbPowerBtn(new ComboxWidget(tr("When pressing the power button")))
    , m_cmbCloseLid(new ComboxWidget(tr("When the lid is closed")))
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //~ contents_path /power/Plugged In
    //~ child_page Plugged In
    m_autoLockScreen->setAccessibleName(tr("Lock screen after"));
    //~ contents_path /power/Plugged In
    //~ child_page Plugged In
//    m_suspendOnLidClose->setAccessibleName(tr("Suspend on lid close"));

    SettingsGroup *powerSettingsGrp = new SettingsGroup;
    powerSettingsGrp->setSpacing(List_Interval);
    if (!IsServerSystem) {
        m_monitorSleepOnPower = new TitledSliderItem(tr("Monitor will suspend after"));
        //~ contents_path /power/Plugged In
        //~ child_page Plugged In
        m_monitorSleepOnPower->setAccessibleName(tr("Monitor will suspend after"));
        m_monitorSleepOnPower->slider()->setType(DCCSlider::Vernier);
        m_monitorSleepOnPower->slider()->setRange(1, 7);
        m_monitorSleepOnPower->slider()->setTickPosition(QSlider::TicksBelow);
        m_monitorSleepOnPower->slider()->setTickInterval(1);
        m_monitorSleepOnPower->slider()->setPageStep(1);
        connect(m_monitorSleepOnPower->slider(), &DCCSlider::valueChanged, this, &UseElectricWidget::requestSetScreenBlackDelayOnPower);
        powerSettingsGrp->appendItem(m_monitorSleepOnPower);
    }
    if (!IsServerSystem) {
        m_computerSleepOnPower = new TitledSliderItem(tr("Computer will suspend after"));
        //~ contents_path /power/Plugged In
        //~ child_page Plugged In
        m_computerSleepOnPower->setAccessibleName(tr("Computer will suspend after"));
        m_computerSleepOnPower->slider()->setType(DCCSlider::Vernier);
        m_computerSleepOnPower->slider()->setRange(1, 7);
        m_computerSleepOnPower->slider()->setTickPosition(QSlider::TicksBelow);
        m_computerSleepOnPower->slider()->setTickInterval(1);
        m_computerSleepOnPower->slider()->setPageStep(1);
        connect(m_computerSleepOnPower->slider(), &DCCSlider::valueChanged, this, &UseElectricWidget::requestSetSleepDelayOnPower);
        powerSettingsGrp->appendItem(m_computerSleepOnPower);
    }

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


    powerSettingsGrp->appendItem(m_autoLockScreen);
    powerSettingsGrp->appendItem(m_cmbCloseLid);
    powerSettingsGrp->appendItem(m_cmbPowerBtn);

    m_layout->setMargin(0);
    m_layout->addWidget(powerSettingsGrp);
    m_layout->setAlignment(Qt::AlignTop);
    m_layout->setSpacing(10);
    setLayout(m_layout);

    QStringList annos;
    annos << "1m" << "5m" << "10m" << "15m" << "30m" << "1h" << tr("Never");
    if (!IsServerSystem) {
        m_computerSleepOnPower->setAnnotations(annos);
    }

    if (!IsServerSystem) {
            m_monitorSleepOnPower->setAnnotations(annos);
        }

    m_autoLockScreen->slider()->setType(DCCSlider::Vernier);
    m_autoLockScreen->slider()->setRange(1, 7);
    m_autoLockScreen->slider()->setTickPosition(QSlider::TicksBelow);
    m_autoLockScreen->slider()->setTickInterval(1);
    m_autoLockScreen->slider()->setPageStep(1);
    m_autoLockScreen->setAnnotations(annos);

    connect(m_monitorSleepOnPower->slider(), &DCCSlider::valueChanged, this, &UseElectricWidget::requestSetScreenBlackDelayOnPower);
    connect(m_autoLockScreen->slider(), &DCCSlider::valueChanged, this, &UseElectricWidget::requestSetAutoLockScreenOnPower);
    connect(m_cmbPowerBtn, &ComboxWidget::onIndexChanged, this, [ = ](int nIndex) {
        if (!model->getSuspend()) {
            if (IsServerSystem || !model->canHibernate()) {
                Q_EMIT requestSetLinePowerPressPowerBtnAction(nIndex > 0 ? nIndex + 2 : nIndex);
            } else {
                Q_EMIT requestSetLinePowerPressPowerBtnAction(nIndex > 0 ? nIndex + 1 : nIndex);
            }
        } else {
            if (IsServerSystem || !model->canHibernate()) {
                Q_EMIT requestSetLinePowerPressPowerBtnAction(nIndex > 1 ? nIndex + 1 : nIndex);
            } else {
                Q_EMIT requestSetLinePowerPressPowerBtnAction(nIndex);
            }
        }
    });
    connect(m_cmbCloseLid, &ComboxWidget::onIndexChanged, [ = ](int nIndex) {
        if (m_mCombox2PowerAction4CloseId.contains(nIndex)) {
            int powerAction = m_mCombox2PowerAction4CloseId[nIndex];
            Q_EMIT requestSetLinePowerLidClosedAction(powerAction);
        }
    });

}

UseElectricWidget::~UseElectricWidget()
{

}

void UseElectricWidget::setModel(const PowerModel *model)
{
    connect(model, &PowerModel::screenBlackDelayChangedOnPower, this, &UseElectricWidget::setScreenBlackDelayOnPower);
    connect(model, &PowerModel::powerLockScreenDelayChanged, this, &UseElectricWidget::setLockScreenAfter);

    if (!IsServerSystem) {
        connect(model, &PowerModel::sleepDelayChangedOnPower, this, &UseElectricWidget::setSleepDelayOnPower);
        setSleepDelayOnPower(model->sleepDelayOnPower());
    }

    setLockScreenAfter(model->getPowerLockScreenDelay());

    if (m_monitorSleepOnPower) {
        connect(model, &PowerModel::screenBlackDelayChangedOnPower, this, &UseElectricWidget::setScreenBlackDelayOnPower);
        setScreenBlackDelayOnPower(model->screenBlackDelayOnPower());
    }

    if (m_computerSleepOnPower) {
        m_computerSleepOnPower->setVisible(model->canSleep());
    }

    //--------------sp2 add-----------------
    m_cmbCloseLid->setVisible(model->lidPresent());
    const int powerAction = model->linePowerLidClosedAction();
    if (m_mPower2ComboxIndex4CloseLid.contains(powerAction)) {
        int comboIndex = m_mPower2ComboxIndex4CloseLid[powerAction];
        m_cmbCloseLid->comboBox()->setCurrentIndex(comboIndex);
    }
    connect(model, &PowerModel::linePowerLidClosedActionChanged, this, [=](const int reply){
       if (m_mPower2ComboxIndex4CloseLid.contains(reply)) {
            int comboIndex = m_mPower2ComboxIndex4CloseLid[reply];
            m_cmbCloseLid->comboBox()->setCurrentIndex(comboIndex);
        }
    });
    int powIndex = model->linePowerPressPowerBtnAction();
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
            m_cmbPowerBtn->comboBox()->setCurrentIndex(model->linePowerPressPowerBtnAction());
        }
    }

    connect(model, &PowerModel::linePowerPressPowerBtnActionChanged, this, [=](const int reply){
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
    //--------------------------------------
}

void UseElectricWidget::setLidClose(bool state)
{
    m_cmbCloseLid->setVisible(state);
}

void UseElectricWidget::setScreenBlackDelayOnPower(const int delay)
{
    m_monitorSleepOnPower->slider()->blockSignals(true);
    m_monitorSleepOnPower->slider()->setValue(delay);
    m_monitorSleepOnPower->setValueLiteral(delayToLiteralString(delay));
    m_monitorSleepOnPower->slider()->blockSignals(false);
}

void UseElectricWidget::setSleepDelayOnPower(const int delay)
{
    m_computerSleepOnPower->slider()->blockSignals(true);
    m_computerSleepOnPower->slider()->setValue(delay);
    m_computerSleepOnPower->setValueLiteral(delayToLiteralString(delay));
    m_computerSleepOnPower->slider()->blockSignals(false);
}

void UseElectricWidget::setLockScreenAfter(const int delay)
{
    m_autoLockScreen->slider()->blockSignals(true);
    m_autoLockScreen->slider()->setValue(delay);
    m_autoLockScreen->setValueLiteral(delayToLiteralString(delay));
    m_autoLockScreen->slider()->blockSignals(false);
}

void UseElectricWidget::setAutoLockScreenOnPower(const int delay)
{
    m_autoLockScreen->slider()->blockSignals(true);
    m_autoLockScreen->slider()->setValue(delay);
    m_autoLockScreen->setValueLiteral(delayToLiteralString(delay));
    m_autoLockScreen->slider()->blockSignals(false);
}

QString UseElectricWidget::delayToLiteralString(const int delay) const
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