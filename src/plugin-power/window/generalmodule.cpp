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
#include "itemmodule.h"
#include "powermodel.h"
#include "powerworker.h"
#include "titlemodule.h"
#include "settingsgroupmodule.h"
#include "widgets/switchwidget.h"
#include "widgets/dcclistview.h"

#include <DComboBox>
#include <DListView>
#include <DSwitchButton>

#define BALANCE "balance"         // 平衡模式
#define PERFORMANCE "performance" // 高性能模式
#define POWERSAVE "powersave"     // 节能模式

DCC_USE_NAMESPACE
    DWIDGET_USE_NAMESPACE

    GeneralModule::GeneralModule(PowerModel *model, PowerWorker *work, QObject *parent)
    : PageModule("general", tr("General"), QIcon::fromTheme("dcc_general_purpose"), parent)
    , m_model(model)
    , m_work(work)
{
    m_powerPlanMap.insert(BALANCE, tr("Balanced"));
    m_powerPlanMap.insert(PERFORMANCE, tr("High Performance"));
    m_powerPlanMap.insert(POWERSAVE, tr("Power Saver"));

    connect(this, &GeneralModule::requestSetWakeDisplay, m_work, &PowerWorker::setScreenBlackLock);
    connect(this, &GeneralModule::requestSetWakeComputer, m_work, &PowerWorker::setSleepLock);

    connect(this, &GeneralModule::requestSetLowBatteryMode, m_work, &PowerWorker::setEnablePowerSave);
    connect(this, &GeneralModule::requestSetPowerSaveMode, m_work, &PowerWorker::setEnablePowerSave);

    connect(this, &GeneralModule::requestSetPowerSavingModeAutoWhenQuantifyLow, m_work, &PowerWorker::setPowerSavingModeAutoWhenQuantifyLow);
    connect(this, &GeneralModule::requestSetPowerSavingModeAuto, m_work, &PowerWorker::setPowerSavingModeAuto);
    connect(this, &GeneralModule::requestSetPowerSavingModeLowerBrightnessThreshold, m_work, &PowerWorker::setPowerSavingModeLowerBrightnessThreshold);
    connect(this, &GeneralModule::requestSetPowerPlan, m_work, &PowerWorker::setPowerPlan);
    initUI();
}

GeneralModule::~GeneralModule()
{
}

void GeneralModule::deactive()
{
}

