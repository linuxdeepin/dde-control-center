//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "displaymodule.h"
#include "brightnesswidget.h"
#include "scalingwidget.h"
#include "resolutionwidget.h"
#include "refreshratewidget.h"
#include "rotatewidget.h"
#include "multiscreenwidget.h"
#include "timeoutdialog.h"

#include "operation/displaymodel.h"
#include "operation/displayworker.h"

#include <DMainWindow>
#include <DIconTheme>

#include <QApplication>
#include <QDesktopWidget>
#include <QVBoxLayout>

DGUI_USE_NAMESPACE
using namespace DCC_NAMESPACE;

QString DisplayPlugin::name() const
{
    return QStringLiteral("display");
}

ModuleObject * DisplayPlugin::module()
{
    //一级菜单--显示
    ModuleObject *moduleInterface = new PageModule();
    moduleInterface->setName("display");
    moduleInterface->setDisplayName(tr("Display"));
    moduleInterface->setDescription(tr("Light, resolution, scaling and etc"));
    moduleInterface->setIcon(DIconTheme::findQIcon("dcc_nav_display"));

    DisplayModule *displayModule = new DisplayModule(moduleInterface);
    moduleInterface->appendChild(displayModule);
    return moduleInterface;
}

QString DisplayPlugin::location() const
{
    return "2";
}

QWidget *DisplayModule::page()
{
    m_displayWidget = new QWidget();

    QVBoxLayout *contentLayout = new QVBoxLayout;
    contentLayout->setSpacing(0);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    m_displayWidget->setLayout(contentLayout);

    pushScreenWidget();

    return m_displayWidget;
}

DisplayModule::DisplayModule(QObject *parent)
    : ModuleObject(parent)
    , m_model(nullptr)
    , m_worker(nullptr)
    , m_displayWidget(nullptr)
{
    if (m_model) {
        delete m_model;
    }
    m_model = new DisplayModel(this);
    m_worker = new DisplayWorker(m_model, this);

    connect(m_model, &DisplayModel::monitorListChanged, this, [=] {
        pushScreenWidget();
    });

    //wayland下没有主屏和副屏之分 所以mainwindow窗体想要居中，直接使用QGuiApplication::primaryScreen()方法
    //获取的主屏的信息是错误的，所以不能直接使用。现在的方法是通过/com/deepin/daemon/Display服务获取主屏的名称
    //然后再在QGuiApplication::screens()中匹配，从而获取主屏的信息。
    connect(m_model, &DisplayModel::monitorListChanged, this, [=] {
        for (auto mon : m_model->monitorList()) {
            if (mon->isPrimary()) {
                setPrimaryScreen(mon->getQScreen());
            }
        }
    });

    connect(m_model, &DisplayModel::primaryScreenChanged, this, [=] (QString primary) {
        if (!primary.isEmpty()) {
            for (auto mon : m_model->monitorList()) {
                if (mon->name() == primary) {
                    setPrimaryScreen(mon->getQScreen());
                }
            }
        }
    });
}

DisplayModule::~DisplayModule()
{
}

void DisplayModule::active()
{
    m_worker->active();
}

void DisplayModule::deactive()
{
    m_displayWidget = nullptr;
    m_primaryScreen = nullptr;
}

