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
#include "modules/power/powermodel.h"

#include <QPushButton>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::power;

UseBatteryWidget::UseBatteryWidget(QWidget *parent)
    : QWidget(parent)
    , m_layout(new QVBoxLayout)
    , m_monitorSleepOnBattery(new TitledSliderItem("Monitor will suspend after"))
    , m_computerSleepOnBattery(new TitledSliderItem("Computer will suspend after"))
    , m_autoLockScreen(new TitledSliderItem("Lock screen after"))
{
    this->setFixedWidth(400);

    m_layout->addWidget(m_monitorSleepOnBattery);
    m_layout->addWidget(m_computerSleepOnBattery);
    m_layout->addWidget(m_autoLockScreen);
    m_layout->setAlignment(Qt::AlignTop);
    m_layout->setSpacing(10);
    setLayout(m_layout);

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
}

UseBatteryWidget::~UseBatteryWidget()
{

}

void UseBatteryWidget::setModel(const PowerModel *model)
{
    connect(model, &PowerModel::sleepDelayChangedOnBattery, this, &UseBatteryWidget::setSleepDelayOnBattery);
    connect(model, &PowerModel::screenBlackDelayChangedOnBattery, this, &UseBatteryWidget::setScreenBlackDelayOnBattery);

    setScreenBlackDelayOnBattery(model->screenBlackDelayOnBattery());
    setSleepDelayOnBattery(model->sleepDelayOnBattery());
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
