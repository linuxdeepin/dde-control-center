/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#include "powerwidget.h"

#include <QDebug>
#include <QtMath>

#include "widgets/titledslideritem.h"
#include "widgets/dccslider.h"
#include "widgets/dccsliderannotated.h"
#include "widgets/optionitem.h"

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::power;

PowerWidget::PowerWidget()
    : ModuleWidget()
#ifndef DCC_DISABLE_POWERSAVE
    , m_powerSaveMode(new SwitchWidget)
    , m_autoPowerSave(new SwitchWidget)
#endif
    , m_sleepTimeoutSettingsOnPower(new SettingsGroup(tr("Plugged in")))
    , m_sleepTimeoutSettingsOnBattery(new SettingsGroup(tr("On battery")))
    , m_monitorSleepOnPower(new TitledSliderItem(tr("Monitor will suspend after")))
    , m_computerSleepOnPower(new TitledSliderItem(tr("Computer will suspend after")))
    , m_monitorSleepOnBattery(new TitledSliderItem(tr("Monitor will suspend after")))
    , m_computerSleepOnBattery(new TitledSliderItem(tr("Computer will suspend after")))
    , m_passwordSettings(new SettingsGroup)
    , m_displayNeedPassword(new SwitchWidget)
    , m_wakeNeedPassword(new SwitchWidget)
    , m_notebookSettings(new SettingsGroup)
    , m_sleepOnLidOff(new SwitchWidget)
{
    setObjectName("Power");

    QStringList annos;
    annos << "1m" << "5m" << "10m" << "15m" << "30m" << "1h" << tr("Never");

    m_monitorSleepOnPower->slider()->setType(DCCSlider::Vernier);
    m_monitorSleepOnPower->slider()->setRange(1, 7);
    m_monitorSleepOnPower->slider()->setTickPosition(QSlider::TicksBelow);
    m_monitorSleepOnPower->slider()->setTickInterval(1);
    m_monitorSleepOnPower->slider()->setPageStep(1);
    m_monitorSleepOnPower->setAnnotations(annos);

    m_monitorSleepOnBattery->slider()->setType(DCCSlider::Vernier);
    m_monitorSleepOnBattery->slider()->setRange(1, 7);
    m_monitorSleepOnBattery->slider()->setTickPosition(QSlider::TicksBelow);
    m_monitorSleepOnBattery->slider()->setTickInterval(1);
    m_monitorSleepOnBattery->slider()->setPageStep(1);
    m_monitorSleepOnBattery->setAnnotations(annos);

    m_computerSleepOnPower->slider()->setType(DCCSlider::Vernier);
    m_computerSleepOnPower->slider()->setRange(1, 7);
    m_computerSleepOnPower->slider()->setTickPosition(QSlider::TicksBelow);
    m_computerSleepOnPower->slider()->setTickInterval(1);
    m_computerSleepOnPower->slider()->setPageStep(1);
    m_computerSleepOnPower->setAnnotations(annos);

    m_computerSleepOnBattery->slider()->setType(DCCSlider::Vernier);
    m_computerSleepOnBattery->slider()->setRange(1, 7);
    m_computerSleepOnBattery->slider()->setTickPosition(QSlider::TicksBelow);
    m_computerSleepOnBattery->slider()->setTickInterval(1);
    m_computerSleepOnBattery->slider()->setPageStep(1);
    m_computerSleepOnBattery->setAnnotations(annos);

    m_sleepTimeoutSettingsOnPower->appendItem(m_monitorSleepOnPower);
    m_sleepTimeoutSettingsOnPower->appendItem(m_computerSleepOnPower);

    m_sleepTimeoutSettingsOnBattery->appendItem(m_monitorSleepOnBattery);
    m_sleepTimeoutSettingsOnBattery->appendItem(m_computerSleepOnBattery);

    m_displayNeedPassword->setTitle(tr("Password required to wake up the monitor"));
    m_passwordSettings->appendItem(m_displayNeedPassword);

    m_wakeNeedPassword->setTitle(tr("Password required to wake up the computer"));
    m_passwordSettings->appendItem(m_wakeNeedPassword);

    m_sleepOnLidOff->setTitle(tr("Suspend on lid close"));
    m_notebookSettings->appendItem(m_sleepOnLidOff);

#ifndef DCC_DISABLE_POWERSAVE
    m_powerSaveMode->setTitle(tr("Power Saving Mode"));
    m_autoPowerSave->setTitle(tr("Auto Mode Switch"));
    m_powerSaveGrp = new SettingsGroup;
    m_powerSaveGrp->appendItem(m_powerSaveMode);
    m_powerSaveGrp->appendItem(m_autoPowerSave);
    m_centralLayout->addWidget(m_powerSaveGrp);
#endif

    m_centralLayout->addWidget(m_sleepTimeoutSettingsOnPower);
    m_centralLayout->addWidget(m_sleepTimeoutSettingsOnBattery);
    m_centralLayout->addWidget(m_passwordSettings);
    m_centralLayout->addWidget(m_notebookSettings);

    setTitle(tr("Power Management"));

    connect(m_displayNeedPassword, &SwitchWidget::checkedChanged, this, &PowerWidget::requestSetScreenBlackLock);
    connect(m_wakeNeedPassword, &SwitchWidget::checkedChanged, this, &PowerWidget::requestSetSleepLock);
    connect(m_sleepOnLidOff, &SwitchWidget::checkedChanged, this, &PowerWidget::requestSetSleepOnLidClosed);

    connect(m_monitorSleepOnPower->slider(), &DCCSlider::valueChanged, this, &PowerWidget::requestSetScreenBlackDelayOnPower);
    connect(m_monitorSleepOnBattery->slider(), &DCCSlider::valueChanged, this, &PowerWidget::requestSetScreenBlackDelayOnBattery);

    connect(m_computerSleepOnPower->slider(), &DCCSlider::valueChanged, this, &PowerWidget::requestSetSleepDelayOnPoewr);
    connect(m_computerSleepOnBattery->slider(), &DCCSlider::valueChanged, this, &PowerWidget::requestSetSleepDelayOnBattery);

#ifndef DCC_DISABLE_POWERSAVE
    connect(m_powerSaveMode, &SwitchWidget::checkedChanged, this, &PowerWidget::requestSetPowerSaveMode);
    connect(m_autoPowerSave, &SwitchWidget::checkedChanged, this, &PowerWidget::requestSetEnableAutoPSM);
#endif
}

