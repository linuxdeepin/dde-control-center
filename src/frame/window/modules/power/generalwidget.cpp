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

    TitleLabel *powerPlansLabel = new TitleLabel("");
    //~ contents_path /power/General
    powerPlansLabel->setText(tr("Power Plans"));
    DFontSizeManager::instance()->bind(powerPlansLabel, DFontSizeManager::T5, QFont::DemiBold);
    //~ contents_path /power/General
    m_lowBatteryMode->setTitle(tr("Power Saving Mode"));
    //~ contents_path /power/General
    m_autoIntoSaveEnergyMode->setTitle(tr("Auto power saving on battery"));
    //~ contents_path /power/General
    m_wakeComputerNeedPassword->setTitle(tr("Password is required to wake up the computer"));
    //~ contents_path /power/General
    m_wakeDisplayNeedPassword->setTitle(tr("Password is required to wake up the monitor"));

    // add power plan
    m_layPowerPlan = new QVBoxLayout;
    m_powerplanListview = new DListView();

    QMap<QString, QString> powerPlanMap;
    powerPlanMap.insert("balance", tr("Balanced"));
    powerPlanMap.insert("performance", tr("High Performance"));
    powerPlanMap.insert("powersave", tr("Power Saver"));

    m_powerPlanModel = new QStandardItemModel(m_powerplanListview);
    QMap<QString, QString>::iterator iter;
    for (iter = powerPlanMap.begin(); iter != powerPlanMap.end(); ++iter) {
        DStandardItem *powerPlanItem = new DStandardItem(iter.value());
        powerPlanItem->setData(iter.key(), PowerPlanRole);
        m_powerPlanModel->appendRow(powerPlanItem);
    }
    m_powerplanListview->setModel(m_powerPlanModel);
    m_powerplanListview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_powerplanListview->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    m_powerplanListview->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    m_powerplanListview->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_powerplanListview->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_powerplanListview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_powerplanListview->setSelectionMode(QAbstractItemView::NoSelection);

    m_layPowerPlan->addWidget(powerPlansLabel);
    m_layPowerPlan->addWidget(m_powerplanListview);
    m_layout->addLayout(m_layPowerPlan);

    //add Energy Saving Mode
    m_saveEnergySettingsGrp = new SettingsGroup;
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

    m_saveEnergySettingsGrp->appendItem(m_lowBatteryMode);
    m_saveEnergySettingsGrp->appendItem(m_swLowPowerAutoIntoSaveEnergyMode);
    m_saveEnergySettingsGrp->appendItem(m_autoIntoSaveEnergyMode);
    m_saveEnergySettingsGrp->appendItem(m_sldLowerBrightness);

    //Power Saving Mode tittle
    QVBoxLayout *labelLayout = new QVBoxLayout;
    m_saveEnergySettingsLabel = new TitleLabel(tr("Power Saving Settings"));
    DFontSizeManager::instance()->bind(m_saveEnergySettingsLabel, DFontSizeManager::T5, QFont::DemiBold);
    labelLayout->addWidget(m_saveEnergySettingsLabel);
    labelLayout->setContentsMargins(6,0,0,0);
    m_layEnergySavingMode->addLayout(labelLayout);
    m_layEnergySavingMode->addWidget(m_saveEnergySettingsGrp);
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

    TitleLabel *label = new TitleLabel(tr("Wakeup Settings"));
    DFontSizeManager::instance()->bind(label, DFontSizeManager::T5, QFont::DemiBold);

    QVBoxLayout *wakeupLayout = new QVBoxLayout;
    wakeupLayout->addWidget(label);
    wakeupLayout->setContentsMargins(8,0,0,0);
    m_layout->addLayout(wakeupLayout);

    m_layout->addWidget(generalSettingsGrp);
    m_layout->setSpacing(List_Interval);
    m_layout->setMargin(0);
    m_layout->setAlignment(Qt::AlignTop);
    m_layout->setContentsMargins(12,0,12,0);
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
    connect(m_powerplanListview, &DListView::clicked, this, &GeneralWidget::onPowerPlanChanged);
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
    connect(model, &PowerModel::suspendChanged, m_wakeComputerNeedPassword, &SwitchWidget::setVisible);

    connect(model, &PowerModel::highPerformaceChanged, this, &GeneralWidget::onHighPerformanceSupportChanged);
    onHighPerformanceSupportChanged(model->isHighPerformanceSupported());

    connect(model, &PowerModel::powerPlanChanged, this, &GeneralWidget::onCurPowerPlanChanged);
    onCurPowerPlanChanged(model->getPowerPlan());

    // init ui data
    blockSignals(true);
    m_wakeDisplayNeedPassword->setChecked(model->screenBlackLock());
    m_wakeComputerNeedPassword->setChecked(model->sleepLock());
    blockSignals(false);