void GeneralModule::initUI()
{
    m_powerPlanModel = new QStandardItemModel(this);
    QMap<QString, QString>::iterator iter;
    for (iter = m_powerPlanMap.begin(); iter != m_powerPlanMap.end(); ++iter) {
        DStandardItem *powerPlanItem = new DStandardItem(iter.value());
        powerPlanItem->setData(iter.key(), PowerPlanRole);
        m_powerPlanModel->appendRow(powerPlanItem);
    }

    //　性能设置
    appendChild(new TitleModule("powerPlansLabel", tr("Power Plans")));
    appendChild(new ItemModule("powerPlans", tr("Power Plans"),
        [this] (ModuleObject *module) -> QWidget*{
            DCCListView *powerplanListview = new DCCListView();
            powerplanListview->setAccessibleName("powerplanListview");

            powerplanListview->setModel(m_powerPlanModel);
            powerplanListview->setEditTriggers(QAbstractItemView::NoEditTriggers);
            powerplanListview->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
            powerplanListview->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            powerplanListview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            powerplanListview->setSelectionMode(QAbstractItemView::NoSelection);

            auto onHighPerformanceSupportChanged = [this, powerplanListview] (const bool isSupport) {
                int row_count = m_powerPlanModel->rowCount();
                if (!isSupport) {
                    int cur_place = powerplanListview->currentIndex().row();
                    for (int i = 0; i < row_count; ++i) {
                        QStandardItem *items = m_powerPlanModel->item(i, 0);
                        if (items->data(PowerPlanRole).toString() == PERFORMANCE) {
                            m_powerPlanModel->removeRow(i);

                            if (cur_place == i || cur_place < 0) {
                                powerplanListview->clicked(m_powerPlanModel->index(0, 0));
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
            };
            onHighPerformanceSupportChanged(m_model->isHighPerformanceSupported());

            connect(powerplanListview, &DListView::clicked, this, [this] (const QModelIndex &index){
                QStandardItem *item = m_powerPlanModel->item(index.row(), index.column());
                QString selectedPowerplan = item->data(PowerPlanRole).toString();
                Q_EMIT requestSetPowerPlan(selectedPowerplan);
            });
            connect(m_model, &PowerModel::highPerformaceSupportChanged, powerplanListview, onHighPerformanceSupportChanged);

            auto onCurPowerPlanChanged = [this] (const QString &curPowerPlan) {
                int row_count = m_powerPlanModel->rowCount();
                for (int i = 0; i < row_count; ++i) {
                    QStandardItem *items = m_powerPlanModel->item(i, 0);
                    if (items->data(PowerPlanRole).toString() == curPowerPlan) {
                        items->setCheckState(Qt::Checked);
                    } else {
                        items->setCheckState(Qt::Unchecked);
                    }
                }
            };

            connect(m_model, &PowerModel::powerPlanChanged, powerplanListview, onCurPowerPlanChanged);
            onCurPowerPlanChanged(m_model->getPowerPlan());
            return powerplanListview;
        },false));
    //　节能设置
    auto powerLowerBrightnessLabel = new TitleModule("powerLowerBrightnessLabel", tr("Power Saving Settings"));
    powerLowerBrightnessLabel->setHiden(!m_model->haveBettary());
    connect(m_model, &PowerModel::haveBettaryChanged, powerLowerBrightnessLabel, [powerLowerBrightnessLabel] (bool haveBettary) {
        powerLowerBrightnessLabel->setHiden(!haveBettary);
    });

    appendChild(powerLowerBrightnessLabel);
    SettingsGroupModule *group = new SettingsGroupModule("powerSavingSettings", tr("Power Saving Settings"));
    appendChild(group);
    group->setHiden(!m_model->haveBettary());
    connect(m_model, &PowerModel::haveBettaryChanged, powerLowerBrightnessLabel, [group] (bool haveBettary) {
        group->setHiden(!haveBettary);
    });

    group->appendChild(new ItemModule("autoPowerSavingOnLowBattery", tr("Auto power saving on low battery"),
        [this] (ModuleObject *module) -> QWidget*{
            DSwitchButton *lowPowerAutoIntoSaveEnergyMode = new DSwitchButton(/*tr("Auto power saving on low battery")*/);
            lowPowerAutoIntoSaveEnergyMode->setChecked(m_model->powerSavingModeAutoWhenQuantifyLow());
            connect(m_model, &PowerModel::powerSavingModeAutoWhenQuantifyLowChanged, lowPowerAutoIntoSaveEnergyMode, &DSwitchButton::setChecked);
            connect(lowPowerAutoIntoSaveEnergyMode, &DSwitchButton::checkedChanged, this, &GeneralModule::requestSetPowerSavingModeAutoWhenQuantifyLow);
            return lowPowerAutoIntoSaveEnergyMode;
        }));
    group->appendChild(new ItemModule("autoPowerSavingOnBattery", tr("Auto power saving on battery"),
        [this] (ModuleObject *module) -> QWidget*{
            DSwitchButton *autoIntoSaveEnergyMode = new DSwitchButton();
            autoIntoSaveEnergyMode->setChecked(m_model->autoPowerSaveMode());
            connect(m_model, &PowerModel::autoPowerSavingModeChanged, autoIntoSaveEnergyMode, &DSwitchButton::setChecked);
            connect(autoIntoSaveEnergyMode, &DSwitchButton::checkedChanged, this, &GeneralModule::requestSetPowerSavingModeAuto);
            return autoIntoSaveEnergyMode;
        }));
    group->appendChild(new ItemModule("decreaseBrightness", tr("Decrease Brightness"),
        [this] (ModuleObject *module) -> QWidget*{
            DComboBox *decreaseBrightnessRatio = new DComboBox();
            decreaseBrightnessRatio->setAccessibleName("sldLowerBrightness");
            QStringList annotions;
            annotions << "10%"
                      << "20%"
                      << "30%"
                      << "40%";
            decreaseBrightnessRatio->addItems(annotions);
            int maxBacklight = m_work->getMaxBacklightBrightness();
            decreaseBrightnessRatio->setVisible(maxBacklight >= 100 || maxBacklight == 0);
            decreaseBrightnessRatio->setCurrentText(QString("%1\%").arg(m_model->powerSavingModeLowerBrightnessThreshold()));
            connect(m_model, &PowerModel::powerSavingModeLowerBrightnessThresholdChanged, decreaseBrightnessRatio, [decreaseBrightnessRatio](const uint dLevel) {
                decreaseBrightnessRatio->setCurrentText(QString("%1\%").arg(dLevel));
            });
            connect(decreaseBrightnessRatio, &DComboBox::currentTextChanged, decreaseBrightnessRatio, [this] (const QString &value) {
                Q_EMIT requestSetPowerSavingModeLowerBrightnessThreshold(value.leftRef(2).toInt());
            });
            return decreaseBrightnessRatio;
        }));

    // 唤醒设置
    appendChild(new TitleModule("wakeupSettingsLabel", tr("Wakeup Settings")));
    group = new SettingsGroupModule("wakeupSettingsLabel", tr("Wakeup Settings"));
    appendChild(group);
    group->appendChild(new ItemModule("wakeComputerNeedPassword", tr("Password is required to wake up the computer"),
        [this] (ModuleObject *module) -> QWidget*{
            DSwitchButton *wakeComputerNeedPassword = new DSwitchButton();
            wakeComputerNeedPassword->setChecked(m_model->sleepLock());
            wakeComputerNeedPassword->setVisible(m_model->canSuspend() && m_model->getSuspend()); // 配置显示
            connect(m_model, &PowerModel::sleepLockChanged, wakeComputerNeedPassword, &DSwitchButton::setChecked);
            connect(m_model, &PowerModel::suspendChanged, wakeComputerNeedPassword, &DSwitchButton::setVisible);
            connect(wakeComputerNeedPassword, &DSwitchButton::checkedChanged, this, &GeneralModule::requestSetWakeComputer);
            return wakeComputerNeedPassword;
        }));
    group->appendChild(new ItemModule("wakeComputerNeedPassword", tr("Password is required to wake up the monitor"),
        [this] (ModuleObject *module) -> QWidget*{
            DSwitchButton *wakeDisplayNeedPassword = new DSwitchButton();
            wakeDisplayNeedPassword->setChecked(m_model->screenBlackLock());
            connect(m_model, &PowerModel::screenBlackLockChanged, wakeDisplayNeedPassword, &DSwitchButton::setChecked);
            connect(wakeDisplayNeedPassword, &DSwitchButton::checkedChanged, this, &GeneralModule::requestSetWakeDisplay);
            return wakeDisplayNeedPassword;
        }));
}
