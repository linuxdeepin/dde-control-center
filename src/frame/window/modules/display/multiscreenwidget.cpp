/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     huangweihua <huangweihua@uniontech.com>
 *
 * Maintainer: huangweihua <huangweihua@uniontech.com>
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

#include "multiscreenwidget.h"
#include "brightnesswidget.h"
#include "scalingwidget.h"
#include "resolutionwidget.h"
#include "refreshratewidget.h"
#include "rotatewidget.h"
#include "secondaryscreendialog.h"
#include "../../mainwindow.h"
#include "widgets/settingsitem.h"
#include "modules/display/displaymodel.h"
#include "modules/display/monitorcontrolwidget.h"
#include "modules/display/monitorindicator.h"
#include "modules/display/recognizewidget.h"
#include "window/gsettingwatcher.h"
#include "window/dconfigwatcher.h"

#include <QVBoxLayout>
#include <QApplication>
#include <QComboBox>
#include <QDesktopWidget>

using namespace dcc::widgets;
using namespace dcc::display;
using namespace DCC_NAMESPACE::display;
DWIDGET_USE_NAMESPACE

const int ComboxWidth = 300;

MultiScreenWidget::MultiScreenWidget(QWidget *parent)
    : QWidget(parent)
    , m_contentLayout(new QVBoxLayout(this))
    , m_monitorControlWidget(new MonitorControlWidget(240, this)) //根据产品设计更改出事高度为240
    , m_fullIndication(new MonitorIndicator(this))
    , m_multiSettingLabel(new TitleLabel(tr("Multiple Displays"), this))
    , m_modeSettingsItem(new SettingsItem(this))
    , m_modeLabel(new QLabel(tr("Mode"), this))
    , m_modeCombox(new QComboBox(this))
    , m_primarySettingsItem(new SettingsItem(this))
    , m_primaryLabel(new QLabel(tr("Main Screen"), this))
    , m_primaryCombox(new QComboBox(this))
    , m_brightnessSpacerItem(new QSpacerItem(0, 20))
    , m_brightnessWidget(new BrightnessWidget(this))
    , m_scalingWidget(new ScalingWidget(this))
    , m_resolutionWidget(new ResolutionWidget(300, this))
    , m_refreshRateWidget(new RefreshRateWidget(300, this))
    , m_rotateWidget(new RotateWidget(300, this))
    , m_model(nullptr)
{
    m_monitorControlWidget->setAccessibleName("MultiScreenWidget_monitorControl");
    m_fullIndication->setAccessibleName("fullIndication");

    m_contentLayout->setSpacing(0);
    m_contentLayout->setContentsMargins(56, 20, 56, 0);
    m_contentLayout->addWidget(m_monitorControlWidget);
    m_contentLayout->addSpacing(20);
    m_contentLayout->addWidget(m_multiSettingLabel);
    QHBoxLayout *modeLayout = new QHBoxLayout(m_modeSettingsItem);
    modeLayout->setContentsMargins(10, 0, 10, 0);
    modeLayout->addWidget(m_modeLabel);
    modeLayout->addWidget(m_modeCombox);
    m_modeCombox->setFocusPolicy(Qt::NoFocus);
    m_modeCombox->setMinimumWidth(ComboxWidth);
    m_modeCombox->setMinimumHeight(36);
    m_modeSettingsItem->addBackground();
    m_modeSettingsItem->setMinimumHeight(48);
    m_modeSettingsItem->setLayout(modeLayout);
    m_contentLayout->addSpacing(10);
    m_contentLayout->addWidget(m_modeSettingsItem);

    QHBoxLayout *primaryLayout = new QHBoxLayout(m_primarySettingsItem);
    primaryLayout->setContentsMargins(10, 0, 10, 0);
    primaryLayout->addWidget(m_primaryLabel);
    primaryLayout->addWidget(m_primaryCombox);
    m_primaryCombox->setFocusPolicy(Qt::NoFocus);
    m_primaryCombox->setMinimumWidth(ComboxWidth);
    m_primaryCombox->setMinimumHeight(36);
    m_primarySettingsItem->addBackground();
    m_primarySettingsItem->setMinimumHeight(48);
    m_primarySettingsItem->setLayout(primaryLayout);
    m_contentLayout->addSpacing(10);
    m_contentLayout->addWidget(m_primarySettingsItem);

    m_contentLayout->addSpacerItem(m_brightnessSpacerItem);
    m_contentLayout->addWidget(m_brightnessWidget);
    m_contentLayout->addSpacing(20);
    m_contentLayout->addWidget(m_scalingWidget);
    m_contentLayout->addSpacing(30);
    m_contentLayout->addWidget(m_resolutionWidget);
    m_contentLayout->addSpacing(20);
    m_contentLayout->addWidget(m_refreshRateWidget);
    m_contentLayout->addSpacing(20);
    m_contentLayout->addWidget(m_rotateWidget);
    m_contentLayout->addStretch();

    setLayout(m_contentLayout);

    // 使用GSettings来控制显示状态
    GSettingWatcher::instance()->bind("displayResolution", m_resolutionWidget);
    GSettingWatcher::instance()->bind("displayRefreshRate", m_refreshRateWidget);
    GSettingWatcher::instance()->bind("displayRotate", m_rotateWidget);
    GSettingWatcher::instance()->bind("displayMultipleDisplays", m_multiSettingLabel);
    GSettingWatcher::instance()->bind("displayMultipleDisplays", m_modeSettingsItem);
    GSettingWatcher::instance()->bind("displayMultipleDisplays", m_primarySettingsItem);
    DConfigWatcher::instance()->bind(DConfigWatcher::display,"mainScreen", m_primarySettingsItem);

    QDesktopWidget *desktopwidget = QApplication::desktop();
    connect(desktopwidget,SIGNAL(resized(int)),this,SLOT(onResetSecondaryScreenDlg()));
}

