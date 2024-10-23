//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later


#include "multiscreenwidget.h"
#include "brightnesswidget.h"
#include "scalingwidget.h"
#include "resolutionwidget.h"
#include "refreshratewidget.h"
#include "rotatewidget.h"
#include "secondaryscreendialog.h"
#include "widgets/settingsitem.h"
#include "monitorcontrolwidget.h"
#include "monitorindicator.h"
#include "recognizewidget.h"
#include "operation/displaymodel.h"

#include <QVBoxLayout>
#include <QComboBox>
#include <QTimer>
#include <QApplication>
#include <QDesktopWidget>
#include <QKeyEvent>
#include <DMainWindow>
#include <DSizeMode>

#include <WayQtUtils.hpp>

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE

const int ComboxWidth = 300;

MultiScreenWidget::MultiScreenWidget(QWidget *parent)
    : QWidget(parent)
    , m_contentLayout(new QVBoxLayout(this))
    , m_monitorControlWidget(new MonitorControlWidget(240, this)) //根据产品设计更改出事高度为240
    , m_fullIndication(new MonitorIndicator(this))
    , m_modeSettingsItem(new SettingsItem(this))
    , m_modeCombox(new QComboBox(this))
    , m_primarySettingsItem(new SettingsItem(this))
    , m_primaryCombox(new QComboBox(this))
    , m_brightnessWidget(new BrightnessWidget(this))
    , m_scalingWidget(new ScalingWidget(this))
    , m_resolutionWidget(new ResolutionWidget(300, this))
    , m_refreshRateWidget(new RefreshRateWidget(300, this))
    , m_rotateWidget(new RotateWidget(300, this))
    , m_model(nullptr)
    , m_resetSecondaryScreenDlgTimer(new QTimer(this))
{
    //初始化列表无法进行静态翻译
    //~ contents_path /display/Multiple Displays
    m_multiSettingLabel = new TitleLabel(tr("Multiple Displays"), this);
    //~ contents_path /display/Mode
    m_modeLabel = new QLabel(tr("Mode"), this);
    //~ contents_path /display/Main Scree
    m_primaryLabel = new QLabel(tr("Main Screen"), this);

    m_monitorControlWidget->setAccessibleName("MultiScreenWidget_monitorControl");
    m_fullIndication->setAccessibleName("fullIndication");

    m_contentLayout->setSpacing(Dtk::Widget::DSizeModeHelper::element(6, 10));
    m_contentLayout->setContentsMargins(0, 20, 0, 0);
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
    m_contentLayout->addWidget(m_primarySettingsItem);

    m_contentLayout->addWidget(m_brightnessWidget);
    m_contentLayout->addWidget(m_scalingWidget);
    m_contentLayout->addWidget(m_resolutionWidget);
    m_contentLayout->addWidget(m_refreshRateWidget);
    m_contentLayout->addWidget(m_rotateWidget);
    m_contentLayout->addStretch();

    setLayout(m_contentLayout);

    QDesktopWidget *desktopwidget = QApplication::desktop();
    connect(desktopwidget,SIGNAL(workAreaResized(int)),this,SLOT(onResetSecondaryScreenDlg()));

    m_resetSecondaryScreenDlgTimer->setSingleShot(true);
    m_resetSecondaryScreenDlgTimer->setInterval(100);
    connect(m_resetSecondaryScreenDlgTimer, &QTimer::timeout, this, &MultiScreenWidget::onResetSecondaryScreenDlgTimerOut);
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
    delete m_fullIndication;
}

