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
#include "generalmodule.h"
#include "widgetmodule.h"
#include "powermodel.h"
#include "powerworker.h"
#include "titlemodule.h"
#include "widgets/titledslideritem.h"
#include "widgets/dccslider.h"
#include "widgets/settingsgroup.h"
#include "widgets/switchwidget.h"
#include "widgets/titlevalueitem.h"

#include <DListView>

#define BALANCE "balance"         // 平衡模式
#define PERFORMANCE "performance" // 高性能模式
#define POWERSAVE "powersave"     // 节能模式

DCC_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

GeneralModule::GeneralModule(PowerModel *model, PowerWorker *work, QObject *parent)
    : ModuleObject("power::general", tr("General"), QIcon::fromTheme("dcc_general_purpose"), parent)
    , m_powerplanListview(nullptr)
    , m_model(model)
    , m_work(work)
    , m_wakeupSettings(nullptr)
    , m_batterySettingsTitle(nullptr)
    , m_batterySettings(nullptr)
{
    deactive();
    setChildType(ModuleObject::ChildType::Page);
    m_powerPlanMap.insert(BALANCE, tr("Balanced"));
    m_powerPlanMap.insert(PERFORMANCE, tr("High Performance"));
    m_powerPlanMap.insert(POWERSAVE, tr("Power Saver"));

    connect(this, &GeneralModule::requestSetWakeDisplay, m_work, &PowerWorker::setScreenBlackLock);
    connect(this, &GeneralModule::requestSetWakeComputer, m_work, &PowerWorker::setSleepLock);

#ifndef DCC_DISABLE_POWERSAVE
    connect(this, &GeneralModule::requestSetPowerSavingModeAuto, m_work, &PowerWorker::setAutoEnablePowerSave);
    connect(this, &GeneralModule::requestSetLowBatteryMode, m_work, &PowerWorker::setEnablePowerSave);
    connect(this, &GeneralModule::requestSetPowerSaveMode, m_work, &PowerWorker::setEnablePowerSave);
#endif

    connect(this, &GeneralModule::requestSetPowerSavingModeAutoWhenQuantifyLow, m_work, &PowerWorker::setPowerSavingModeAutoWhenQuantifyLow);
    connect(this, &GeneralModule::requestSetPowerSavingModeAuto, m_work, &PowerWorker::setPowerSavingModeAuto);
    connect(this, &GeneralModule::requestSetPowerSavingModeLowerBrightnessThreshold, m_work, &PowerWorker::setPowerSavingModeLowerBrightnessThreshold);
    connect(this, &GeneralModule::requestSetPowerPlan, m_work, &PowerWorker::setPowerPlan);

    m_powerPlanModel = new QStandardItemModel(this);
    QMap<QString, QString>::iterator iter;
    for (iter = m_powerPlanMap.begin(); iter != m_powerPlanMap.end(); ++iter) {
        DStandardItem *powerPlanItem = new DStandardItem(iter.value());
        powerPlanItem->setData(iter.key(), PowerPlanRole);
        m_powerPlanModel->appendRow(powerPlanItem);
    }

    //　性能设置
    appendChild(new TitleModule("powerPlansLabel", tr("Power Plans")));
    appendChild(new WidgetModule<DListView>("powerPlans", tr("Power Plans"), this, &GeneralModule::initPowerPlans));
    //　节能设置
    appendChild(new TitleModule("powerLowerBrightnessLabel", tr("Power Saving Settings")));
    appendChild(new WidgetModule<SettingsGroup>("powerLowerBrightness", tr("Power Saving Settings"), this, &GeneralModule::initPowerSavingSettings));
    // 唤醒设置
    appendChild(new TitleModule("WakeupSettingsLabel", tr("Wakeup Settings")));
    m_wakeupSettings = new WidgetModule<SettingsGroup>("WakeupSettings", tr("Wakeup Settings"), this, &GeneralModule::initWakeupSettings);
    appendChild(m_wakeupSettings);
    // 电池设置
    onBatteryChanged(m_model->haveBettary());

    setModel(m_model);
}

GeneralModule::~GeneralModule()
{
}

void GeneralModule::deactive()
{
    m_powerplanListview = nullptr;
    m_sldLowerBrightness = nullptr;
}

void GeneralModule::setModel(PowerModel *model)
{
    m_model = model;
    connect(m_model, &PowerModel::highPerformaceSupportChanged, this, &GeneralModule::onHighPerformanceSupportChanged);
    connect(m_model, &PowerModel::haveBettaryChanged, this, &GeneralModule::onBatteryChanged);

    connect(m_model, &PowerModel::powerPlanChanged, this, &GeneralModule::onCurPowerPlanChanged);
    onCurPowerPlanChanged(m_model->getPowerPlan());
}
void GeneralModule::initPowerPlans(Dtk::Widget::DListView *view)
{
    m_powerplanListview = view;
    m_powerplanListview->setAccessibleName("powerplanListview");

    m_powerplanListview->setModel(m_powerPlanModel);
    m_powerplanListview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_powerplanListview->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    m_powerplanListview->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_powerplanListview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_powerplanListview->setSelectionMode(QAbstractItemView::NoSelection);

    onHighPerformanceSupportChanged(m_model->isHighPerformanceSupported());

    connect(m_powerplanListview, &DListView::clicked, this, &GeneralModule::onPowerPlanChanged);
}

