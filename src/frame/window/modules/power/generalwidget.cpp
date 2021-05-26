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

#define BALANCE "balance"         // 平衡模式
#define PERFORMANCE "performance" // 高性能模式
#define POWERSAVE "powersave"     // 节能模式

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
    , m_bIsBattery(bIsBattery)
    , m_layout(new QVBoxLayout)
    , m_swLowPowerAutoIntoSaveEnergyMode(new SwitchWidget(tr("Auto power saving on low battery")))
    , m_autoIntoSaveEnergyMode(new SwitchWidget(tr("Auto power saving on battery")))
    , m_sldLowerBrightness(new TitledSliderItem(tr("Decrease brightness"), this))
    , m_wakeComputerNeedPassword(new SwitchWidget(tr("Password is required to wake up the computer")))
    , m_wakeDisplayNeedPassword(new SwitchWidget(tr("Password is required to wake up the monitor")))
    , m_batteryLabel(new TitleLabel(tr("Battery")))
    , m_powerShowTimeToFull(new SwitchWidget(tr("Display remaining using and charging time")))
    , m_ShowTimeToFullTips(new PowerDisplayWidget(tr("Maximum capacity"), this))
    , m_showBatteryCapacity(new SwitchWidget(tr("Show battery capacity")))
    , m_batteryCapacity(new TitleValueItem)
{
    m_powerPlanMap.insert(BALANCE, tr("Balanced"));
    m_powerPlanMap.insert(PERFORMANCE, tr("High Performance"));
    m_powerPlanMap.insert(POWERSAVE, tr("Power Saver"));

    initUi();

    connect(m_powerplanListview, &DListView::clicked, this, &GeneralWidget::onPowerPlanChanged);
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

void GeneralWidget::initUi()
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    /**** 性能设置 ************************************************************************/
    //~ contents_path /power/General
    TitleLabel *powerPlansLabel = new TitleLabel(tr("Power Plans"));                            // 性能设置label
    DFontSizeManager::instance()->bind(powerPlansLabel, DFontSizeManager::T5, QFont::DemiBold); // 性能设置label字体
    QVBoxLayout *powerPlansLayout = new QVBoxLayout;                                            // 性能模式布局
    m_powerplanListview = new DListView();                                                      // 电源模式列表

    m_powerPlanModel = new QStandardItemModel(m_powerplanListview);
    QMap<QString, QString>::iterator iter;
    for (iter = m_powerPlanMap.begin(); iter != m_powerPlanMap.end(); ++iter) {
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

    powerPlansLabel->setContentsMargins(10, 0, 10, 0);  // 性能设置label与外面布局的边距
    powerPlansLayout->addWidget(powerPlansLabel);       // 添加性能设置label
    powerPlansLayout->addWidget(m_powerplanListview);   // 添加性能模式列表
    powerPlansLayout->setSpacing(10);                   // 布局中组件间距
    powerPlansLayout->setContentsMargins(10, 0, 10, 0); // 性能设置布局与外面总布局的边距
    m_layout->addLayout(powerPlansLayout);              // 将性能设置布局添加到总布局中
    /*************************************************************************************/

    /**** 节能设置 ************************************************************************/
    //~ contents_path /power/General
    TitleLabel *energySavingLabel = new TitleLabel(tr("Power Saving Settings"));                  // 节能设置label
    DFontSizeManager::instance()->bind(energySavingLabel, DFontSizeManager::T5, QFont::DemiBold); // 节能设置label字体
    QVBoxLayout *energySavingLayout = new QVBoxLayout;                                            // 节能设置布局
    SettingsGroup *energySavingGrp = new SettingsGroup;

    QStringList annotions;
    annotions << "10%" << "20%" << "30%" << "40%";

    m_sldLowerBrightness->slider()->setAnnotations(annotions);
    m_sldLowerBrightness->slider()->setRange(1, 4);
    m_sldLowerBrightness->slider()->setPageStep(10);
    m_sldLowerBrightness->slider()->setType(DCCSlider::Vernier);
    m_sldLowerBrightness->slider()->setTickPosition(QSlider::NoTicks);

    energySavingGrp->appendItem(m_swLowPowerAutoIntoSaveEnergyMode);
    energySavingGrp->appendItem(m_autoIntoSaveEnergyMode);
    energySavingGrp->appendItem(m_sldLowerBrightness);

    energySavingLabel->setContentsMargins(10, 0, 10, 0);  // 节能设置label与外面布局的边距
    energySavingLayout->addWidget(energySavingLabel);     // 添加节能设置label
    energySavingLayout->addWidget(energySavingGrp);       // 添加节能设置
    energySavingLayout->setSpacing(10);                   // 布局中组件间距
    energySavingLayout->setContentsMargins(10, 0, 10, 0); // 节能设置与外面总布局的边距
    m_layout->addLayout(energySavingLayout);              // 将节能模式布局添加到总布局中
    /*************************************************************************************/

    /**** 唤醒设置 ************************************************************************/
    //~ contents_path /power/General
    TitleLabel *wakeupLabel = new TitleLabel(tr("Wakeup Settings"));                        // 唤醒设置label
    DFontSizeManager::instance()->bind(wakeupLabel, DFontSizeManager::T5, QFont::DemiBold); // 唤醒设置label字体
    QVBoxLayout *wakeupLayout = new QVBoxLayout;                                            // 唤醒设置布局
    SettingsGroup *wakeupSettingsGrp = new SettingsGroup;

    wakeupSettingsGrp->appendItem(m_wakeComputerNeedPassword);
    wakeupSettingsGrp->appendItem(m_wakeDisplayNeedPassword);

    wakeupLabel->setContentsMargins(10, 0, 10, 0);
    wakeupLayout->addWidget(wakeupLabel);           // 添加唤醒设置label
    wakeupLayout->addWidget(wakeupSettingsGrp);     // 添加唤醒设置
    wakeupLayout->setSpacing(10);                   // 布局中组件间距
    wakeupLayout->setContentsMargins(10, 0, 10, 0); // 唤醒设置与外面总布局的边距
    m_layout->addLayout(wakeupLayout);              // 将唤醒设置布局添加到总布局中
    /*************************************************************************************/

    /**** 电池设置 ************************************************************************/
    //~ contents_path /power/General
    DFontSizeManager::instance()->bind(m_batteryLabel, DFontSizeManager::T5, QFont::DemiBold); // 电池设置label字体
    QVBoxLayout *batteyLayout = new QVBoxLayout;                                               // 电池设置布局
    SettingsGroup *batterySettingsGrp = new SettingsGroup;

    QDBusInterface powerInter("com.deepin.system.Power", "/com/deepin/system/Power", "com.deepin.system.Power", QDBusConnection::systemBus());
    double capacity = powerInter.property("BatteryCapacity").toDouble();
    m_ShowTimeToFullTips->setText(QString::number(int(capacity)) + "%");

    batterySettingsGrp->appendItem(m_powerShowTimeToFull);
    batterySettingsGrp->appendItem(m_ShowTimeToFullTips);

    m_batteryLabel->setVisible(m_bIsBattery);
    m_powerShowTimeToFull->setVisible(m_bIsBattery);
    m_ShowTimeToFullTips->setVisible(m_bIsBattery);

    m_batteryLabel->setContentsMargins(10, 0, 10, 0); // 电池设置label与外面布局的边距
    batteyLayout->addWidget(m_batteryLabel);          // 添加电池设置label
    batteyLayout->addWidget(batterySettingsGrp);      // 添加电池设置
    batteyLayout->setSpacing(10);                     // 布局中组件间距
    batteyLayout->setContentsMargins(10, 0, 10, 0);   // 电池设置与外面总布局的边距
    m_layout->addLayout(batteyLayout);                // 将唤醒设置布局添加到总布局中
    /*************************************************************************************/

    m_layout->setSpacing(30);
    m_layout->setAlignment(Qt::AlignTop);
    m_layout->setContentsMargins(0, 10, 0, 5); // 总布局上下边距

    ContentWidget *contentWgt = new ContentWidget;
    QWidget *mainWgt = new TranslucentFrame; // 添加一层半透明框架
    mainWgt->setLayout(m_layout);
    contentWgt->setContent(mainWgt);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(contentWgt);
    setLayout(mainLayout);
}

void GeneralWidget::setModel(const PowerModel *model)
{
    connect(model, &PowerModel::screenBlackLockChanged, m_wakeDisplayNeedPassword, &SwitchWidget::setChecked);
    connect(model, &PowerModel::sleepLockChanged, m_wakeComputerNeedPassword, &SwitchWidget::setChecked);

#ifndef DCC_DISABLE_POWERSAVE
    connect(model, &PowerModel::autoPowerSavingModeChanged, m_autoIntoSaveEnergyMode, &SwitchWidget::setChecked);
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
    m_sldLowerBrightness->setVisible(maxBacklight >= 100 || maxBacklight == 0);
    m_sldLowerBrightness->slider()->setValue(model->powerSavingModeLowerBrightnessThreshold() / 10);
    connect(model, &PowerModel::powerSavingModeLowerBrightnessThresholdChanged, this,  [ = ](const uint dLevel) {
        m_sldLowerBrightness->slider()->setValue(dLevel / 10);
    });

    bool bStatus = model->haveBettary();

    m_swLowPowerAutoIntoSaveEnergyMode->setVisible(bStatus);
    m_autoIntoSaveEnergyMode->setVisible(bStatus);

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
        for (int i = 0; i < row_count; ++i) {
            QStandardItem *items = m_powerPlanModel->item(i, 0);
            if (items->data(PowerPlanRole).toString() == PERFORMANCE) {
                m_powerPlanModel->removeRow(i);
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
