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
#include "widgets/labels/normallabel.h"
#include "widgets/settingsgroup.h"
#include "modules/power/powermodel.h"
#include "window/utils.h"
#include "widgets/titledslideritem.h"
#include "widgets/titlelabel.h"
#include "widgets/titlevalueitem.h"
#include "widgets/contentwidget.h"

#include <DFontSizeManager>

#include <QLabel>
#include <QListView>
#include <QStandardItemModel>
#include <QFont>
#include <QScrollArea>
#include <QGSettings>
#include <QDBusInterface>

using namespace dcc;
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
    , m_swLowPowerAutoIntoSaveEnergyMode(new SwitchWidget(tr("Auto power saving on low battery")))
    , m_showBatteryCapacity(new SwitchWidget(tr("Show battery capacity")))
    , m_batteryCapacity(new TitleValueItem)
    , m_lowBatteryMode(new SwitchWidget(tr("Power Saving Mode")))
    , m_autoIntoSaveEnergyMode(new SwitchWidget(tr("Auto power saving on battery")))
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
    m_autoIntoSaveEnergyMode->setTitle(tr("Auto power saving on battery"));
    //~ contents_path /power/General
    m_wakeComputerNeedPassword->setTitle(tr("Password is required to wake up the computer"));
    //~ contents_path /power/General
    m_wakeDisplayNeedPassword->setTitle(tr("Password is required to wake up the monitor"));


    //add Energy Saving Mode
    SettingsGroup *saveEnergySettingsGrp = new SettingsGroup;
    generalSettingsGrp->setSpacing(List_Interval);


    m_layEnergySavingMode = new QVBoxLayout;
    m_sldLowerBrightness = new TitledSliderItem(tr("Decrease brightness"), this);
    m_sldLowerBrightness->addBackground();

    auto reduceSlider = m_sldLowerBrightness->slider();
    QStringList annotions;

    annotions << "10%" << "20%" << "30%" << "40%";
    reduceSlider->setAnnotations(annotions);
    reduceSlider->setRange(1, 4);
    reduceSlider->setPageStep(10);
    reduceSlider->setType(DCCSlider::Vernier);
    reduceSlider->setTickPosition(QSlider::NoTicks);

    saveEnergySettingsGrp->appendItem(m_lowBatteryMode);
    saveEnergySettingsGrp->appendItem(m_swLowPowerAutoIntoSaveEnergyMode);
    saveEnergySettingsGrp->appendItem(m_autoIntoSaveEnergyMode);
    saveEnergySettingsGrp->appendItem(m_sldLowerBrightness);

    //Power Saving Mode tittle
    TitleLabel *label = new TitleLabel(tr("Power Saving Settings"));
    DFontSizeManager::instance()->bind(label, DFontSizeManager::T5, QFont::DemiBold);
    m_layEnergySavingMode->addWidget(label);
    m_layEnergySavingMode->addWidget(saveEnergySettingsGrp);
    //---------------------------------------------------------

    //add battery info
    m_powerShowTimeToFull->setTitle(tr("Display capacity and remaining charging time"));
    m_ShowTimeToFullTips->setTitle(tr("Maximum capacity"));

    QDBusInterface inter("com.deepin.system.Power", "/com/deepin/system/Power", "com.deepin.system.Power", QDBusConnection::systemBus());

    double capacity = inter.property("BatteryCapacity").toDouble();

    m_ShowTimeToFullTips->setText(QString::number(int(capacity)) + "%");

    DFontSizeManager::instance()->bind(m_titleWidget, DFontSizeManager::T5, QFont::DemiBold);
    m_titleWidget->setMargin(5);

    //-------------------------------------------------------


    generalSettingsGrp->appendItem(m_wakeComputerNeedPassword);
    generalSettingsGrp->appendItem(m_wakeDisplayNeedPassword);
    generalSettingsGrp->insertWidget(m_titleWidget);
    generalSettingsGrp->appendItem(m_powerShowTimeToFull);
    generalSettingsGrp->appendItem(m_ShowTimeToFullTips);

    m_titleWidget->setVisible(bIsBattery);
    m_powerShowTimeToFull->setVisible(bIsBattery);
    m_ShowTimeToFullTips->setVisible(bIsBattery);

    m_layout->addLayout(m_layEnergySavingMode);

    label = new TitleLabel(tr("Wakeup Settings"));
    DFontSizeManager::instance()->bind(label, DFontSizeManager::T5, QFont::DemiBold);

    m_layout->addWidget(label);

    m_layout->addWidget(generalSettingsGrp);
    m_layout->setSpacing(List_Interval);
    m_layout->setMargin(0);
    m_layout->setAlignment(Qt::AlignTop);
//    setLayout(m_layout);
    ContentWidget *contentWgt = new ContentWidget;
    QWidget *mainWgt = new TranslucentFrame;
    mainWgt->setLayout(m_layout);
    contentWgt->setContent(mainWgt);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->addWidget(contentWgt);
    setLayout(mainLayout);

    connect(m_lowBatteryMode, &SwitchWidget::checkedChanged, this, &GeneralWidget::requestSetPowerSaveMode);
    connect(m_swLowPowerAutoIntoSaveEnergyMode, &SwitchWidget::checkedChanged, this, &GeneralWidget::requestSetPowerSavingModeAutoWhenQuantifyLow);
    connect(m_autoIntoSaveEnergyMode, &SwitchWidget::checkedChanged, this, &GeneralWidget::requestSetPowerSavingModeAuto);
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

    //-------------sp2 add-------------------------
    m_swLowPowerAutoIntoSaveEnergyMode->setChecked(model->powerSavingModeAutoWhenQuantifyLow());
    connect(model, &PowerModel::powerSavingModeAutoWhenQuantifyLowChanged, m_swLowPowerAutoIntoSaveEnergyMode, &SwitchWidget::setChecked);

    m_sldLowerBrightness->slider()->setValue(model->powerSavingModeLowerBrightnessThreshold() / 10);
    connect(model, &PowerModel::powerSavingModeLowerBrightnessThresholdChanged, this,  [ = ](const int dLevel) {
        m_sldLowerBrightness->slider()->setValue(dLevel / 10);
    });

    //---------------------------------------------
    initSlider();
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

void GeneralWidget::initSlider()
{
    auto slotfunc1 = [ = ](int pos) {

        //滑块位置改变时，发送设置音量的信号
        Q_EMIT requestSetPowerSavingModeLowerBrightnessThreshold(pos * 10);
    };
    //当点击滑槽时不会有，sliderMoved消息，用这个补
    connect(m_sldLowerBrightness->slider(), &DCCSlider::valueChanged, slotfunc1);
    //滑块移动消息处理
    connect(m_sldLowerBrightness->slider(), &DCCSlider::sliderMoved, slotfunc1);
}