void PowerWidget::setModel(PowerModel *const model)
{
    connect(model, &PowerModel::screenBlackLockChanged, m_displayNeedPassword, &SwitchWidget::setChecked);
    connect(model, &PowerModel::sleepLockChanged, m_wakeNeedPassword, &SwitchWidget::setChecked);
    connect(model, &PowerModel::lidPresentChanged, m_notebookSettings, &SettingsGroup::setVisible);
    connect(model, &PowerModel::sleepOnLidCloseChanged, m_sleepOnLidOff, &SwitchWidget::setChecked);

    connect(model, &PowerModel::sleepDelayChangedOnPower, this, &PowerWidget::setSleepDelayOnPower);
    connect(model, &PowerModel::sleepDelayChangedOnBattery, this, &PowerWidget::setSleepDelayOnBattery);
    connect(model, &PowerModel::screenBlackDelayChangedOnPower, this, &PowerWidget::setScreenBlackDelayOnPower);
    connect(model, &PowerModel::screenBlackDelayChangedOnBattery, this, &PowerWidget::setScreenBlackDelayOnBattery);
    connect(model, &PowerModel::haveBettaryChanged, m_sleepTimeoutSettingsOnBattery, &SettingsGroup::setVisible);

#ifndef DCC_DISABLE_POWERSAVE
    connect(model, &PowerModel::autoPowerSavingModeChanged, m_autoPowerSave, &SwitchWidget::setChecked);
    connect(model, &PowerModel::powerSaveModeChanged, m_powerSaveMode, &SwitchWidget::setChecked);
    connect(model, &PowerModel::haveBettaryChanged, m_powerSaveGrp, &SettingsGroup::setVisible);
#endif

    // init ui data
    blockSignals(true);
    m_displayNeedPassword->setChecked(model->screenBlackLock());
    m_wakeNeedPassword->setChecked(model->sleepLock());
    m_notebookSettings->setVisible(model->lidPresent());
    m_sleepOnLidOff->setChecked(model->sleepOnLidClose());
    blockSignals(false);

    m_sleepTimeoutSettingsOnBattery->setVisible(model->haveBettary());

    setScreenBlackDelayOnPower(model->screenBlackDelayOnPower());
    setScreenBlackDelayOnBattery(model->screenBlackDelayOnBattery());

    setSleepDelayOnPower(model->sleepDelayOnPower());
    setSleepDelayOnBattery(model->sleepDelayOnBattery());

#ifndef DCC_DISABLE_POWERSAVE
    m_autoPowerSave->setChecked(model->autoPowerSaveMode());
    m_powerSaveMode->setChecked(model->powerSaveMode());
    m_powerSaveGrp->setVisible(model->haveBettary());
#endif
}

QString PowerWidget::delayToLiteralString(const int delay) const
{
    switch (delay) {
    case 1:
        return tr("1 Minute");
    case 2:
        return tr("%1 Minutes").arg(5);
    case 3:
        return tr("%1 Minutes").arg(10);
    case 4:
        return tr("%1 Minutes").arg(15);
    case 5:
        return tr("%1 Minutes").arg(30);
    case 6:
        return tr("1 Hour");
    case 7:
        return tr("Never");
    default:
        return tr("%1 Minutes").arg(15);
    }
}

void PowerWidget::setScreenBlackDelayOnPower(const int delay)
{
    m_monitorSleepOnPower->slider()->blockSignals(true);
    m_monitorSleepOnPower->slider()->setValue(delay);
    m_monitorSleepOnPower->setValueLiteral(delayToLiteralString(delay));
    m_monitorSleepOnPower->slider()->blockSignals(false);
}

void PowerWidget::setScreenBlackDelayOnBattery(const int delay)
{
    m_monitorSleepOnBattery->slider()->blockSignals(true);
    m_monitorSleepOnBattery->slider()->setValue(delay);
    m_monitorSleepOnBattery->setValueLiteral(delayToLiteralString(delay));
    m_monitorSleepOnBattery->slider()->blockSignals(false);
}

void PowerWidget::setSleepDelayOnPower(const int delay)
{
    m_computerSleepOnPower->slider()->blockSignals(true);
    m_computerSleepOnPower->slider()->setValue(delay);
    m_computerSleepOnPower->setValueLiteral(delayToLiteralString(delay));
    m_computerSleepOnPower->slider()->blockSignals(false);
}

void PowerWidget::setSleepDelayOnBattery(const int delay)
{
    m_computerSleepOnBattery->slider()->blockSignals(true);
    m_computerSleepOnBattery->slider()->setValue(delay);
    m_computerSleepOnBattery->setValueLiteral(delayToLiteralString(delay));
    m_computerSleepOnBattery->slider()->blockSignals(false);
}
