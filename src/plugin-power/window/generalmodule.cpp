// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "generalmodule.h"

#include "dccslider.h"
#include "itemmodule.h"
#include "powermodel.h"
#include "powerworker.h"
#include "settingsgroupmodule.h"
#include "titlemodule.h"
#include "widgets/dcclistview.h"

#include <titledslideritem.h>

#include <DComboBox>
#include <DIconTheme>
#include <DListView>
#include <DSwitchButton>

#define BALANCE "balance"                        // 平衡模式
#define PERFORMANCE "performance"                // 高性能模式
#define BALANCEPERFORMANCE "balance_performance" // 高性能模式
#define POWERSAVE "powersave"                    // 节能模式

using namespace DCC_NAMESPACE;
DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

static QString get_translate(const QString &type)
{
    if (type == BALANCE) {
        return QObject::tr("Auto adjust CPU operating frequency based on CPU load condition");
    }
    if (type == BALANCEPERFORMANCE) {
        return QObject::tr(
                "Aggressively adjust CPU operating frequency based on CPU load condition");
    }
    if (type == PERFORMANCE) {
        return QObject::tr("Be good to imporving performance, but power consumption and heat "
                           "generation will increase");
    }
    return QObject::tr("CPU always works under low frequency, will reduce power consumption");
}

GeneralModule::GeneralModule(PowerModel *model, PowerWorker *work, QObject *parent)
    : PageModule("general", tr("General"), DIconTheme::findQIcon("dcc_general_purpose"), parent)
    , m_model(model)
    , m_work(work)
{
    m_powerPlanMap.insert(BALANCE, tr("Balanced"));
    m_powerPlanMap.insert(BALANCEPERFORMANCE, tr("Balance Performance"));
    m_powerPlanMap.insert(PERFORMANCE, tr("High Performance"));
    m_powerPlanMap.insert(POWERSAVE, tr("Power Saver"));

    connect(this, &GeneralModule::requestSetWakeDisplay, m_work, &PowerWorker::setScreenBlackLock);
    connect(this, &GeneralModule::requestSetWakeComputer, m_work, &PowerWorker::setSleepLock);

    connect(this,
            &GeneralModule::requestSetLowBatteryMode,
            m_work,
            &PowerWorker::setEnablePowerSave);
    connect(this,
            &GeneralModule::requestSetPowerSaveMode,
            m_work,
            &PowerWorker::setEnablePowerSave);

    connect(this,
            &GeneralModule::requestSetPowerSavingModeAutoWhenQuantifyLow,
            m_work,
            &PowerWorker::setPowerSavingModeAutoWhenQuantifyLow);
    connect(this,
            &GeneralModule::requestSetPowerSavingModeAuto,
            m_work,
            &PowerWorker::setPowerSavingModeAuto);
    connect(this,
            &GeneralModule::requestSetPowerSavingModeLowerBrightnessThreshold,
            m_work,
            &PowerWorker::setPowerSavingModeLowerBrightnessThreshold);
    connect(this,
            &GeneralModule::requestSetPowerSavingModeAutoBatteryPercentage,
            m_work,
            &PowerWorker::setPowerSavingModeAutoBatteryPercentage);
    connect(this, &GeneralModule::requestSetPowerPlan, m_work, &PowerWorker::setPowerPlan);
    initUI();
}

GeneralModule::~GeneralModule() { }

void GeneralModule::deactive() { }