MultiScreenWidget::~MultiScreenWidget()
{
    for (auto dlg : m_secondaryScreenDlgList) {
        dlg->deleteLater();
    }
    m_secondaryScreenDlgList.clear();

    for(auto widget : m_recognizeWidget) {
        widget->deleteLater();
    }
    m_recognizeWidget.clear();

    GSettingWatcher::instance()->erase("displayResolution", m_resolutionWidget);
    GSettingWatcher::instance()->erase("displayRefreshRate", m_refreshRateWidget);
    GSettingWatcher::instance()->erase("displayRotate", m_rotateWidget);
    GSettingWatcher::instance()->erase("displayMultipleDisplays", m_multiSettingLabel);
    GSettingWatcher::instance()->erase("displayMultipleDisplays", m_modeSettingsItem);
    GSettingWatcher::instance()->erase("displayMultipleDisplays", m_primarySettingsItem);
    DConfigWatcher::instance()->erase(DConfigWatcher::display,"mainScreen", m_primarySettingsItem);
}

void MultiScreenWidget::setModel(dcc::display::DisplayModel *model)
{
    m_model = model;

    initModeList();
    initPrimaryList();

    connect(m_model, &DisplayModel::displayModeChanged, m_monitorControlWidget, &MonitorControlWidget::setScreensMerged);
    connect(m_model, &DisplayModel::displayModeChanged, this, [=](const int mode) {
        if (mode == MERGE_MODE) {
            m_modeCombox->setCurrentIndex(0);
            m_primarySettingsItem->setVisible(false);
            m_brightnessWidget->showBrightness();
            m_monitorControlWidget->setModel(m_model);
            for (auto dlg : m_secondaryScreenDlgList) {
                dlg->deleteLater();
            }
            m_secondaryScreenDlgList.clear();
        } else if (mode == EXTEND_MODE) {
            m_modeCombox->setCurrentIndex(1);
            m_primarySettingsItem->setVisible(true);
            m_brightnessWidget->showBrightness(m_model->primaryMonitor());
            m_monitorControlWidget->setModel(m_model);
            initSecondaryScreenDialog();
        } else if (m_model->displayMode() == SINGLE_MODE) {
            auto monitorList = m_model->monitorList();
            for (int idx = 0; idx < monitorList.size(); ++idx) {
                auto monitor = monitorList[idx];
                if (monitor->enable()) {
                    m_modeCombox->setCurrentIndex(idx + 2);
                    m_monitorControlWidget->setModel(m_model, monitor);
                    break;
                }
            }

            m_primarySettingsItem->setVisible(false);
            m_brightnessWidget->showBrightness(m_model->primaryMonitor());

            for (auto dlg : m_secondaryScreenDlgList) {
                dlg->deleteLater();
            }
            m_secondaryScreenDlgList.clear();
        }
    });
    connect(m_model, &DisplayModel::primaryScreenChanged, this, [=](const QString &name) {
        for (int idx = 0; idx < m_primaryCombox->count(); ++idx) {
            if (name == m_primaryCombox->itemText(idx)) {
                m_primaryCombox->blockSignals(true);
                m_primaryCombox->setCurrentIndex(idx);
                m_primaryCombox->blockSignals(false);
                break;
            }
        }

        if (m_model->displayMode() == MERGE_MODE) {
            m_brightnessWidget->showBrightness();
        } else if (m_model->displayMode() == EXTEND_MODE) {
            m_brightnessWidget->showBrightness(m_model->primaryMonitor());
        } else if (m_model->displayMode() == SINGLE_MODE) {
            m_monitorControlWidget->setModel(m_model, m_model->primaryMonitor());
            m_brightnessWidget->showBrightness(m_model->primaryMonitor());
            auto monitorList = m_model->monitorList();
            for (int idx = 0; idx < monitorList.size(); ++idx) {
                auto monitor = monitorList[idx];
                if (monitor->enable()) {
                    m_modeCombox->setCurrentIndex(idx + 2);
                    break;
                }
            }
        }

        m_resolutionWidget->setMonitor(m_model->primaryMonitor());
        m_refreshRateWidget->setMonitor(m_model->primaryMonitor());
        m_rotateWidget->setMonitor(m_model->primaryMonitor());

        for (const auto &monitor : m_model->monitorList()) {
            if (name == monitor->name()) {
                m_fullIndication->setGeometry(monitor->rect());
                m_fullIndication->setVisible(true);
                QTimer::singleShot(500, this, [=] { m_fullIndication->setVisible(false); });
                break;
            }
        }

        Q_EMIT requestGatherEnabled(true);
        initSecondaryScreenDialog();
    });
    connect(m_model, &DisplayModel::resolutionRefreshEnableChanged, this, [=](const bool enable) {
        m_multiSettingLabel->setVisible(GSettingWatcher::instance()->getStatus("displayMultipleDisplays") != "Hidden" ? enable : false);
        m_modeSettingsItem->setVisible(enable);
        m_primarySettingsItem->setVisible(enable);
    });
    connect(m_model, &DisplayModel::brightnessEnableChanged, this, [=](const bool enable) {
        m_brightnessSpacerItem->changeSize(0, enable ? 20 : 0);
        m_brightnessWidget->setVisible(enable);
    });

    connect(m_monitorControlWidget, &MonitorControlWidget::requestMonitorPress, this, &MultiScreenWidget::onMonitorPress);
    connect(m_monitorControlWidget, &MonitorControlWidget::requestMonitorRelease, this, &MultiScreenWidget::onMonitorRelease);
    connect(m_monitorControlWidget, &MonitorControlWidget::requestRecognize, this, &MultiScreenWidget::requestRecognize);
    connect(m_monitorControlWidget, &MonitorControlWidget::requestSetMonitorPosition, this, &MultiScreenWidget::onRequestSetMonitorPosition);
    connect(m_monitorControlWidget, &MonitorControlWidget::requestGatherWindows, this, &MultiScreenWidget::onGatherWindows);
    connect(this, &MultiScreenWidget::requestGatherEnabled, m_monitorControlWidget, &MonitorControlWidget::onGatherEnabled);

    connect(m_modeCombox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int idx) {
        if (idx <= 1 && m_model->displayMode() != idx + 1) {
            m_monitorControlWidget->setMergeMode((idx == 0)? true: false);
            Q_EMIT requestSwitchMode(idx + 1);
        } else if (idx > 1 && (m_model->displayMode() != SINGLE_MODE || (m_model->monitorList()[idx - 2]->name() != m_model->primary() && !m_model->primary().isEmpty()))) {
            m_monitorControlWidget->setMergeMode(false);
            Q_EMIT requestSwitchMode(SINGLE_MODE, m_model->monitorList()[idx - 2]->name());
        }
    });
    connect(m_primaryCombox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int idx) {
        Q_EMIT requestSetPrimary(m_primaryCombox->itemText(idx));
    });

    connect(m_brightnessWidget, &BrightnessWidget::requestSetColorTemperature, this, &MultiScreenWidget::requestSetColorTemperature);
    connect(m_brightnessWidget, &BrightnessWidget::requestSetMonitorBrightness, this, &MultiScreenWidget::requestSetMonitorBrightness);
    connect(m_brightnessWidget, &BrightnessWidget::requestAmbientLightAdjustBrightness, this, &MultiScreenWidget::requestAmbientLightAdjustBrightness);
    connect(m_brightnessWidget, &BrightnessWidget::requestSetMethodAdjustCCT, this, &MultiScreenWidget::requestSetMethodAdjustCCT);
    connect(m_scalingWidget, &ScalingWidget::requestUiScaleChange, this, &MultiScreenWidget::requestUiScaleChange);
    connect(m_resolutionWidget, &ResolutionWidget::requestSetResolution, this, &MultiScreenWidget::requestSetResolution);
    connect(m_resolutionWidget, &ResolutionWidget::requestSetFillMode, this, &MultiScreenWidget::requestSetFillMode);
    connect(m_refreshRateWidget, &RefreshRateWidget::requestSetResolution, this, &MultiScreenWidget::requestSetResolution);
    connect(m_rotateWidget, &RotateWidget::requestSetRotate, this, &MultiScreenWidget::requestSetRotate);

    connect(this, &MultiScreenWidget::requestRecognize, this, &MultiScreenWidget::onRequestRecognize);

    m_monitorControlWidget->setScreensMerged(m_model->displayMode());
    m_monitorControlWidget->setModel(m_model, m_model->displayMode() == SINGLE_MODE ? m_model->primaryMonitor() : nullptr);
    m_multiSettingLabel->setVisible(m_model->resolutionRefreshEnable() && (GSettingWatcher::instance()->getStatus("displayMultipleDisplays") != "Hidden"));
    m_modeSettingsItem->setVisible(m_model->resolutionRefreshEnable());
    m_primarySettingsItem->setVisible(m_model->resolutionRefreshEnable());
    m_primarySettingsItem->setVisible(m_model->displayMode() == EXTEND_MODE);
    m_brightnessWidget->setMode(m_model);
    m_brightnessWidget->showBrightness(m_model->displayMode() == MERGE_MODE ? nullptr : m_model->primaryMonitor());
    m_brightnessWidget->setVisible(m_model->brightnessEnable());
    m_brightnessSpacerItem->changeSize(0, m_model->brightnessEnable() ? 20 : 0);
    m_scalingWidget->setModel(m_model);
    m_resolutionWidget->setModel(m_model, m_model->primaryMonitor());
    m_refreshRateWidget->setModel(m_model, m_model->primaryMonitor());
    m_rotateWidget->setModel(m_model, m_model->primaryMonitor());

    initSecondaryScreenDialog();
}