#ifndef DCC_DISABLE_POWERSAVE
    m_autoIntoSaveEnergyMode->setChecked(model->autoPowerSaveMode());
    m_lowBatteryMode->setChecked(model->powerSaveMode());
#endif

    m_wakeComputerNeedPassword->setVisible(model->canSleep() && model->getSuspend());

    //-------------sp2 add-------------------------
    m_swLowPowerAutoIntoSaveEnergyMode->setChecked(model->powerSavingModeAutoWhenQuantifyLow());
    connect(model, &PowerModel::powerSavingModeAutoWhenQuantifyLowChanged, m_swLowPowerAutoIntoSaveEnergyMode, &SwitchWidget::setChecked);

    QDBusInterface Interface("com.deepin.daemon.Display",
                             "/com/deepin/daemon/Display",
                             "com.deepin.daemon.Display",
                             QDBusConnection::sessionBus());
    int maxBacklight = Interface.property("MaxBacklightBrightness").toInt();
    m_sldLowerBrightness->setVisible(maxBacklight >= 100);
    m_sldLowerBrightness->slider()->setValue(model->powerSavingModeLowerBrightnessThreshold() / 10);
    connect(model, &PowerModel::powerSavingModeLowerBrightnessThresholdChanged, this,  [ = ](const uint dLevel) {
        m_sldLowerBrightness->slider()->setValue(dLevel / 10);
    });

    bool bStatus = model->haveBettary();
    m_saveEnergySettingsGrp->setVisible(true);
    m_saveEnergySettingsLabel->setVisible(true);

    m_lowBatteryMode->setVisible(bStatus);
    m_swLowPowerAutoIntoSaveEnergyMode->setVisible(bStatus);
    m_autoIntoSaveEnergyMode->setVisible(bStatus);
    m_sldLowerBrightness->setVisible(true);

    connect(model, &PowerModel::haveBettaryChanged, this, &GeneralWidget::onBatteryChanged);

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

void GeneralWidget::onBatteryChanged(const bool &state)
{
    m_lowBatteryMode->setVisible(state);
    m_swLowPowerAutoIntoSaveEnergyMode->setVisible(state);
    m_autoIntoSaveEnergyMode->setVisible(state);
}

void GeneralWidget::onPowerPlanChanged(const QModelIndex &index)
{
    QStandardItem *item = m_powerPlanModel->item(index.row(), index.column());
    QString selectedPowerplan = item->data(PowerPlanRole).toString();
    Q_EMIT requestSetPowerPlan(selectedPowerplan);
}

void GeneralWidget::onCurPowerPlanChanged(const QString &curPowerPlan)
{
    int row_count = m_powerPlanModel->rowCount();
    for (int i = 0; i < row_count; ++i) {
        QStandardItem *items = m_powerPlanModel->item(i, 0);
        if (items->data(PowerPlanRole).toString() == curPowerPlan) {
            items->setCheckState(Qt::Checked);
        } else {
            items->setCheckState(Qt::Unchecked);
        }
    }
}

void GeneralWidget::onHighPerformanceSupportChanged(const bool isSupport)
{
    const QString highPerform = "performance";
    int row_count = m_powerPlanModel->rowCount();
    if (!isSupport) {
        for (int i = 0; i < row_count; ++i) {
            QStandardItem *items = m_powerPlanModel->item(i, 0);
            if (items->data(PowerPlanRole).toString() == highPerform) {
                m_powerPlanModel->removeRow(i);
                break;
            }
        }
    } else {
        bool findHighPerform = false;
        for (int i = 0; i < row_count; ++i) {
            QStandardItem *items = m_powerPlanModel->item(i, 0);
            if (items->data(PowerPlanRole).toString() == highPerform) {
                findHighPerform = true;
                break;
            }
        }
        if (!findHighPerform) {
            DStandardItem *powerPlanItem = new DStandardItem(highPerform);
            powerPlanItem->setData(highPerform, PowerPlanRole);
            m_powerPlanModel->appendRow(powerPlanItem);
            m_powerPlanModel->insertRow(1, powerPlanItem);
        }
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
