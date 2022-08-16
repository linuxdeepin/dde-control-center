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
#include "window/gsettingwatcher.h"
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

#define BALANCE "balance"         // 平衡模式
#define PERFORMANCE "performance" // 高性能模式
#define POWERSAVE "powersave"     // 节能模式

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::power;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::power;

const QString gsetting_systemSuspend = "systemSuspend";

static QGSettings *GSettings()
{
    static QGSettings settings("com.deepin.dde.dock.module.power");
    return &settings;
}

GeneralWidget::GeneralWidget(dcc::power::PowerModel *model, QWidget *parent, bool bIsBattery)
    : QWidget(parent)
    , m_bIsBattery(bIsBattery)
    , m_layout(new QVBoxLayout(this))
    , m_batteryLabel(nullptr)
    , m_batteryCapacity(new TitleValueItem)
    , m_model(model)
{
    if (m_model && m_model->isBalanceSupported()) {
        //~ contents_path /power/General
        //~ child_page General
        m_powerPlanMap.insert(BALANCE, tr("Balanced"));
    }

    if (m_model && m_model->isHighPerformanceSupported()) {
        //~ contents_path /power/General
        //~ child_page General
        m_powerPlanMap.insert(PERFORMANCE, tr("High Performance"));
    }

    if (m_model && m_model->isPowerSaveSupported()) {
        //~ contents_path /power/General
        //~ child_page General
        m_powerPlanMap.insert(POWERSAVE, tr("Power Saver"));
    }

    //~ contents_path /power/General
    //~ child_page General
    m_swLowPowerAutoIntoSaveEnergyMode = new SwitchWidget(tr("Auto power saving on low battery"), this);
    //~ contents_path /power/General
    //~ child_page General
    m_autoIntoSaveEnergyMode = new SwitchWidget(tr("Auto power saving on battery"), this);
    //~ contents_path /power/General
    //~ child_page General
    m_sldLowerBrightness = new TitledSliderItem(tr("Decrease brightness"), this);
    //~ contents_path /power/General
    //~ child_page General
    m_wakeComputerNeedPassword = new SwitchWidget(tr("Password is required to wake up the computer"), this);
    //~ contents_path /power/General
    //~ child_page General
    m_wakeDisplayNeedPassword = new SwitchWidget(tr("Password is required to wake up the monitor"), this);
    //~ contents_path /power/General
    //~ child_page General
    m_powerShowTimeToFull = new SwitchWidget(tr("Display remaining using and charging time"), this);
    //~ contents_path /power/General
    //~ child_page General
    m_ShowTimeToFullTips = new PowerDisplayWidget(tr("Maximum capacity"), this);
    initUi();

    connect(m_powerplanListview, &DListView::clicked, this, &GeneralWidget::onPowerPlanChanged);
    connect(m_swLowPowerAutoIntoSaveEnergyMode, &SwitchWidget::checkedChanged, this, &GeneralWidget::requestSetPowerSavingModeAutoWhenQuantifyLow);
    connect(m_autoIntoSaveEnergyMode, &SwitchWidget::checkedChanged, this, &GeneralWidget::requestSetPowerSavingModeAuto);
    connect(m_wakeComputerNeedPassword, &SwitchWidget::checkedChanged, this, &GeneralWidget::requestSetWakeComputer);
    connect(m_wakeDisplayNeedPassword, &SwitchWidget::checkedChanged, this, &GeneralWidget::requestSetWakeDisplay);
    connect(m_powerShowTimeToFull, &SwitchWidget::checkedChanged, this, &GeneralWidget::setPowerDisplay);
    connect(GSettings(), &QGSettings::changed, this, &GeneralWidget::onGSettingsChanged);
    onGSettingsChanged("showtimetofull");

    initConnect();
}

GeneralWidget::~GeneralWidget()
{
    GSettingWatcher::instance()->erase("powerPlansLabel");
    GSettingWatcher::instance()->erase("powerLowerBrightness");
    GSettingWatcher::instance()->erase("powerAutointoSaveenergyBattery");
    GSettingWatcher::instance()->erase("powerAutointoSaveenergy");
    GSettingWatcher::instance()->erase("powerLowerBrightness");
    GSettingWatcher::instance()->erase("powerShowtimeTofull");
    GSettingWatcher::instance()->erase("powerShowtimeTofulltips");
    GSettingWatcher::instance()->erase(gsetting_systemSuspend, m_wakeComputerNeedPassword);
}