void MultiScreenWidget::initModeList()
{
    m_modeCombox->addItem(tr("Duplicate"));
    m_modeCombox->addItem(tr("Extend"));

    auto monitorList = m_model->monitorList();
    for (int idx = 0; idx < monitorList.size(); ++idx) {
        auto monitor = monitorList[idx];
        if(monitorList.size() <= 2)
            m_modeCombox->addItem(tr("Only on %1").arg(monitor->name()));

        if (m_model->displayMode() == MERGE_MODE) {
            m_modeCombox->setCurrentIndex(0);
        } else if (m_model->displayMode() == EXTEND_MODE) {
            m_modeCombox->setCurrentIndex(1);
        } else if (m_model->displayMode() == SINGLE_MODE && monitor->enable()) {
            m_modeCombox->setCurrentIndex(idx + 2);
        }
    }
}

void MultiScreenWidget::initPrimaryList()
{
    for (const auto &monitor : m_model->monitorList()) {
        m_primaryCombox->addItem(monitor->name());
        if (monitor->name() == m_model->primary()) {
            m_primaryCombox->setCurrentText(m_model->primary());
        }
    }
}

void MultiScreenWidget::initSecondaryScreenDialog()
{
    if (m_model->displayMode() == EXTEND_MODE) {
        for (auto dlg : m_secondaryScreenDlgList) {
            dlg->deleteLater();
        }
        m_secondaryScreenDlgList.clear();

        for (const auto &monitor : m_model->monitorList()) {
            if (monitor == m_model->primaryMonitor()) {
                QTimer::singleShot(0, this, [=] { requestSetMainwindowRect(m_model->primaryMonitor(), true); });
                continue;
            }

            SecondaryScreenDialog *dlg = new SecondaryScreenDialog(this);
            dlg->setAttribute(Qt::WA_WState_WindowOpacitySet);
            dlg->setModel(m_model, monitor);
            connect(dlg, &SecondaryScreenDialog::requestRecognize, this, &MultiScreenWidget::requestRecognize);
            connect(dlg, &SecondaryScreenDialog::requestSetMonitorBrightness, this, &MultiScreenWidget::requestSetMonitorBrightness);
            connect(dlg, &SecondaryScreenDialog::requestAmbientLightAdjustBrightness, this, &MultiScreenWidget::requestAmbientLightAdjustBrightness);
            connect(dlg, &SecondaryScreenDialog::requestSetResolution, this, &MultiScreenWidget::requestSetResolution);
            connect(dlg, &SecondaryScreenDialog::requestSetFillMode, this, &MultiScreenWidget::requestSetFillMode);
            connect(dlg, &SecondaryScreenDialog::requestSetRotate, this, &MultiScreenWidget::requestSetRotate);
            connect(dlg, &SecondaryScreenDialog::requestGatherWindows, this, &MultiScreenWidget::onGatherWindows);
            connect(dlg, &SecondaryScreenDialog::requestCloseRecognize, this, &MultiScreenWidget::onRequestCloseRecognize);
            connect(this, &MultiScreenWidget::requestGatherEnabled, dlg, &SecondaryScreenDialog::requestGatherEnabled);
            m_secondaryScreenDlgList.append(dlg);
        }

        activateWindow();
    }
}