void DisplayModule::showSingleScreenWidget()
{
    QWidget *singleScreenWidget = new QWidget();

    QVBoxLayout *contentLayout = new QVBoxLayout;
    contentLayout->setSpacing(0);
    contentLayout->setContentsMargins(0, 20, 0, 0);

    BrightnessWidget *brightnessWidget = new BrightnessWidget(singleScreenWidget);
    brightnessWidget->setMode(m_model);
    contentLayout->addWidget(brightnessWidget);
    const bool brightnessIsEnabled = m_model->brightnessEnable() && m_model->primaryMonitor() && m_model->primaryMonitor()->canBrightness();
    brightnessWidget->setVisible(brightnessIsEnabled);
    connect(brightnessWidget, &BrightnessWidget::requestSetColorTemperature, m_worker, &DisplayWorker::setColorTemperature);
    connect(brightnessWidget, &BrightnessWidget::requestSetMonitorBrightness, m_worker, &DisplayWorker::setMonitorBrightness);
    connect(brightnessWidget, &BrightnessWidget::requestAmbientLightAdjustBrightness, m_worker, &DisplayWorker::setAmbientLightAdjustBrightness);
    connect(brightnessWidget, &BrightnessWidget::requestSetMethodAdjustCCT, m_worker, &DisplayWorker::SetMethodAdjustCCT);

    QSpacerItem *scalingSpacerItem = new QSpacerItem(0, brightnessIsEnabled? 20 : 0);
    contentLayout->addSpacerItem(scalingSpacerItem);

    ScalingWidget *scalingWidget = new ScalingWidget(singleScreenWidget);
    scalingWidget->setModel(m_model);
    contentLayout->addWidget(scalingWidget);
    connect(scalingWidget, &ScalingWidget::requestUiScaleChange, m_worker, &DisplayWorker::setUiScale);

    QSpacerItem *resolutionSpacerItem = new QSpacerItem(0, 30);
    contentLayout->addSpacerItem(resolutionSpacerItem);

    ResolutionWidget *resolutionWidget = new ResolutionWidget(300, singleScreenWidget);
    resolutionWidget->setModel(m_model, m_model->monitorList().count() ? m_model->monitorList().first() : nullptr);
    contentLayout->addWidget(resolutionWidget);
    connect(resolutionWidget, &ResolutionWidget::requestSetResolution, this, [=](Monitor *monitor, const int mode) {
        onRequestSetResolution(monitor, mode);
        updateWinsize();
    }, Qt::QueuedConnection);

   connect(resolutionWidget, &ResolutionWidget::requestSetFillMode, this, [this](Monitor *monitor, const QString fillMode) {
        onRequestSetFillMode(monitor, fillMode);
   });

   RefreshRateWidget *refreshRateWidget = new RefreshRateWidget(300, singleScreenWidget);
   refreshRateWidget->setModel(m_model, m_model->monitorList().count() ? m_model->monitorList().first() : nullptr);
   contentLayout->addSpacing(20);
   contentLayout->addWidget(refreshRateWidget);
   connect(refreshRateWidget, &RefreshRateWidget::requestSetResolution, this, &DisplayModule::onRequestSetResolution, Qt::QueuedConnection);

   RotateWidget *rotateWidget = new RotateWidget(300, singleScreenWidget);
   rotateWidget->setModel(m_model, m_model->monitorList().count() ? m_model->monitorList().first() : nullptr);
   contentLayout->addSpacing(20);
   contentLayout->addWidget(rotateWidget);
   connect(rotateWidget, &RotateWidget::requestSetRotate, this, &DisplayModule::onRequestSetRotate, Qt::QueuedConnection);

   contentLayout->addStretch();

   singleScreenWidget->setLayout(contentLayout);

   if(m_displayWidget->layout()->count() > 0) {
       QWidget * w = m_displayWidget->layout()->itemAt(0)->widget();
       m_displayWidget->layout()->removeWidget(w);
       w->setParent(nullptr);
       delete w;
   }
   m_displayWidget->layout()->addWidget(singleScreenWidget);

   auto setBrightnessWidget = [brightnessWidget, scalingSpacerItem, this]() {
       const bool visible = m_model->brightnessEnable() && m_model->primaryMonitor() && m_model->primaryMonitor()->canBrightness();
       scalingSpacerItem->changeSize(0, visible ? 20 : 0);
       brightnessWidget->setVisible(visible);
   };

   connect(m_model, &DisplayModel::primaryScreenChanged, brightnessWidget ,setBrightnessWidget);
   connect(m_model, &DisplayModel::brightnessEnableChanged, brightnessWidget ,setBrightnessWidget);
}