void GeneralWidget::initUi()
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    /**** 性能设置 ************************************************************************/
    //~ contents_path /power/General
    //~ child_page General
    TitleLabel *powerPlansLabel = new TitleLabel(tr("Power Plans"), this);                            // 性能设置label
    DFontSizeManager::instance()->bind(powerPlansLabel, DFontSizeManager::T5, QFont::DemiBold); // 性能设置label字体
    GSettingWatcher::instance()->bind("powerPlansLabel", powerPlansLabel);                      // 使用GSettings来控制显示状态
    QVBoxLayout *powerPlansLayout = new QVBoxLayout(this);                                            // 性能模式布局
    m_powerplanListview = new DListView(this);                                                        // 电源模式列表
    m_powerplanListview->setAccessibleName("powerplanListview");

    m_powerPlanModel = new QStandardItemModel(m_powerplanListview);
    QMap<QString, QString>::iterator iter;
    for (iter = m_powerPlanMap.begin(); iter != m_powerPlanMap.end(); ++iter) {
        DStandardItem *powerPlanItem = new DStandardItem(iter.value());
        powerPlanItem->setData(iter.key(), PowerPlanRole);
        m_powerPlanModel->appendRow(powerPlanItem);
    }
    GSettingWatcher::instance()->bind("powerPlansLabel", m_powerplanListview); // 使用GSettings来控制显示状态
    m_powerplanListview->setModel(m_powerPlanModel);
    m_powerplanListview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_powerplanListview->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    m_powerplanListview->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    m_powerplanListview->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_powerplanListview->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_powerplanListview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_powerplanListview->setSelectionMode(QAbstractItemView::NoSelection);

    powerPlansLabel->setContentsMargins(10, 0, 10, 0);  // 性能设置label与外面布局的边距
    QMargins itemMargins(m_powerplanListview->itemMargins()); // 添加性能模式标题间隔
    itemMargins.setLeft(2); //dtk控件里面自带文字左间距8,这里设置2就足够10
    m_powerplanListview->setItemMargins(itemMargins);
    powerPlansLayout->addWidget(powerPlansLabel);       // 添加性能设置label
    powerPlansLayout->addWidget(m_powerplanListview);   // 添加性能模式列表
    powerPlansLayout->setSpacing(10);                   // 布局中组件间距
    powerPlansLayout->setContentsMargins(10, 0, 10, 0); // 性能设置布局与外面总布局的边距
    m_layout->addLayout(powerPlansLayout);              // 将性能设置布局添加到总布局中

    powerPlansLabel->setVisible(m_model->isBalanceSupported() || m_model->isHighPerformanceSupported() || m_model->isPowerSaveSupported());
    m_powerplanListview->setVisible(m_model->isBalanceSupported() || m_model->isHighPerformanceSupported() || m_model->isPowerSaveSupported());

    /*************************************************************************************/

    /**** 节能设置 ************************************************************************/
    //~ contents_path /power/General
    //~ child_page General
    TitleLabel *energySavingLabel = new TitleLabel(tr("Power Saving Settings"), this);                  // 节能设置label
    DFontSizeManager::instance()->bind(energySavingLabel, DFontSizeManager::T5, QFont::DemiBold); // 节能设置label字体
    QVBoxLayout *energySavingLayout = new QVBoxLayout(this);                                            // 节能设置布局
    GSettingWatcher::instance()->bind("powerLowerBrightness", energySavingLabel);                 // 使用GSettings来控制显示状态
    SettingsGroup *energySavingGrp = new SettingsGroup;

    QStringList annotions;
    annotions << "10%" << "20%" << "30%" << "40%";

    m_sldLowerBrightness->slider()->setAccessibleName("sldLowerBrightness");
    m_sldLowerBrightness->slider()->setAnnotations(annotions);
    m_sldLowerBrightness->slider()->setRange(1, 4);
    m_sldLowerBrightness->slider()->setPageStep(10);
    m_sldLowerBrightness->slider()->setType(DCCSlider::Vernier);
    m_sldLowerBrightness->slider()->setTickPosition(QSlider::NoTicks);

    energySavingGrp->appendItem(m_swLowPowerAutoIntoSaveEnergyMode);
    energySavingGrp->appendItem(m_autoIntoSaveEnergyMode);
    energySavingGrp->appendItem(m_sldLowerBrightness);
    // 使用GSettings来控制显示状态
    GSettingWatcher::instance()->bind("powerAutointoSaveenergyBattery", m_autoIntoSaveEnergyMode);
    GSettingWatcher::instance()->bind("powerAutointoSaveenergy", m_swLowPowerAutoIntoSaveEnergyMode);
    GSettingWatcher::instance()->bind("powerLowerBrightness", m_sldLowerBrightness);

    energySavingLabel->setContentsMargins(10, 0, 10, 0);  // 节能设置label与外面布局的边距
    energySavingLayout->addWidget(energySavingLabel);     // 添加节能设置label
    energySavingLayout->addWidget(energySavingGrp);       // 添加节能设置
    energySavingLayout->setSpacing(10);                   // 布局中组件间距
    energySavingLayout->setContentsMargins(10, 0, 10, 0); // 节能设置与外面总布局的边距
    m_layout->addLayout(energySavingLayout);              // 将节能模式布局添加到总布局中

    energySavingLabel->setVisible(m_model->isPowerSaveSupported());
    energySavingGrp->setVisible(m_model->isPowerSaveSupported());

    /*************************************************************************************/

    /**** 唤醒设置 ************************************************************************/
    //~ contents_path /power/General
    //~ child_page General
    TitleLabel *wakeupLabel = new TitleLabel(tr("Wakeup Settings"), this);                        // 唤醒设置label
    DFontSizeManager::instance()->bind(wakeupLabel, DFontSizeManager::T5, QFont::DemiBold); // 唤醒设置label字体
    QVBoxLayout *wakeupLayout = new QVBoxLayout(this);                                          // 唤醒设置布局
    SettingsGroup *wakeupSettingsGrp = new SettingsGroup;
    wakeupSettingsGrp->appendItem(m_wakeComputerNeedPassword);
    wakeupSettingsGrp->appendItem(m_wakeDisplayNeedPassword);
    GSettingWatcher::instance()->bind(gsetting_systemSuspend, m_wakeComputerNeedPassword);  // 使用GSettings来控制显示状态

    wakeupLabel->setContentsMargins(10, 0, 10, 0);
    wakeupLayout->addWidget(wakeupLabel);           // 添加唤醒设置label
    wakeupLayout->addWidget(wakeupSettingsGrp);     // 添加唤醒设置
    wakeupLayout->setSpacing(10);                   // 布局中组件间距
    wakeupLayout->setContentsMargins(10, 0, 10, 0); // 唤醒设置与外面总布局的边距
    m_layout->addLayout(wakeupLayout);              // 将唤醒设置布局添加到总布局中
    /*************************************************************************************/

    /**** 电池设置 ************************************************************************/
    //~ contents_path /power/General
    //~ child_page General
    m_batteryLabel = new TitleLabel(tr("Battery"), this);
    DFontSizeManager::instance()->bind(m_batteryLabel, DFontSizeManager::T5, QFont::DemiBold); // 电池设置label字体
    QVBoxLayout *batteyLayout = new QVBoxLayout(this);                                               // 电池设置布局
    SettingsGroup *batterySettingsGrp = new SettingsGroup;

    QDBusInterface powerInter("com.deepin.system.Power", "/com/deepin/system/Power", "com.deepin.system.Power", QDBusConnection::systemBus());
    double capacity = powerInter.property("BatteryCapacity").toDouble();
    m_ShowTimeToFullTips->setText(QString::number(int(capacity)) + "%");

    batterySettingsGrp->appendItem(m_powerShowTimeToFull);
    batterySettingsGrp->appendItem(m_ShowTimeToFullTips);

    // 使用GSettings来控制显示状态
    GSettingWatcher::instance()->bind("powerShowtimeTofull", m_powerShowTimeToFull);
    GSettingWatcher::instance()->bind("powerShowtimeTofulltips", m_ShowTimeToFullTips);
    m_batteryLabel->setVisible(m_bIsBattery);
    m_powerShowTimeToFull->setVisible(m_bIsBattery && (GSettingWatcher::instance()->getStatus("powerShowtimeTofull") != "Hidden"));
    m_ShowTimeToFullTips->setVisible(m_bIsBattery && (GSettingWatcher::instance()->getStatus("powerShowtimeTofulltips") != "Hidden"));

    m_batteryLabel->setContentsMargins(10, 0, 10, 0); // 电池设置label与外面布局的边距
    batteyLayout->addWidget(m_batteryLabel);          // 添加电池设置label
    batteyLayout->addWidget(batterySettingsGrp);      // 添加电池设置
    batteyLayout->setSpacing(10);                     // 布局中组件间距
    batteyLayout->setContentsMargins(10, 0, 10, 0);   // 电池设置与外面总布局的边距
    m_layout->addLayout(batteyLayout);                // 将唤醒设置布局添加到总布局中
    /*************************************************************************************/

    m_layout->setSpacing(30);
    m_layout->setAlignment(Qt::AlignTop);
    m_layout->setContentsMargins(0, 2, 0, 5); // 总布局上下边距

    ContentWidget *contentWgt = new ContentWidget(this);
    contentWgt->setAccessibleName("General_ContentWidget");
    QWidget *mainWgt = new TranslucentFrame(this); // 添加一层半透明框架
    mainWgt->setLayout(m_layout);
    contentWgt->setContent(mainWgt);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(contentWgt);
    mainLayout->setContentsMargins(0, 8, 0, 8); // 圆角
    setLayout(mainLayout);
}