void MultiScreenWidget::onGatherWindows(const QPoint cursor)
{
    Q_EMIT requestGatherEnabled(false);

    for (const auto &monitor : m_model->monitorList()) {
        auto mrt = monitor->rect();
        if (mrt.contains(cursor.x(), cursor.y())) {
            for (QWidget *w : qApp->topLevelWidgets()) {
                if (DMainWindow *mainWin = qobject_cast<DMainWindow *>(w)) {
                    auto rt = mainWin->rect();
                    if (rt.width() > monitor->w())
                        rt.setWidth(monitor->w());

                    if (rt.height() > monitor->h())
                        rt.setHeight(monitor->h());

                    auto tsize = (mrt.size() / m_model->monitorScale(monitor) - rt.size()) / 2;
                    rt.moveTo(monitor->x() + tsize.width(), monitor->y() + tsize.height());
                    mainWin->setGeometry(rt);
                }
            }

            for (auto dlg : m_secondaryScreenDlgList) {
                auto rt = dlg->rect();
                if (rt.width() > monitor->w())
                    rt.setWidth(monitor->w());

                if (rt.height() > monitor->h())
                    rt.setHeight(monitor->h());
                auto tsize = (mrt.size() / m_model->monitorScale(monitor) - rt.size()) / 2;
                rt.moveTo(monitor->x() + tsize.width(), monitor->y() + tsize.height());
                dlg->setGeometry(rt);
            }
            break;
        }
    }
}