void DisplayModule::updateWinsize(QRect rect)
{
    if (!m_displayWidget || !m_displayWidget->window()) {
        return;
    }

    DMainWindow* topWidget = qobject_cast<DMainWindow *>(m_displayWidget->window());

    if (!qApp->screens().contains(m_primaryScreen))
        return;

    if(!topWidget)
        return;

    if(topWidget->isMaximized())
        return;

    int w = m_primaryScreen->geometry().width();
    int h = m_primaryScreen->geometry().height();
    if (rect.width() && rect.height()) {
        w = rect.width();
        h = rect.height();
    }
    int WidgetMinimumWidth = qMin(w, 800);
    int WidgetMinimumHeight = qMin(h, 600);

    topWidget->setMinimumSize(QSize(WidgetMinimumWidth, WidgetMinimumHeight));

    topWidget->move(QPoint(m_primaryScreen->geometry().left() + (w - topWidget->geometry().width()) / 2,
                m_primaryScreen->geometry().top() + (h - topWidget->geometry().height()) / 2));
}

void DisplayModule::setPrimaryScreen(QScreen *screen)
{
    if(m_primaryScreen == screen)
        return;

    m_primaryScreen = screen;
    updateWinsize();
    connect(m_primaryScreen, &QScreen::geometryChanged, this, &DisplayModule::updateWinsize);
}

void DisplayModule::showMultiScreenWidget()
{
    connect(m_model, &DisplayModel::displayModeChanged, this, [this]() {
        onSetFillMode();
        updateWinsize();
    });

    MultiScreenWidget *multiScreenWidget = new MultiScreenWidget();
    multiScreenWidget->setModel(m_model);
    connect(multiScreenWidget, &MultiScreenWidget::requestSwitchMode, m_worker, &DisplayWorker::switchMode);
    connect(multiScreenWidget, &MultiScreenWidget::requestSetMonitorPosition, m_worker, &DisplayWorker::setMonitorPosition);
    connect(multiScreenWidget, &MultiScreenWidget::requestSetPrimary, m_worker, &DisplayWorker::setPrimary);
    connect(multiScreenWidget, &MultiScreenWidget::requestSetColorTemperature, m_worker, &DisplayWorker::setColorTemperature);
    connect(multiScreenWidget, &MultiScreenWidget::requestSetMonitorBrightness, m_worker, &DisplayWorker::setMonitorBrightness);
    connect(multiScreenWidget, &MultiScreenWidget::requestAmbientLightAdjustBrightness, m_worker, &DisplayWorker::setAmbientLightAdjustBrightness);
    connect(multiScreenWidget, &MultiScreenWidget::requestSetMethodAdjustCCT, m_worker, &DisplayWorker::SetMethodAdjustCCT);
    connect(multiScreenWidget, &MultiScreenWidget::requestUiScaleChange, m_worker, &DisplayWorker::setUiScale);

    connect(multiScreenWidget, &MultiScreenWidget::requestSetResolution, this, [=](Monitor *monitor, const int mode) {
        onRequestSetResolution(monitor, mode);
        updateWinsize();
    }, Qt::QueuedConnection);
    connect(multiScreenWidget, &MultiScreenWidget::requestSetFillMode, this, [this](Monitor *monitor, const QString fillMode) {
        onRequestSetFillMode(monitor, fillMode);
    });

    connect(multiScreenWidget, &MultiScreenWidget::requestCurrFillModeChanged, this, [this](Monitor *monitor, const QString currfillMode) {
        if (m_model->displayMode() == MERGE_MODE && monitor->isPrimary()) {
            for (auto m : m_model->monitorList())
                m_worker->setCurrentFillMode(m, currfillMode);
        }
    });

    connect(multiScreenWidget, &MultiScreenWidget::requestSetRotate, this, &DisplayModule::onRequestSetRotate, Qt::QueuedConnection);
    connect(multiScreenWidget, &MultiScreenWidget::requestSetMainwindowRect, this, [=](Monitor *moi,  bool isInit) {
        Q_UNUSED(moi)
        bool stateChanged = false;
        //窗口初始化且窗口最大化的时候不需要移动窗口
        if (!m_displayWidget->window()) {
            return;
        }

        if (m_displayWidget->window()->isMaximized()) {
            if(isInit){
                return;
            }
//            m_pMainWindow->setNeedRememberLastSize(false);
//            m_pMainWindow->showNormal();

//            QSize lastsize = m_pMainWindow->getLastSize();
//            if (!lastsize.isValid() || lastsize == m_pMainWindow->maximumSize()) {
//                lastsize.setWidth(m_pMainWindow->minimumWidth());
//                lastsize.setHeight(m_pMainWindow->minimumHeight());
//            }
//            m_pMainWindow->resize(lastsize);
            stateChanged = true;
        }

        if (stateChanged) {
            m_displayWidget->window()->showMaximized();
        }

        QScreen *screen = m_model->primaryMonitor()->getQScreen();
        if (qApp->screens().contains(screen)) {
            m_displayWidget->window()->setGeometry(QRect(screen->geometry().topLeft(),m_displayWidget->window()->size()));
            m_displayWidget->window()->move(QPoint(screen->geometry().left() + (screen->geometry().width() - m_displayWidget->window()->width()) / 2,
                        screen->geometry().top() + (screen->geometry().height() - m_displayWidget->window()->height()) / 2));
        }
    });

    if(m_displayWidget->layout()->count() > 0) {
        QWidget * w = m_displayWidget->layout()->itemAt(0)->widget();
        m_displayWidget->layout()->removeWidget(w);
        w->setParent(nullptr);
        delete w;
    }
    m_displayWidget->layout()->addWidget(multiScreenWidget);
}