void GeneralModule::initPowerSavingSettings(SettingsGroup *energySavingGrp)
{
    SwitchWidget *m_swLowPowerAutoIntoSaveEnergyMode = new SwitchWidget(tr("Auto power saving on low battery"), energySavingGrp);
    SwitchWidget *m_autoIntoSaveEnergyMode = new SwitchWidget(tr("Auto power saving on battery"), energySavingGrp);
    m_sldLowerBrightness = new TitledSliderItem(tr("Decrease brightness"), energySavingGrp);
    QStringList annotions;
    annotions << "10%"
              << "20%"
              << "30%"
              << "40%";
    m_sldLowerBrightness->slider()->setAccessibleName("sldLowerBrightness");
    m_sldLowerBrightness->slider()->setAnnotations(annotions);
    m_sldLowerBrightness->slider()->setRange(1, 4);
    m_sldLowerBrightness->slider()->setPageStep(10);
    m_sldLowerBrightness->slider()->setType(DCCSlider::Vernier);
    m_sldLowerBrightness->slider()->setTickPosition(QSlider::NoTicks);

    m_swLowPowerAutoIntoSaveEnergyMode->setChecked(m_model->powerSavingModeAutoWhenQuantifyLow());
    connect(m_model, &PowerModel::powerSavingModeAutoWhenQuantifyLowChanged, m_swLowPowerAutoIntoSaveEnergyMode, &SwitchWidget::setChecked);

    int maxBacklight = m_work->getMaxBacklightBrightness();
    m_sldLowerBrightness->setVisible(maxBacklight >= 100 || maxBacklight == 0);
    m_sldLowerBrightness->slider()->setValue(m_model->powerSavingModeLowerBrightnessThreshold() / 10);
    connect(m_model, &PowerModel::powerSavingModeLowerBrightnessThresholdChanged, this, [this](const uint dLevel) {
        m_sldLowerBrightness->slider()->blockSignals(true);
        m_sldLowerBrightness->slider()->setValue(dLevel / 10);
        m_sldLowerBrightness->slider()->blockSignals(false);
    });

    bool bStatus = m_model->haveBettary();
    m_swLowPowerAutoIntoSaveEnergyMode->setVisible(bStatus);
    m_autoIntoSaveEnergyMode->setVisible(bStatus);
#ifndef DCC_DISABLE_POWERSAVE
    m_autoIntoSaveEnergyMode->setChecked(m_model->autoPowerSaveMode());

    connect(m_model, &PowerModel::autoPowerSavingModeChanged, m_autoIntoSaveEnergyMode, &SwitchWidget::setChecked);
#endif
    connect(m_model, &PowerModel::haveBettaryChanged, m_swLowPowerAutoIntoSaveEnergyMode, &SwitchWidget::setVisible);
    connect(m_model, &PowerModel::haveBettaryChanged, m_autoIntoSaveEnergyMode, &SwitchWidget::setVisible);
    auto slotfunc1 = [=](int pos) {
        //滑块位置改变时，发送设置音量的信号
        Q_EMIT requestSetPowerSavingModeLowerBrightnessThreshold(pos * 10);
    };
    //当点击滑槽时不会有，sliderMoved消息，用这个补
    connect(m_sldLowerBrightness->slider(), &DCCSlider::valueChanged, slotfunc1);
    //滑块移动消息处理
    connect(m_sldLowerBrightness->slider(), &DCCSlider::sliderMoved, slotfunc1);

    energySavingGrp->appendItem(m_swLowPowerAutoIntoSaveEnergyMode);
    energySavingGrp->appendItem(m_autoIntoSaveEnergyMode);
    energySavingGrp->appendItem(m_sldLowerBrightness);
}

void GeneralModule::initWakeupSettings(SettingsGroup *wakeupSettingsGrp)
{
    SwitchWidget *m_wakeComputerNeedPassword = new SwitchWidget(tr("Password is required to wake up the computer"), wakeupSettingsGrp);
    SwitchWidget *m_wakeDisplayNeedPassword = new SwitchWidget(tr("Password is required to wake up the monitor"), wakeupSettingsGrp);

    m_wakeDisplayNeedPassword->setChecked(m_model->screenBlackLock());
    m_wakeComputerNeedPassword->setChecked(m_model->sleepLock());
    m_wakeComputerNeedPassword->setVisible(m_model->canSuspend() && m_model->getSuspend()); // 配置显示

    connect(m_model, &PowerModel::screenBlackLockChanged, m_wakeDisplayNeedPassword, &SwitchWidget::setChecked);
    connect(m_model, &PowerModel::sleepLockChanged, m_wakeComputerNeedPassword, &SwitchWidget::setChecked);
    connect(m_model, &PowerModel::suspendChanged, m_wakeComputerNeedPassword, &SwitchWidget::setVisible);

    connect(m_wakeComputerNeedPassword, &SwitchWidget::checkedChanged, this, &GeneralModule::requestSetWakeComputer);
    connect(m_wakeDisplayNeedPassword, &SwitchWidget::checkedChanged, this, &GeneralModule::requestSetWakeDisplay);

    wakeupSettingsGrp->appendItem(m_wakeComputerNeedPassword);
    wakeupSettingsGrp->appendItem(m_wakeDisplayNeedPassword);
}