void GeneralWidget::initConnect()
{
    Q_ASSERT(m_model);

    connect(m_model, &PowerModel::screenBlackLockChanged, m_wakeDisplayNeedPassword, &SwitchWidget::setChecked);
    connect(m_model, &PowerModel::sleepLockChanged, m_wakeComputerNeedPassword, &SwitchWidget::setChecked);

#ifndef DCC_DISABLE_POWERSAVE
    connect(m_model, &PowerModel::autoPowerSavingModeChanged, m_autoIntoSaveEnergyMode, &SwitchWidget::setChecked);
#endif
    connect(m_model, &PowerModel::suspendChanged, m_wakeComputerNeedPassword, &SwitchWidget::setVisible);

    connect(GSettingWatcher::instance(), &GSettingWatcher::notifyGSettingsChanged, this, [ = ] (const QString &key, const QString &value) {
        if (key == gsetting_systemSuspend && !value.isEmpty()) {
            m_wakeComputerNeedPassword->setVisible(m_model->getSuspend() && value != "Hidden");
        }
    });

    connect(m_model, &PowerModel::highPerformaceSupportChanged, this, &GeneralWidget::onHighPerformanceSupportChanged);
    onHighPerformanceSupportChanged(m_model->isHighPerformanceSupported());

    connect(m_model, &PowerModel::powerPlanChanged, this, &GeneralWidget::onCurPowerPlanChanged);
    onCurPowerPlanChanged(m_model->getPowerPlan());

    // init ui data
    blockSignals(true);
    m_wakeDisplayNeedPassword->setChecked(m_model->screenBlackLock());
    m_wakeComputerNeedPassword->setChecked(m_model->sleepLock());
    blockSignals(false);

#ifndef DCC_DISABLE_POWERSAVE
    m_autoIntoSaveEnergyMode->setChecked(m_model->autoPowerSaveMode());
#endif

    m_wakeComputerNeedPassword->setVisible(m_model->getSuspend() && (GSettingWatcher::instance()->getStatus(gsetting_systemSuspend) != "Hidden"));

    //-------------sp2 add-------------------------
    m_swLowPowerAutoIntoSaveEnergyMode->setChecked(m_model->powerSavingModeAutoWhenQuantifyLow());
    connect(m_model, &PowerModel::powerSavingModeAutoWhenQuantifyLowChanged, m_swLowPowerAutoIntoSaveEnergyMode, &SwitchWidget::setChecked);

    QDBusInterface Interface("com.deepin.daemon.Display",
                             "/com/deepin/daemon/Display",
                             "com.deepin.daemon.Display",
                             QDBusConnection::sessionBus());
    int maxBacklight = Interface.property("MaxBacklightBrightness").toInt();
    m_sldLowerBrightness->setVisible(maxBacklight >= 100 || maxBacklight == 0);
    m_sldLowerBrightness->slider()->setValue(m_model->powerSavingModeLowerBrightnessThreshold() / 10);
    connect(m_model, &PowerModel::powerSavingModeLowerBrightnessThresholdChanged, this,  [ = ](const uint dLevel) {
        m_sldLowerBrightness->slider()->blockSignals(true);
        m_sldLowerBrightness->slider()->setValue(dLevel / 10);
        m_sldLowerBrightness->slider()->blockSignals(false);
    });

    bool bStatus = m_model->haveBettary();

    if (GSettingWatcher::instance()->getStatus("powerAutointoSaveenergyBattery") != "Hidden")
        m_swLowPowerAutoIntoSaveEnergyMode->setVisible(bStatus);
    if (GSettingWatcher::instance()->getStatus("powerAutointoSaveenergy") != "Hidden")
        m_autoIntoSaveEnergyMode->setVisible(bStatus);

    connect(m_model, &PowerModel::haveBettaryChanged, this, &GeneralWidget::onBatteryChanged);

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
    m_swLowPowerAutoIntoSaveEnergyMode->setVisible(state);
    m_autoIntoSaveEnergyMode->setVisible(state);
    m_batteryLabel->setVisible(state);
    m_powerShowTimeToFull->setVisible(state);
    m_ShowTimeToFullTips->setVisible(state);
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
