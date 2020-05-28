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
#include "widgets/powerdisplaywidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/labels/normallabel.h"
#include "modules/power/powermodel.h"
#include "window/utils.h"

#include <QLabel>
#include <QListView>
#include <QStandardItemModel>
#include <QFont>
#include <QGSettings>
#include <QDBusInterface>

#include <DFontSizeManager>

DWIDGET_USE_NAMESPACE

using namespace dcc::widgets;
using namespace dcc::power;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::power;

static QGSettings *GSettings()
{
    static QGSettings settings("com.deepin.dde.dock.module.power");
    return &settings;
}

GeneralWidget::GeneralWidget(QWidget *parent, bool bIsBattery)
    : QWidget(parent)
    , m_layout(new QVBoxLayout)
    , m_lowBatteryMode(new SwitchWidget(tr("Power Saving Mode")))
    , m_autoIntoSaveEnergyMode(new SwitchWidget(tr("Auto Mode Switch")))
    , m_wakeComputerNeedPassword(new SwitchWidget(tr("Password is required to wake up the computer")))
    , m_wakeDisplayNeedPassword(new SwitchWidget(tr("Password is required to wake up the monitor")))
    , m_titleWidget(new QLabel(tr("Battery")))
    , m_powerShowTimeToFull(new SwitchWidget(tr("Display capacity and remaining charging time")))
    , m_ShowTimeToFullTips(new PowerDisplayWidget(this))
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    SettingsGroup *generalSettingsGrp = new SettingsGroup;
    generalSettingsGrp->setSpacing(List_Interval);

    //~ contents_path /power/General
    m_lowBatteryMode->setTitle(tr("Power Saving Mode"));
    //~ contents_path /power/General
    m_autoIntoSaveEnergyMode->setTitle(tr("Auto Mode Switch"));
    //~ contents_path /power/General
    m_wakeComputerNeedPassword->setTitle(tr("Password is required to wake up the computer"));
    //~ contents_path /power/General
    m_wakeDisplayNeedPassword->setTitle(tr("Password is required to wake up the monitor"));
    m_powerShowTimeToFull->setTitle(tr("Display capacity and remaining charging time"));
    m_ShowTimeToFullTips->setTitle(tr("Maximum capacity"));

    QDBusInterface inter("com.deepin.system.Power", "/com/deepin/system/Power", "com.deepin.system.Power", QDBusConnection::systemBus());
    double capacity = inter.property("BatteryCapacity").toDouble();
    m_ShowTimeToFullTips->setText("%" + QString::number(int(capacity)));

    DFontSizeManager::instance()->bind(m_titleWidget, DFontSizeManager::T4, 75);
    m_titleWidget->setMargin(5);

    generalSettingsGrp->appendItem(m_lowBatteryMode);
    generalSettingsGrp->appendItem(m_autoIntoSaveEnergyMode);
    generalSettingsGrp->appendItem(m_wakeComputerNeedPassword);
    generalSettingsGrp->appendItem(m_wakeDisplayNeedPassword);
    generalSettingsGrp->insertWidget(m_titleWidget);
    generalSettingsGrp->appendItem(m_powerShowTimeToFull);
    generalSettingsGrp->appendItem(m_ShowTimeToFullTips);

    m_titleWidget->setVisible(bIsBattery);
    m_powerShowTimeToFull->setVisible(bIsBattery);
    m_ShowTimeToFullTips->setVisible(bIsBattery);
    m_lowBatteryMode->setVisible(bIsBattery);
    m_autoIntoSaveEnergyMode->setVisible(bIsBattery);

    m_layout->addWidget(generalSettingsGrp);
    m_layout->setMargin(0);
    m_layout->setAlignment(Qt::AlignTop);
    m_layout->setSpacing(5);
    setLayout(m_layout);

    connect(m_lowBatteryMode, &SwitchWidget::checkedChanged, this, &GeneralWidget::requestSetLowBatteryMode);
    connect(m_autoIntoSaveEnergyMode, &SwitchWidget::checkedChanged, this, &GeneralWidget::requestSetAutoIntoSaveEnergyMode);
    connect(m_wakeComputerNeedPassword, &SwitchWidget::checkedChanged, this, &GeneralWidget::requestSetWakeComputer);
    connect(m_wakeDisplayNeedPassword, &SwitchWidget::checkedChanged, this, &GeneralWidget::requestSetWakeDisplay);
    connect(m_powerShowTimeToFull, &SwitchWidget::checkedChanged, this, &GeneralWidget::setPowerDisplay);
    connect(GSettings(), &QGSettings::changed, this, &GeneralWidget::onGSettingsChanged);

    onGSettingsChanged("showtimetofull");
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

    m_wakeComputerNeedPassword->setVisible(model->canSleep());
}

void GeneralWidget::setPowerDisplay(const bool &state)
{
    if (GSettings()->keys().contains("showtimetofull")) {
        blockSignals(true);
        const bool isEnable = GSettings()->keys().contains("showtimetofull") && GSettings()->get("showtimetofull").toBool();
        bool gstate = isEnable && GSettings()->get("showtimetofull").toBool();
        if (gstate != state)
            GSettings()->set("showtimetofull", state);
        blockSignals(false);
    }
}

void GeneralWidget::onGSettingsChanged(const QString &key)
{
    if (key != "showtimetofull") {
        return;
    }

    if (GSettings()->keys().contains("showtimetofull")) {
        const bool isEnable = GSettings()->keys().contains("showtimetofull") && GSettings()->get("showtimetofull").toBool();
        bool state = isEnable && GSettings()->get("showtimetofull").toBool();
        m_powerShowTimeToFull->setChecked(state);
    }
}