void DisplayModule::onRequestSetResolution(Monitor *monitor, const uint mode)
{
    Resolution firstRes;
    QString lastFillMode = m_model->primaryMonitor()->currentFillMode();

    for (auto res : monitor->modeList()) {
        if (res.id() == mode) {
            firstRes = res;
            break;
        }
    }

    auto tfunc = [this](Monitor *tmon, Resolution tmode) {
        if (m_model->displayMode() == MERGE_MODE) {
            for (auto monitor : m_model->monitorList()) {
                bool bFind = false;
                for (auto res : monitor->modeList()) {
                    if (res == tmode) {
                        m_worker->setMonitorResolution(monitor, res.id());
                        bFind = true;
                        break;
                    }
                }
                if (!bFind) {
                    m_worker->setMonitorResolutionBySize(monitor, tmode.width(), tmode.height());
                }
            }
        } else {
            m_worker->setMonitorResolution(tmon, tmode.id());
        }

        // 扩展模式调整分辨率时会再次调整显示屏位置，此时会调用两次applyChanges接口，
        // 修改分辨率调用applyChanges后任务栏会响应分辨率改变信号，然后调整大小，造成部分界面显示到第二个屏幕
        // 只有一个屏幕时displayMode可能是EXTEND_MODE，同时需要判断显示数量
        if (m_model->displayMode() != EXTEND_MODE || m_model->monitorList().size() < 2)
            m_worker->applyChanges();
    };
    m_worker->backupConfig();
    tfunc(monitor, firstRes);

    //此处处理调用applyChanges的200ms延时, TimeoutDialog提前弹出的问题
    QTimer::singleShot(300, monitor, [this, monitor, lastFillMode]{
        if (showTimeoutDialog(monitor) == QDialog::Accepted) {
            m_worker->saveChanges();
        } else {
            m_worker->resetBackup();
        }
    });
}

void DisplayModule::onSetFillMode(QString currFullMode)
{
    if (m_model->primaryMonitor() == nullptr)
        return;

    if (currFullMode.isEmpty())
        currFullMode = m_model->primaryMonitor()->currentFillMode();

    //切分辨率时，如果是复制模式下，桌面显示不支持，全部切换为拉伸 否则以主屏为主
    if (m_model->displayMode() == MERGE_MODE) {
        for (auto m : m_model->monitorList()) {
                if(!m_model->allSupportFillModes())
                    m_worker->setCurrentFillMode(m, m_model->defaultFillMode());
                else
                    m_worker->setCurrentFillMode(m, currFullMode);
        }
    }
}