void MultiScreenWidget::setModel(DisplayModel *model)
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
                for (auto screen : QGuiApplication::screens()) {
                    disconnect(screen, &QScreen::geometryChanged, this, &MultiScreenWidget::onResetFullIndication);
                }
                QScreen *screen = m_model->primaryMonitor()->getQScreen();
                if(!screen)
                    continue;

                connect(screen, &QScreen::geometryChanged, this, &MultiScreenWidget::onResetFullIndication);

                m_fullIndication->setGeometry(screen->geometry());
                m_fullIndication->move(screen->geometry().topLeft());
                m_fullIndication->setVisible(true);
                QTimer::singleShot(1000, this, [=] { m_fullIndication->setVisible(false); });
                m_brightnessWidget->setVisible(monitor->canBrightness());
                break;
            }
        }

        Q_EMIT requestGatherEnabled(true);
        initSecondaryScreenDialog();
    });
    connect(m_model, &DisplayModel::brightnessEnableChanged, this, [=](const bool enable) {
        const bool visible = enable && m_model->primaryMonitor() && m_model->primaryMonitor()->canBrightness();
        m_brightnessWidget->setVisible(visible);
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
    connect(m_resolutionWidget, &ResolutionWidget::requestCurrFillModeChanged, this, &MultiScreenWidget::requestCurrFillModeChanged);
    connect(m_refreshRateWidget, &RefreshRateWidget::requestSetResolution, this, &MultiScreenWidget::requestSetResolution);
    connect(m_rotateWidget, &RotateWidget::requestSetRotate, this, &MultiScreenWidget::requestSetRotate);

    connect(this, &MultiScreenWidget::requestRecognize, this, &MultiScreenWidget::onRequestRecognize);

    m_monitorControlWidget->setScreensMerged(m_model->displayMode());
    m_monitorControlWidget->setModel(m_model, m_model->displayMode() == SINGLE_MODE ? m_model->primaryMonitor() : nullptr);

    m_brightnessWidget->setMode(m_model);
    m_brightnessWidget->showBrightness(m_model->displayMode() == MERGE_MODE ? nullptr : m_model->primaryMonitor());
    const bool brightnessIsEnabled = m_model->brightnessEnable() && m_model->primaryMonitor() && m_model->primaryMonitor()->canBrightness();
    m_brightnessWidget->setVisible(brightnessIsEnabled);
    m_scalingWidget->setModel(m_model);
    m_resolutionWidget->setModel(m_model, m_model->primaryMonitor());
    m_refreshRateWidget->setModel(m_model, m_model->primaryMonitor());
    m_rotateWidget->setModel(m_model, m_model->primaryMonitor());
    m_primarySettingsItem->setVisible(m_model->displayMode() == EXTEND_MODE);

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
    if (WQt::Utils::isTreeland()) {
        // FIXME(treeland): why this will break treeland
        return;
    }
    if (m_model->displayMode() == EXTEND_MODE) {
        m_resetSecondaryScreenDlgTimer->stop();
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
            connect(dlg, &SecondaryScreenDialog::requestCurrFillModeChanged, this, &MultiScreenWidget::requestCurrFillModeChanged);
            connect(dlg, &SecondaryScreenDialog::requestSetRotate, this, &MultiScreenWidget::requestSetRotate);
            connect(dlg, &SecondaryScreenDialog::requestGatherWindows, this, &MultiScreenWidget::onGatherWindows);
            connect(dlg, &SecondaryScreenDialog::requestCloseRecognize, this, &MultiScreenWidget::onRequestCloseRecognize);
            connect(this, &MultiScreenWidget::requestGatherEnabled, dlg, &SecondaryScreenDialog::requestGatherEnabled);
            m_secondaryScreenDlgList.append(dlg);
            
            dlg->show();
        }
        activateWindow();

        if (!qgetenv("WAYLAND_DISPLAY").isEmpty()) {
            m_resetSecondaryScreenDlgTimer->start();
        } else {
            onResetSecondaryScreenDlgTimerOut();
        }
    }
}

void MultiScreenWidget::onGatherWindows(const QPoint cursor)
{
    Q_EMIT requestGatherEnabled(false);
    for (const auto &monitor : m_model->monitorList()) {
        auto screen = monitor->getQScreen();
        auto mrt = screen->geometry();

        if (mrt.contains(cursor.x(), cursor.y())) {
            for (QWidget *w : qApp->topLevelWidgets()) {
                if (DMainWindow *mainWin = qobject_cast<DMainWindow *>(w)) {
                    auto rt = mainWin->rect();
                    if (rt.width() > screen->geometry().width())
                        rt.setWidth(screen->geometry().width());

                    if (rt.height() > screen->geometry().height())
                        rt.setHeight(screen->geometry().height());

                    auto tsize = (mrt.size() - rt.size()) / 2;
                    rt.moveTo(screen->geometry().topLeft().x() + tsize.width(), screen->geometry().topLeft().y() + tsize.height());
                    mainWin->setGeometry(rt);
                }
            }

            for (auto dlg : m_secondaryScreenDlgList) {
                auto rt = dlg->rect();
                if (rt.width() > screen->geometry().width())
                    rt.setWidth(screen->geometry().width());

                if (rt.height() > screen->geometry().height())
                    rt.setHeight(screen->geometry().height());

                auto tsize = (mrt.size() - rt.size()) / 2;
                rt.moveTo(screen->geometry().topLeft().x() + tsize.width(), screen->geometry().topLeft().y() + tsize.height());
                dlg->QDialog::setGeometry(rt);
                // 将副窗口置顶
                dlg->activateWindow();
            }
            break;
        }
    }
}

void MultiScreenWidget::onMonitorPress(Monitor *monitor)
{
    QScreen *screen = monitor->getQScreen();
    if(!screen)
        return;

    m_fullIndication->setGeometry(screen->geometry());
    m_fullIndication->move(screen->geometry().topLeft());
    m_fullIndication->setVisible(true);

    QTimer::singleShot(1000, this, [=] { m_fullIndication->setVisible(false); });
}

void MultiScreenWidget::onMonitorRelease(Monitor *monitor)
{
    Q_UNUSED(monitor)
    m_fullIndication->setVisible(false);
    QTimer::singleShot(2500, this, [=] { requestSetMainwindowRect(m_model->primaryMonitor(), false); });
}

void MultiScreenWidget::onRequestSetMonitorPosition(QHash<Monitor *, QPair<int, int>> monitorPosition)
{
    Q_EMIT requestSetMonitorPosition(monitorPosition);
}

void MultiScreenWidget::onResetFullIndication(const QRect &geometry)
{
    m_fullIndication->setGeometry(geometry);
    m_fullIndication->move(geometry.topLeft());
}

void MultiScreenWidget::onResetSecondaryScreenDlgTimerOut()
{
    for (auto dlg : m_secondaryScreenDlgList) {
        dlg->resetDialog();
    }
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
            RecognizeWidget *widget = new RecognizeWidget(m_model->monitorList()[0], text);
            m_recognizeWidget[text] = widget;
        }
    } else { // 扩展模式
        for (auto monitor : m_model->monitorList()) {
            // 所在显示器不存在显示框
            if (m_recognizeWidget.value(monitor->name()) == nullptr) {
                RecognizeWidget *widget = new RecognizeWidget(monitor, monitor->name());
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
