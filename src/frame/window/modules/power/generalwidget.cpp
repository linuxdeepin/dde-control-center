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
#include "generalwidget.h"
#include "widgets/switchwidget.h"
#include "widgets/labels/normallabel.h"
#include "modules/power/powermodel.h"

#include <QLabel>
#include <QListView>
#include <QStandardItemModel>
#include <QFont>

using namespace dcc::widgets;
using namespace dcc::power;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::power;

GeneralWidget::GeneralWidget(QWidget *parent)
    : QWidget(parent)
    , m_layout(new QVBoxLayout)
    //~ contents_path /power/General
    , m_lowBatteryMode(new SwitchWidget(tr("Power Saving Mode")))
    //~ contents_path /power/General
    , m_autoIntoSaveEnergyMode(new SwitchWidget(tr("Auto Mode Switch")))
    //~ contents_path /power/General
    , m_wakeComputerNeedPassword(new SwitchWidget(tr("Password is required to wake up the computer")))
    //~ contents_path /power/General
    , m_wakeDisplayNeedPassword(new SwitchWidget(tr("Password is required to wake up the monitor")))
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_layout->addWidget(m_lowBatteryMode);
    m_layout->setSpacing(0);
    m_layout->addWidget(m_autoIntoSaveEnergyMode);
    m_layout->addWidget(m_wakeComputerNeedPassword);
    m_layout->addWidget(m_wakeDisplayNeedPassword);
    m_layout->setAlignment(Qt::AlignTop);
    m_layout->setSpacing(5);
    setLayout(m_layout);

    connect(m_lowBatteryMode, &SwitchWidget::checkedChanged, this, &GeneralWidget::requestSetLowBatteryMode);
    connect(m_autoIntoSaveEnergyMode, &SwitchWidget::checkedChanged, this, &GeneralWidget::requestSetAutoIntoSaveEnergyMode);
    connect(m_wakeComputerNeedPassword, &SwitchWidget::checkedChanged, this, &GeneralWidget::requestSetWakeComputer);
    connect(m_wakeDisplayNeedPassword, &SwitchWidget::checkedChanged, this, &GeneralWidget::requestSetWakeDisplay);
}

GeneralWidget::~GeneralWidget()
{

}

void GeneralWidget::setModel(const PowerModel *model)
{
    connect(model, &PowerModel::screenBlackLockChanged, m_wakeDisplayNeedPassword, &SwitchWidget::setChecked);
    connect(model, &PowerModel::sleepLockChanged, m_wakeComputerNeedPassword, &SwitchWidget::setChecked);

#ifndef DCC_DISABLE_POWERSAVE
    connect(model, &PowerModel::autoPowerSavingModeChanged, m_autoIntoSaveEnergyMode, &SwitchWidget::setChecked);
    connect(model, &PowerModel::powerSaveModeChanged, m_lowBatteryMode, &SwitchWidget::setChecked);
#endif

    // init ui data
    blockSignals(true);
    m_wakeDisplayNeedPassword->setChecked(model->screenBlackLock());
    m_wakeComputerNeedPassword->setChecked(model->sleepLock());
    blockSignals(false);

#ifndef DCC_DISABLE_POWERSAVE
    m_autoIntoSaveEnergyMode->setChecked(model->autoPowerSaveMode());
    m_lowBatteryMode->setChecked(model->powerSaveMode());
#endif
}