void DisplayModule::onRequestSetFillMode(Monitor *monitor, const QString fillMode)
{
    auto lastFillMode = monitor->currentFillMode();
    if (m_model->displayMode() == MERGE_MODE) {
        for (auto m : m_model->monitorList()) {
            m_worker->backupConfig();
            m_worker->setCurrentFillMode(m, fillMode);
        }
    } else {
        m_worker->backupConfig();
        m_worker->setCurrentFillMode(monitor, fillMode);
    }
    //桌面显示增加15秒倒计时功能
    QTimer::singleShot(300, monitor, [this, monitor, lastFillMode]{
        if (showTimeoutDialog(monitor, true) != QDialog::Accepted) {
            m_worker->resetBackup();
        }
        m_worker->clearBackup();
    });
}

void DisplayModule::onRequestSetRotate(Monitor *monitor, const int rotate)
{
    m_worker->backupConfig();
    m_worker->setMonitorRotate(monitor, rotate);
    m_worker->applyChanges();

    //此处处理调用applyChanges的200ms延时, TimeoutDialog提前弹出的问题
    QTimer::singleShot(300, monitor, [this, monitor]{
        if (showTimeoutDialog(monitor) == QDialog::Accepted) {
            m_worker->saveChanges();
        } else {
            // 若是重力感应 调整后不对数据进行保存
            if (monitor->currentRotateMode() != Monitor::RotateMode::Gravity) {
                m_worker->resetBackup();
            }
        }
    });
}

void DisplayModule::pushScreenWidget()
{
    if (!m_displayWidget)
        return;
    if (m_model->monitorList().size() > 1) {
        showMultiScreenWidget();
    } else {
        showSingleScreenWidget();
    }
}

int DisplayModule::showTimeoutDialog(Monitor *monitor, const bool isFillMode)
{
    TimeoutDialog *timeoutDialog = new TimeoutDialog(15);
    qreal radio = qApp->devicePixelRatio();
    QRectF rt(monitor->x(), monitor->y(), monitor->w() / radio, monitor->h() / radio);
    QTimer::singleShot(1, this, [=] { timeoutDialog->moveToCenterByRect(rt.toRect()); });
    // 若用户切换重力旋转 直接退出对话框
    if (!isFillMode) {
        connect(monitor, &Monitor::currentRotateModeChanged, timeoutDialog, &TimeoutDialog::close);
    }

    connect(monitor, &Monitor::geometryChanged, timeoutDialog, [=] {
        if (timeoutDialog) {
            QRectF rt(monitor->x(), monitor->y(), monitor->w() / radio, monitor->h() / radio);
            timeoutDialog->moveToCenterByRect(rt.toRect());
        }
    });
    connect(m_model, &DisplayModel::monitorListChanged, timeoutDialog, &TimeoutDialog::reject);

    return timeoutDialog->exec();
}

void DisplayModule::showDisplayRecognize()
{
    // 复制模式
    if (m_model->displayMode() == MERGE_MODE) {
        QString text = m_model->monitorList().first()->name();
        for (int idx = 1; idx < m_model->monitorList().size(); idx++) {
            text += QString(" = %1").arg(m_model->monitorList()[idx]->name());
        }

        // 所在显示器不存在显示框
        if (m_recognizeWidget.value(text) == nullptr) {
            RecognizeWidget *widget = new RecognizeWidget(m_model->monitorList()[0], text);
            QTimer::singleShot(5000, this, [=] {
                widget->deleteLater();
                m_recognizeWidget.remove(text);
            });
            m_recognizeWidget[text] = widget;
        }
    } else { // 扩展模式
        for (auto monitor : m_model->monitorList()) {
            // 所在显示器不存在显示框
            if (m_recognizeWidget.value(monitor->name()) == nullptr) {
                RecognizeWidget *widget = new RecognizeWidget(monitor, monitor->name());
                m_recognizeWidget[monitor->name()] = widget;
                QTimer::singleShot(5000, this, [=] {
                    widget->deleteLater();
                    m_recognizeWidget.remove(monitor->name());
                });
                m_recognizeWidget[monitor->name()] = widget;
            }
        }
    }
}