void MultiScreenWidget::onMonitorPress(Monitor *monitor)
{
    m_fullIndication->setGeometry(monitor->rect());

    m_fullIndication->setVisible(true);

    QTimer::singleShot(1000, this, [=] { m_fullIndication->setVisible(false); });
}

void MultiScreenWidget::onMonitorRelease(Monitor *monitor)
{
    Q_UNUSED(monitor)
    m_fullIndication->setVisible(false);
    QTimer::singleShot(1000, this, [=] { requestSetMainwindowRect(m_model->primaryMonitor(), false); });
}

void MultiScreenWidget::onRequestSetMonitorPosition(QHash<dcc::display::Monitor *, QPair<int, int>> monitorPosition)
{
    Q_EMIT requestSetMonitorPosition(monitorPosition);
}

void MultiScreenWidget::onResetSecondaryScreenDlg()
{
    for (int i = 0; i < m_secondaryScreenDlgList.count(); ++i) {
        SecondaryScreenDialog *screenDialog = m_secondaryScreenDlgList.at(i);
        Q_ASSERT(screenDialog);
        screenDialog->setWindowOpacity(1);
        screenDialog->resetDialog();
    }
}

void MultiScreenWidget::onRequestRecognize()
{
    for(auto widget : m_recognizeWidget) {
        widget->deleteLater();
    }
    m_recognizeWidget.clear();

    // 复制模式
    if (m_model->displayMode() == MERGE_MODE) {
        QString text = m_model->monitorList().first()->name();
        for (int idx = 1; idx < m_model->monitorList().size(); idx++) {
            text += QString(" = %1").arg(m_model->monitorList()[idx]->name());
        }

        // 所在显示器不存在显示框
        if (m_recognizeWidget.value(text) == nullptr) {
            dcc::display::RecognizeWidget *widget = new dcc::display::RecognizeWidget(m_model->monitorList()[0], text);
            m_recognizeWidget[text] = widget;
        }
    } else { // 扩展模式
        for (auto monitor : m_model->monitorList()) {
            // 所在显示器不存在显示框
            if (m_recognizeWidget.value(monitor->name()) == nullptr) {
                dcc::display::RecognizeWidget *widget = new dcc::display::RecognizeWidget(monitor, monitor->name());
                m_recognizeWidget[monitor->name()] = widget;
            }
        }
    }

    this->setFocus(); //获取焦点响应键盘事件
}

void MultiScreenWidget::onRequestCloseRecognize()
{
    disconnect(this, &MultiScreenWidget::requestRecognize, this, &MultiScreenWidget::onRequestRecognize);

    for(auto widget : m_recognizeWidget) {
        widget->deleteLater();
    }
    m_recognizeWidget.clear();

    connect(this, &MultiScreenWidget::requestRecognize, this, &MultiScreenWidget::onRequestRecognize);
}

void MultiScreenWidget::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Escape) {
        onRequestCloseRecognize();
    }

    QWidget::keyPressEvent(e);
}