void GeneralModule::initBatterySettings(SettingsGroup *batterySettingsGrp)
{
    SwitchWidget *m_powerShowTimeToFull = new SwitchWidget(tr("Display remaining using and charging time"), batterySettingsGrp);
    TitleValueItem *m_ShowTimeToFullTips = new TitleValueItem(batterySettingsGrp);
    m_ShowTimeToFullTips->setTitle(tr("Maximum capacity"));
    m_ShowTimeToFullTips->setValue(QString::number(int(m_work->getBatteryCapacity())) + "%");

    connect(m_powerShowTimeToFull, &SwitchWidget::checkedChanged, this, &GeneralModule::setPowerDisplay);
    connect(m_model, &PowerModel::haveBettaryChanged, m_powerShowTimeToFull, &SwitchWidget::setVisible);
    connect(m_model, &PowerModel::haveBettaryChanged, m_ShowTimeToFullTips, &TitleValueItem::setVisible);

    batterySettingsGrp->appendItem(m_powerShowTimeToFull);
    batterySettingsGrp->appendItem(m_ShowTimeToFullTips);
}

void GeneralModule::setPowerDisplay(const bool &state)
{ // 保存设置值
    //        if (GSettings()->keys().contains("showtimetofull")) {
    //            blockSignals(true);
    //            const bool isEnable = GSettings()->keys().contains("showtimetofull") && GSettings()->get("showtimetofull").toBool();
    //            bool gstate = isEnable && GSettings()->get("showtimetofull").toBool();
    //            if (gstate != state)
    //                GSettings()->set("showtimetofull", state);
    //            blockSignals(false);
    //        }
}

void GeneralModule::onPowerPlanChanged(const QModelIndex &index)
{
    QStandardItem *item = m_powerPlanModel->item(index.row(), index.column());
    QString selectedPowerplan = item->data(PowerPlanRole).toString();
    Q_EMIT requestSetPowerPlan(selectedPowerplan);
}

void GeneralModule::onCurPowerPlanChanged(const QString &curPowerPlan)
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

void GeneralModule::onHighPerformanceSupportChanged(const bool isSupport)
{
    if (!m_powerplanListview)
        return;
    int row_count = m_powerPlanModel->rowCount();
    if (!isSupport) {
        int cur_place = m_powerplanListview->currentIndex().row();
        for (int i = 0; i < row_count; ++i) {
            QStandardItem *items = m_powerPlanModel->item(i, 0);
            if (items->data(PowerPlanRole).toString() == PERFORMANCE) {
                m_powerPlanModel->removeRow(i);

                if (cur_place == i || cur_place < 0) {
                    m_powerplanListview->clicked(m_powerPlanModel->index(0, 0));
                }
                break;
            }
        }
    } else {
        bool findHighPerform = false;
        for (int i = 0; i < row_count; ++i) {
            QStandardItem *items = m_powerPlanModel->item(i, 0);
            if (items->data(PowerPlanRole).toString() == PERFORMANCE) {
                findHighPerform = true;
                break;
            }
        }
        if (!findHighPerform) {
            DStandardItem *powerPlanItem = new DStandardItem(m_powerPlanMap.value(PERFORMANCE));
            powerPlanItem->setData(PERFORMANCE, PowerPlanRole);
            m_powerPlanModel->insertRow(1, powerPlanItem);
        }
    }
}

void GeneralModule::onBatteryChanged(const bool &state)
{
    if (state) {
        const QList<ModuleObject *> &childrens = ModuleObject::childrens();
        m_batterySettingsTitle = new TitleModule("batterySettings", tr("Battery"));
        m_batterySettings = new WidgetModule<SettingsGroup>("batterySettings", tr("Battery"), this, &GeneralModule::initBatterySettings);
        insertChild(childrens.indexOf(m_wakeupSettings) + 1, m_batterySettingsTitle);
        insertChild(childrens.indexOf(m_batterySettingsTitle) + 1, m_batterySettings);
    } else if (m_batterySettingsTitle) {
        removeChild(m_batterySettingsTitle);
        m_batterySettingsTitle->deleteLater();
        m_batterySettingsTitle = nullptr;
        removeChild(m_batterySettings);
        m_batterySettings->deleteLater();
        m_batterySettings = nullptr;
    }
}