void GeneralModule::initUI()
{
    m_powerPlanModel = new QStandardItemModel(this);
    QMap<QString, QString>::iterator iter;
    for (iter = m_powerPlanMap.begin(); iter != m_powerPlanMap.end(); ++iter) {
        DStandardItem *powerPlanItem = new DStandardItem(iter.value());
        DStandardItem *powerPlanItemTip = new DStandardItem(::get_translate(iter.key()));
        powerPlanItemTip->setEnabled(false);
        powerPlanItemTip->setEditable(false);
        powerPlanItemTip->setBackgroundRole(DPalette::Window);
        powerPlanItemTip->setTextColorRole(DPalette::TextTips);
        powerPlanItemTip->setSizeHint(QSize(-1, 20));
        powerPlanItem->setData(iter.key(), PowerPlanRole);
        m_powerPlanModel->appendRow(powerPlanItem);
        m_powerPlanModel->appendRow(powerPlanItemTip);
    }

    // 　性能设置
    appendChild(new TitleModule("powerPlansTitle", tr("Power Plans")));
    appendChild(new ItemModule(
            "powerPlans",
            tr("Power Plans"),
            [this](ModuleObject *module) -> QWidget * {
                Q_UNUSED(module);
                DCCListView *powerplanListview = new DCCListView();
                powerplanListview->setAccessibleName("Power Plans");

                powerplanListview->setModel(m_powerPlanModel);
                powerplanListview->setEditTriggers(QAbstractItemView::NoEditTriggers);
                powerplanListview->setBackgroundType(
                        DStyledItemDelegate::BackgroundType::RoundedBackground);
                powerplanListview->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                powerplanListview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                powerplanListview->setSelectionMode(QAbstractItemView::NoSelection);

                auto onHighPerformanceSupportChanged = [this,
                                                        powerplanListview](const bool isSupport) {
                    int row_count = m_powerPlanModel->rowCount();
                    if (!isSupport) {
                        int cur_place = powerplanListview->currentIndex().row();
                        for (int i = 0; i < row_count; i += 2) {
                            QStandardItem *items = m_powerPlanModel->item(i, 0);
                            if (items->data(PowerPlanRole).toString() == PERFORMANCE) {
                                m_powerPlanModel->removeRows(i, 2);

                                if (cur_place == i || cur_place < 0) {
                                    powerplanListview->clicked(m_powerPlanModel->index(0, 0));
                                }
                                break;
                            }
                        }
                    } else {
                        bool findHighPerform = false;
                        for (int i = 0; i < row_count; i += 2) {
                            QStandardItem *items = m_powerPlanModel->item(i, 0);
                            if (items->data(PowerPlanRole).toString() == PERFORMANCE) {
                                findHighPerform = true;
                                break;
                            }
                        }
                        if (!findHighPerform) {
                            DStandardItem *powerPlanItem =
                                    new DStandardItem(m_powerPlanMap.value(PERFORMANCE));
                            powerPlanItem->setData(PERFORMANCE, PowerPlanRole);
                            if (row_count == 6) {
                                DStandardItem *powerPlanItemTip =
                                        new DStandardItem(::get_translate(PERFORMANCE));
                                m_powerPlanModel->insertRow(4, { powerPlanItem, powerPlanItemTip });
                            } else {
                                DStandardItem *powerPlanItemTip =
                                        new DStandardItem(::get_translate(PERFORMANCE));
                                m_powerPlanModel->insertRow(2, { powerPlanItem, powerPlanItemTip });
                            }
                        }
                    }
                };
                onHighPerformanceSupportChanged(m_model->isHighPerformanceSupported());
                auto onBalancePerformanceSupportedChanged = [this, powerplanListview](
                                                                    const bool isSupport) {
                    int row_count = m_powerPlanModel->rowCount();
                    if (!isSupport) {
                        int cur_place = powerplanListview->currentIndex().row();
                        for (int i = 0; i < row_count; i += 2) {
                            QStandardItem *items = m_powerPlanModel->item(i, 0);
                            if (items->data(PowerPlanRole).toString() == BALANCEPERFORMANCE) {
                                m_powerPlanModel->removeRows(i, 2);

                                if (cur_place == i || cur_place < 0) {
                                    powerplanListview->clicked(m_powerPlanModel->index(0, 0));
                                }
                                break;
                            }
                        }
                    } else {
                        bool findBalancePerform = false;
                        for (int i = 0; i < row_count; i += 2) {
                            QStandardItem *items = m_powerPlanModel->item(i, 0);
                            if (items->data(PowerPlanRole).toString() == BALANCEPERFORMANCE) {
                                findBalancePerform = true;
                                break;
                            }
                        }
                        if (!findBalancePerform) {
                            DStandardItem *powerPlanItem =
                                    new DStandardItem(m_powerPlanMap.value(BALANCEPERFORMANCE));
                            powerPlanItem->setData(PERFORMANCE, PowerPlanRole);
                            DStandardItem *powerPlanItemTip =
                                    new DStandardItem(::get_translate(BALANCEPERFORMANCE));
                            m_powerPlanModel->insertRow(2, { powerPlanItem, powerPlanItemTip });
                        }
                    }
                };
                onBalancePerformanceSupportedChanged(m_model->isBalancePerformanceSupported());
                connect(powerplanListview,
                        &DListView::clicked,
                        this,
                        [this](const QModelIndex &index) {
                            QStandardItem *item =
                                    m_powerPlanModel->item(index.row(), index.column());
                            QString selectedPowerplan = item->data(PowerPlanRole).toString();
                            Q_EMIT requestSetPowerPlan(selectedPowerplan);
                        });
                connect(m_model,
                        &PowerModel::highPerformaceSupportChanged,
                        powerplanListview,
                        onHighPerformanceSupportChanged);

                auto onCurPowerPlanChanged = [this](const QString &curPowerPlan) {
                    int row_count = m_powerPlanModel->rowCount();
                    for (int i = 0; i < row_count; i += 2) {
                        QStandardItem *items = m_powerPlanModel->item(i, 0);
                        if (items->data(PowerPlanRole).toString() == curPowerPlan) {
                            items->setCheckState(Qt::Checked);
                        } else {
                            items->setCheckState(Qt::Unchecked);
                        }
                    }
                };

                connect(m_model,
                        &PowerModel::powerPlanChanged,
                        powerplanListview,
                        onCurPowerPlanChanged);
                onCurPowerPlanChanged(m_model->getPowerPlan());
                return powerplanListview;
            },
            false));
    // 　节能设置
    auto powerLowerBrightnessLabel =
            new TitleModule("powerSavingSettingsTitle", tr("Power Saving Settings"));

    appendChild(powerLowerBrightnessLabel);
    SettingsGroupModule *group =
            new SettingsGroupModule("powerSavingSettingsGroup", tr("Power Saving Settings"));
    appendChild(group);

    ItemModule *itemAutoPowerSavingOnLowBattery = new ItemModule(
            "autoPowerSavingOnLowBattery",
            tr("Auto power saving on low battery"),
            [this](ModuleObject *module) -> QWidget * {
                Q_UNUSED(module)
                DSwitchButton *lowPowerAutoIntoSaveEnergyMode =
                        new DSwitchButton(/*tr("Auto power saving on low battery")*/);
                lowPowerAutoIntoSaveEnergyMode->setChecked(
                        m_model->powerSavingModeAutoWhenQuantifyLow());
                connect(m_model,
                        &PowerModel::powerSavingModeAutoWhenQuantifyLowChanged,
                        lowPowerAutoIntoSaveEnergyMode,
                        &DSwitchButton::setChecked);
                connect(lowPowerAutoIntoSaveEnergyMode,
                        &DSwitchButton::checkedChanged,
                        this,
                        &GeneralModule::requestSetPowerSavingModeAutoWhenQuantifyLow);
                return lowPowerAutoIntoSaveEnergyMode;
            });
    itemAutoPowerSavingOnLowBattery->setVisible(m_model->haveBettary());
    connect(m_model,
            &PowerModel::haveBettaryChanged,
            itemAutoPowerSavingOnLowBattery,
            &ItemModule::setVisible);
    group->appendChild(itemAutoPowerSavingOnLowBattery);
    ItemModule *itemAutoPowerSavingOnBatterySlider = new ItemModule(
            "decreaseBrightness",
            tr("Decrease Brightness"),
            [this](ModuleObject *module) -> QWidget * {
                Q_UNUSED(module)
                TitledSliderItem *sldLowerBrightness =
                        new TitledSliderItem(tr("Low battery threshold"));
                QStringList annotions;
                annotions << "10%"
                          << "20%"
                          << "30%"
                          << "40%"
                          << "50%";
                sldLowerBrightness->setAnnotations(annotions);
                sldLowerBrightness->slider()->setRange(1, 5);
                sldLowerBrightness->slider()->setPageStep(10);
                sldLowerBrightness->slider()->setType(DCCSlider::Vernier);
                sldLowerBrightness->slider()->setTickPosition(QSlider::NoTicks);
                sldLowerBrightness->slider()->setValue(
                        m_model->powerSavingModeAutoBatteryPercentage() / 10);

                sldLowerBrightness->setValueLiteral(
                        QString("%1%").arg(m_model->powerSavingModeAutoBatteryPercentage()));
                connect(m_model,
                        &PowerModel::powerSavingModeAutoBatteryPercentageChanged,
                        sldLowerBrightness,
                        [sldLowerBrightness](const uint dLevel) {
                            sldLowerBrightness->slider()->blockSignals(true);
                            sldLowerBrightness->slider()->setValue(dLevel / 10);
                            sldLowerBrightness->slider()->blockSignals(false);
                        });
                connect(sldLowerBrightness->slider(),
                        &DCCSlider::valueChanged,
                        this,
                        [=](int value) {
                            sldLowerBrightness->setValueLiteral(annotions[value - 1]);
                            Q_EMIT requestSetPowerSavingModeAutoBatteryPercentage(value * 10);
                        });
                return sldLowerBrightness;
            },
            false);
    itemAutoPowerSavingOnBatterySlider->setVisible(m_model->haveBettary());
    group->appendChild(itemAutoPowerSavingOnBatterySlider);
    connect(m_model,
            &PowerModel::haveBettaryChanged,
            itemAutoPowerSavingOnBatterySlider,
            &ItemModule::setVisible);
    ItemModule *itemAutoPowerSavingOnBattery =
            new ItemModule("autoPowerSavingOnBattery",
                           tr("Auto power saving on battery"),
                           [this](ModuleObject *module) -> QWidget * {
                               Q_UNUSED(module)
                               DSwitchButton *autoIntoSaveEnergyMode = new DSwitchButton();
                               autoIntoSaveEnergyMode->setChecked(m_model->autoPowerSaveMode());
                               connect(m_model,
                                       &PowerModel::autoPowerSavingModeChanged,
                                       autoIntoSaveEnergyMode,
                                       &DSwitchButton::setChecked);
                               connect(autoIntoSaveEnergyMode,
                                       &DSwitchButton::checkedChanged,
                                       this,
                                       &GeneralModule::requestSetPowerSavingModeAuto);
                               return autoIntoSaveEnergyMode;
                           });
    itemAutoPowerSavingOnBattery->setVisible(m_model->haveBettary());
    connect(m_model,
            &PowerModel::haveBettaryChanged,
            itemAutoPowerSavingOnBattery,
            &ItemModule::setVisible);
    group->appendChild(itemAutoPowerSavingOnBattery);

    group->appendChild(new ItemModule(
            "decreaseBrightness",
            tr("Decrease Brightness"),
            [this](ModuleObject *module) -> QWidget * {
                Q_UNUSED(module)
                TitledSliderItem *sldLowerBrightness =
                        new TitledSliderItem(tr("Decrease Brightness"));
                sldLowerBrightness->setAccessibleName("Decrease Brightness");
                QStringList annotions;
                annotions << "10%"
                          << "20%"
                          << "30%"
                          << "40%";
                sldLowerBrightness->setAnnotations(annotions);
                sldLowerBrightness->slider()->setRange(1, 4);
                sldLowerBrightness->slider()->setPageStep(10);
                sldLowerBrightness->slider()->setType(DCCSlider::Vernier);
                sldLowerBrightness->slider()->setTickPosition(QSlider::NoTicks);

                int maxBacklight = m_work->getMaxBacklightBrightness();
                sldLowerBrightness->setVisible(maxBacklight >= 100 || maxBacklight == 0);
                sldLowerBrightness->slider()->setValue(
                        m_model->powerSavingModeLowerBrightnessThreshold() / 10);
                sldLowerBrightness->setValueLiteral(
                        QString("%1%").arg(m_model->powerSavingModeLowerBrightnessThreshold()));
                connect(m_model,
                        &PowerModel::powerSavingModeLowerBrightnessThresholdChanged,
                        sldLowerBrightness,
                        [sldLowerBrightness](const uint dLevel) {
                            sldLowerBrightness->slider()->blockSignals(true);
                            sldLowerBrightness->slider()->setValue(dLevel / 10);
                            sldLowerBrightness->slider()->blockSignals(false);
                        });
                connect(sldLowerBrightness->slider(),
                        &DCCSlider::valueChanged,
                        this,
                        [=](int value) {
                            sldLowerBrightness->setValueLiteral(annotions[value - 1]);
                            Q_EMIT requestSetPowerSavingModeLowerBrightnessThreshold(value * 10);
                        });
                return sldLowerBrightness;
            },
            false));

    // 唤醒设置
    appendChild(new TitleModule("wakeupSettingsTitle", tr("Wakeup Settings")));
    group = new SettingsGroupModule("wakeupSettingsGroup", tr("Wakeup Settings"));
    appendChild(group);
    group->appendChild(new ItemModule(
            "passwordIsRequiredToWakeUpTheComputer",
            tr("Unlocking is required to wake up the computer"),
            [this](ModuleObject *module) -> QWidget * {
                Q_UNUSED(module)
                DSwitchButton *wakeComputerNeedPassword = new DSwitchButton();
                wakeComputerNeedPassword->setChecked(m_model->sleepLock()
                                                     && !m_model->isNoPasswdLogin());
                wakeComputerNeedPassword->setDisabled(m_model->isNoPasswdLogin());
                wakeComputerNeedPassword->setVisible(m_model->canSuspend()
                                                     && m_model->getSuspend()); // 配置显示
                connect(m_model,
                        &PowerModel::sleepLockChanged,
                        wakeComputerNeedPassword,
                        [wakeComputerNeedPassword, this](bool checked) {
                            wakeComputerNeedPassword->setChecked(checked
                                                                 && !m_model->isNoPasswdLogin());
                        });
                connect(m_model,
                        &PowerModel::suspendChanged,
                        wakeComputerNeedPassword,
                        &DSwitchButton::setVisible);
                connect(wakeComputerNeedPassword,
                        &DSwitchButton::checkedChanged,
                        this,
                        &GeneralModule::requestSetWakeComputer);
                connect(m_model,
                        &PowerModel::noPasswdLoginChanged,
                        wakeComputerNeedPassword,
                        &DSwitchButton::setDisabled);
                return wakeComputerNeedPassword;
            }));
    group->appendChild(
            new ItemModule("passwordIsRequiredToWakeUpTheMonitor",
                           tr("Unlocking is required to wake up the monitor"),
                           [this](ModuleObject *module) -> QWidget * {
                               Q_UNUSED(module)
                               DSwitchButton *wakeDisplayNeedPassword = new DSwitchButton();
                               wakeDisplayNeedPassword->setChecked(m_model->screenBlackLock()
                                                                   && !m_model->isNoPasswdLogin());
                               wakeDisplayNeedPassword->setDisabled(m_model->isNoPasswdLogin());
                               connect(m_model,
                                       &PowerModel::screenBlackLockChanged,
                                       wakeDisplayNeedPassword,
                                       [wakeDisplayNeedPassword, this](bool checked) {
                                           wakeDisplayNeedPassword->setChecked(
                                                   checked && !m_model->isNoPasswdLogin());
                                       });

                               connect(wakeDisplayNeedPassword,
                                       &DSwitchButton::checkedChanged,
                                       this,
                                       &GeneralModule::requestSetWakeDisplay);
                               connect(m_model,
                                       &PowerModel::noPasswdLoginChanged,
                                       wakeDisplayNeedPassword,
                                       &DSwitchButton::setDisabled);
                               return wakeDisplayNeedPassword;
                           }));
}
