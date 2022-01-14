/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     lq <longqi_cm@deepin.com>
 *
 * Maintainer: lq <longqi_cm@deepin.com>
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

#include "displaymodule.h"
#include "displaywidget.h"
#include "brightnesswidget.h"
#include "scalingwidget.h"
#include "resolutionwidget.h"
#include "refreshratewidget.h"
#include "rotatewidget.h"
#include "secondaryscreendialog.h"
#include "multiscreenwidget.h"
#include "window/utils.h"
#include "window/gsettingwatcher.h"
#include "widgets/timeoutdialog.h"
#include "modules/display/displaymodel.h"
#include "modules/display/displayworker.h"

#include <QApplication>
#include <QDesktopWidget>

using namespace dcc::display;
using namespace DCC_NAMESPACE::display;

DisplayModule::DisplayModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
    , m_displayModel(nullptr)
    , m_displayWorker(nullptr)
    , m_displayWidget(nullptr)
{
    // 用于传入MultiScreenWidget销毁副屏窗口
    m_pMainWindow = dynamic_cast<MainWindow *>(m_frameProxy);
}

DisplayModule::~DisplayModule()
{
    m_displayModel->deleteLater();
    m_displayWorker->deleteLater();

    GSettingWatcher::instance()->erase("displayResolution");
    GSettingWatcher::instance()->erase("displayRefreshRate");
    GSettingWatcher::instance()->erase("displayRotate");
}

void DisplayModule::windowUpdate()
{
    if (m_pMainWindow)
        m_pMainWindow->updateWinsize();
}

void DisplayModule::initialize()
{
    initSearchData();
}

const QString DisplayModule::name() const
{
    return QStringLiteral("display");
}

const QString DisplayModule::displayName() const
{
    return tr("Display");
}

void DisplayModule::active()
{
    m_displayWidget = new DisplayWidget(m_pMainWindow);
    m_displayWidget->setVisible(false);
    pushScreenWidget();
    m_frameProxy->pushWidget(this, m_displayWidget);
    QTimer::singleShot(0, this, [=] {
        if (m_frameProxy->currModule()->name() != name())
            return;
        m_displayWidget->setVisible(true);
    });
}

int DisplayModule::load(const QString &path)
{
    if (!m_displayWidget) {
        active();
    }

    return m_displayWidget->showPath(path);
}

void DisplayModule::preInitialize(bool sync, FrameProxyInterface::PushType pushtype)
{
    Q_UNUSED(pushtype);
    if (m_displayModel) {
        delete m_displayModel;
    }
    m_displayModel = new DisplayModel;
    m_displayWorker = new DisplayWorker(m_displayModel, nullptr, sync);

    m_displayModel->moveToThread(qApp->thread());
    m_displayWorker->moveToThread(qApp->thread());

    connect(m_displayModel, &DisplayModel::monitorListChanged, this, [=] {
        //当前从显示模块切到了其他模块, 会导致m_displayWidget不为空,但是也有些数据被delete, 容易访问空指针
        //加不在当前模块内就不处理显示器插拔的信号
        if (m_frameProxy->currModule() && m_frameProxy->currModule()->name() != name())
            return;
        if (m_displayWidget != nullptr) {
            pushScreenWidget();
        }
    });

    addChildPageTrans();
    initSearchData();

    QTimer::singleShot(0, m_displayWorker, [=] {
        m_displayWorker->active();
    });

    //wayland下没有主屏和副屏之分 所以mainwindow窗体想要居中，直接使用QGuiApplication::primaryScreen()方法
    //获取的主屏的信息是错误的，所以不能直接使用。现在的方法是通过/com/deepin/daemon/Display服务获取主屏的名称
    //然后再在QGuiApplication::screens()中匹配，从而获取主屏的信息。
    connect(m_displayModel, &DisplayModel::monitorListChanged, this, [=] {
        if (m_pMainWindow) {
            for (auto mon : m_displayModel->monitorList()) {
                if (mon->isPrimary()) {
                    m_pMainWindow->setPrimaryScreen(mon->getQScreen());
                }
            }
        }
    });

    connect(m_displayModel, &DisplayModel::primaryScreenChanged, this, [=] (QString primary) {
        if (m_pMainWindow && !primary.isEmpty()) {
            for (auto mon : m_displayModel->monitorList()) {
                if (mon->name() == primary) {
                    m_pMainWindow->setPrimaryScreen(mon->getQScreen());
                }
            }
        }
    });
}

QStringList DisplayModule::availPage() const
{
    QStringList sl;
    sl << "Resolution"
       << "Refresh Rate"
       << "Brightness";

    if (m_displayModel && m_displayModel->monitorList().size() > 1) {
        sl << "Multiple Displays";
    }

    if (IsServerSystem) {
        sl << "Display Scaling";
    }

    if (m_displayModel && !m_displayModel->touchscreenList().isEmpty()) {
        sl << "Touch Screen";
    }

    return sl;
}

void DisplayModule::addChildPageTrans() const
{
    if (m_frameProxy != nullptr) {
        //display
        m_frameProxy->addChildPageTrans("Brightness", tr("Brightness"));
        m_frameProxy->addChildPageTrans("Auto Brightness", tr("Auto Brightness"));
        m_frameProxy->addChildPageTrans("Color Temperature", tr("Color Temperature"));
        m_frameProxy->addChildPageTrans("Night Shift", tr("Night Shift"));
        m_frameProxy->addChildPageTrans("Change Color Temperature", tr("Change Color Temperature"));
        m_frameProxy->addChildPageTrans("Multiple Displays", tr("Multiple Displays"));
        m_frameProxy->addChildPageTrans("Mode", tr("Mode"));
        m_frameProxy->addChildPageTrans("Main Screen", tr("Main Screen"));
        m_frameProxy->addChildPageTrans("Refresh Rate", tr("Refresh Rate"));
        m_frameProxy->addChildPageTrans("Resolution", tr("Resolution"));
        m_frameProxy->addChildPageTrans("Rotation", tr("Rotation"));
        m_frameProxy->addChildPageTrans("Display Scaling", tr("Display Scaling"));
    }
}

void DisplayModule::showSingleScreenWidget()
{
    QWidget *singleScreenWidget = new QWidget(m_displayWidget);

    QVBoxLayout *contentLayout = new QVBoxLayout;
    contentLayout->setSpacing(0);
    contentLayout->setContentsMargins(56, 20, 56, 0);

    BrightnessWidget *brightnessWidget = new BrightnessWidget(singleScreenWidget);
    brightnessWidget->setMode(m_displayModel);
    contentLayout->addWidget(brightnessWidget);
    brightnessWidget->setVisible(m_displayModel->brightnessEnable());
    connect(brightnessWidget, &BrightnessWidget::requestSetColorTemperature, m_displayWorker, &DisplayWorker::setColorTemperature);
    connect(brightnessWidget, &BrightnessWidget::requestSetMonitorBrightness, m_displayWorker, &DisplayWorker::setMonitorBrightness);
    connect(brightnessWidget, &BrightnessWidget::requestAmbientLightAdjustBrightness, m_displayWorker, &DisplayWorker::setAmbientLightAdjustBrightness);
    connect(brightnessWidget, &BrightnessWidget::requestSetMethodAdjustCCT, m_displayWorker, &DisplayWorker::SetMethodAdjustCCT);

    QSpacerItem *scalingSpacerItem = new QSpacerItem(0, m_displayModel->brightnessEnable() ? 20 : 0);
    contentLayout->addSpacerItem(scalingSpacerItem);

    ScalingWidget *scalingWidget = new ScalingWidget(singleScreenWidget);
    scalingWidget->setModel(m_displayModel);
    contentLayout->addWidget(scalingWidget);
    connect(scalingWidget, &ScalingWidget::requestUiScaleChange, m_displayWorker, &DisplayWorker::setUiScale);

    QSpacerItem *resolutionSpacerItem = new QSpacerItem(0, 30);
    contentLayout->addSpacerItem(resolutionSpacerItem);

    ResolutionWidget *resolutionWidget = new ResolutionWidget(300, singleScreenWidget);
    resolutionWidget->setModel(m_displayModel, m_displayModel->monitorList().count() ? m_displayModel->monitorList().first() : nullptr);
    GSettingWatcher::instance()->bind("displayResolution", resolutionWidget);  // 使用GSettings来控制显示状态
    contentLayout->addWidget(resolutionWidget);
    connect(resolutionWidget, &ResolutionWidget::requestSetResolution, this, [=](dcc::display::Monitor *monitor, const int mode) {
        onRequestSetResolution(monitor, mode);
        windowUpdate();
    }, Qt::QueuedConnection);

   connect(resolutionWidget, &ResolutionWidget::requestSetFillMode, this, [this](dcc::display::Monitor *monitor, const QString fillMode) {
        onRequestSetFillMode(monitor, fillMode);
    });

    RefreshRateWidget *refreshRateWidget = new RefreshRateWidget(300, singleScreenWidget);
    refreshRateWidget->setModel(m_displayModel, m_displayModel->monitorList().count() ? m_displayModel->monitorList().first() : nullptr);
    GSettingWatcher::instance()->bind("displayRefreshRate", refreshRateWidget);  // 使用GSettings来控制显示状态
    contentLayout->addSpacing(20);
    contentLayout->addWidget(refreshRateWidget);
    connect(refreshRateWidget, &RefreshRateWidget::requestSetResolution, this, &DisplayModule::onRequestSetResolution, Qt::QueuedConnection);

    RotateWidget *rotateWidget = new RotateWidget(300, singleScreenWidget);
    rotateWidget->setModel(m_displayModel, m_displayModel->monitorList().count() ? m_displayModel->monitorList().first() : nullptr);
    GSettingWatcher::instance()->bind("displayRotate", rotateWidget);  // 使用GSettings来控制显示状态
    contentLayout->addSpacing(20);
    contentLayout->addWidget(rotateWidget);
    connect(rotateWidget, &RotateWidget::requestSetRotate, this, &DisplayModule::onRequestSetRotate, Qt::QueuedConnection);

    contentLayout->addStretch();

    singleScreenWidget->setLayout(contentLayout);
    m_displayWidget->setContent(singleScreenWidget);

    connect(m_displayModel, &DisplayModel::brightnessEnableChanged, this, [brightnessWidget, scalingSpacerItem](const bool enable) {
        scalingSpacerItem->changeSize(0, enable ? 20 : 0);
        brightnessWidget->setVisible(enable);
    });
}

void DisplayModule::showMultiScreenWidget()
{
    connect(m_displayModel, &DisplayModel::displayModeChanged, this, [this]() {
        onSetFillMode();
        windowUpdate();
    });

    MultiScreenWidget *multiScreenWidget = new MultiScreenWidget(m_pMainWindow);
    multiScreenWidget->setModel(m_displayModel);
    connect(multiScreenWidget, &MultiScreenWidget::requestSwitchMode, m_displayWorker, &DisplayWorker::switchMode);
    connect(multiScreenWidget, &MultiScreenWidget::requestSetMonitorPosition, m_displayWorker, &DisplayWorker::setMonitorPosition);
    connect(multiScreenWidget, &MultiScreenWidget::requestSetPrimary, m_displayWorker, &DisplayWorker::setPrimary);
    connect(multiScreenWidget, &MultiScreenWidget::requestSetColorTemperature, m_displayWorker, &DisplayWorker::setColorTemperature);
    connect(multiScreenWidget, &MultiScreenWidget::requestSetMonitorBrightness, m_displayWorker, &DisplayWorker::setMonitorBrightness);
    connect(multiScreenWidget, &MultiScreenWidget::requestAmbientLightAdjustBrightness, m_displayWorker, &DisplayWorker::setAmbientLightAdjustBrightness);
    connect(multiScreenWidget, &MultiScreenWidget::requestSetMethodAdjustCCT, m_displayWorker, &DisplayWorker::SetMethodAdjustCCT);
    connect(multiScreenWidget, &MultiScreenWidget::requestUiScaleChange, m_displayWorker, &DisplayWorker::setUiScale);
    connect(multiScreenWidget, &MultiScreenWidget::requestSetResolution, this, [=](dcc::display::Monitor *monitor, const int mode) {
        onRequestSetResolution(monitor, mode);
        windowUpdate();
    }, Qt::QueuedConnection);
    connect(multiScreenWidget, &MultiScreenWidget::requestSetFillMode, this, [this](dcc::display::Monitor *monitor, const QString fillMode) {
        onRequestSetFillMode(monitor, fillMode);
    });

    connect(multiScreenWidget, &MultiScreenWidget::requestCurrFillModeChanged, this, [this](dcc::display::Monitor *monitor, const QString currfillMode) {
        if (m_displayModel->displayMode() == MERGE_MODE && monitor->isPrimary()) {
            for (auto m : m_displayModel->monitorList())
                m_displayWorker->setCurrentFillMode(m, currfillMode);
        }
    });

    connect(multiScreenWidget, &MultiScreenWidget::requestSetRotate, this, &DisplayModule::onRequestSetRotate, Qt::QueuedConnection);
    connect(multiScreenWidget, &MultiScreenWidget::requestSetMainwindowRect, this, [=](Monitor *moi,  bool isInit) {
        bool stateChanged = false;
        //窗口初始化且窗口最大化的时候不需要移动窗口
        if (m_pMainWindow->isMaximized()) {
            if(isInit){
                return;
            }
            m_pMainWindow->setNeedRememberLastSize(false);
            m_pMainWindow->showNormal();

            QSize lastsize = m_pMainWindow->getLastSize();
            if (!lastsize.isValid() || lastsize == m_pMainWindow->maximumSize()) {
                lastsize.setWidth(m_pMainWindow->minimumWidth());
                lastsize.setHeight(m_pMainWindow->minimumHeight());
            }
            m_pMainWindow->resize(lastsize);
            stateChanged = true;
        }

        if (stateChanged) {
            m_pMainWindow->showMaximized();
        }

        QScreen *screen = m_displayModel->primaryMonitor()->getQScreen();
        m_pMainWindow->setGeometry(QRect(screen->geometry().topLeft(),m_pMainWindow->size()));
        m_pMainWindow->move(QPoint(screen->geometry().left() + (screen->geometry().width() - m_pMainWindow->width()) / 2,
                    screen->geometry().top() + (screen->geometry().height() - m_pMainWindow->height()) / 2));
    });

    m_displayWidget->setContent(multiScreenWidget);
}

void DisplayModule::onRequestSetResolution(Monitor *monitor, const int mode)
{
    Resolution lastRes = monitor->currentMode();
    Resolution firstRes;
    QString lastFillMode = m_displayModel->primaryMonitor()->currentFillMode();

    for (auto res : monitor->modeList()) {
        if (res.id() == mode) {
            firstRes = res;
            break;
        }
    }

    auto tfunc = [this](Monitor *tmon, Resolution tmode) {
        if (m_displayModel->displayMode() == MERGE_MODE) {
            for (auto monitor : m_displayModel->monitorList()) {
                bool bFind = false;
                for (auto res : monitor->modeList()) {
                    if (res == tmode) {
                        m_displayWorker->setMonitorResolution(monitor, res.id());
                        bFind = true;
                        break;
                    }
                }
                if (!bFind) {
                    m_displayWorker->setMonitorResolutionBySize(monitor, tmode.width(), tmode.height());
                }
            }
        } else {
            m_displayWorker->setMonitorResolution(tmon, tmode.id());
        }
        m_displayWorker->applyChanges();
    };

    tfunc(monitor, firstRes);

    //此处处理调用applyChanges的200ms延时, TimeoutDialog提前弹出的问题
    QTimer::singleShot(300, monitor, [this, tfunc, monitor, lastRes,lastFillMode]{
        if (showTimeoutDialog(monitor) == QDialog::Accepted) {
            m_displayWorker->saveChanges();
        } else {
            tfunc(monitor, lastRes);
            onSetFillMode(lastFillMode);
        }
    });
}

void DisplayModule::onSetFillMode(QString currFullMode)
{
    if (m_displayModel->primaryMonitor() == nullptr)
        return;

    if (currFullMode.isEmpty())
        currFullMode = m_displayModel->primaryMonitor()->currentFillMode();

    //切分辨率时，如果是复制模式下，桌面显示不支持，全部切换为拉伸 否则以主屏为主
    if (m_displayModel->displayMode() == MERGE_MODE) {
        for (auto m : m_displayModel->monitorList()) {
                if(!m_displayModel->allSupportFillModes())
                    m_displayWorker->setCurrentFillMode(m, m_displayModel->defaultFillMode());
                else
                    m_displayWorker->setCurrentFillMode(m, currFullMode);
        }
    }
}

void DisplayModule::onRequestSetFillMode(dcc::display::Monitor *monitor, const QString fillMode)
{
    auto lastFillMode = monitor->currentFillMode();
    if (m_displayModel->displayMode() == MERGE_MODE) {
        for (auto m : m_displayModel->monitorList())
            m_displayWorker->setCurrentFillMode(m, fillMode);
    } else {
        m_displayWorker->setCurrentFillMode(monitor, fillMode);
    }
    //桌面显示增加15秒倒计时功能
    QTimer::singleShot(300, monitor, [this, monitor, lastFillMode]{
        if (showTimeoutDialog(monitor, true) != QDialog::Accepted) {
            if (m_displayModel->displayMode() == MERGE_MODE) {
                for (auto m : m_displayModel->monitorList())
                    m_displayWorker->setCurrentFillMode(m, lastFillMode);
            }
            else {
               m_displayWorker->setCurrentFillMode(monitor, lastFillMode);
            }
        }
    });
}

void DisplayModule::onRequestSetRotate(Monitor *monitor, const int rotate)
{
    auto lastRotate = monitor->rotate();
    m_displayWorker->setMonitorRotate(monitor, rotate);
    m_displayWorker->applyChanges();

    //此处处理调用applyChanges的200ms延时, TimeoutDialog提前弹出的问题
    QTimer::singleShot(300, monitor, [this, monitor, lastRotate]{
        if (showTimeoutDialog(monitor) == QDialog::Accepted) {
            m_displayWorker->saveChanges();
        } else {
            // 若是重力感应 调整后不对数据进行保存
            if (monitor->currentRotateMode() != dcc::display::Monitor::RotateMode::Gravity) {
                m_displayWorker->setMonitorRotate(monitor, lastRotate);
                m_displayWorker->applyChanges();
            }
        }
    });
}

void DisplayModule::pushScreenWidget()
{
    if (m_displayModel->monitorList().size() > 1) {
        showMultiScreenWidget();
    } else {
        showSingleScreenWidget();
    }
}

int DisplayModule::showTimeoutDialog(Monitor *monitor, const bool isFillMode)
{
    QDesktopWidget *desktopwidget = QApplication::desktop();
    TimeoutDialog *timeoutDialog = new TimeoutDialog(15);
    qreal radio = qApp->devicePixelRatio();
    QRectF rt(monitor->x(), monitor->y(), monitor->w() / radio, monitor->h() / radio);
    QTimer::singleShot(1, this, [=] { timeoutDialog->moveToCenterByRect(rt.toRect()); });
    // 若用户切换重力旋转 直接退出对话框
    if (!isFillMode) {
        connect(monitor, &Monitor::currentRotateModeChanged, timeoutDialog, &TimeoutDialog::close);
    }

    connect(desktopwidget, &QDesktopWidget::resized, timeoutDialog, [=] {
        if (timeoutDialog) {
            QRectF rt(monitor->x(), monitor->y(), monitor->w() / radio, monitor->h() / radio);
            timeoutDialog->moveToCenterByRect(rt.toRect());
        }
    });
    connect(m_displayModel, &DisplayModel::monitorListChanged, timeoutDialog, &TimeoutDialog::deleteLater);

    return timeoutDialog->exec();
}

void DisplayModule::initSearchData()
{
    static QMap<QString, bool> gsettingsMap;

    const QStringList& gsSecondList { "brightnessEnable" };

    auto func_is_visible = [=](const QString &gsettings, bool state = true) {
        if ("" == gsettings) {
            return false;
        }

        bool ret = false;
        if (!state) {
            ret = GSettingWatcher::instance()->get(gsettings).toBool();
        } else {
            ret = GSettingWatcher::instance()->get(gsettings).toString() != "Hidden";
        }
        gsettingsMap.insert(gsettings, ret);
        return ret;
    };


    const QString& module = displayName();
    bool isMulti = m_displayModel->monitorList().size() > 1;

    auto func_mul_changed = [ = ](bool multi) {
        m_frameProxy->setWidgetVisible(module, tr("Mode"), multi && func_is_visible("displayMultipleDisplays"));
        m_frameProxy->setWidgetVisible(module, tr("Main Screen"), multi && func_is_visible("displayMultipleDisplays"));
        m_frameProxy->setWidgetVisible(module, tr("Multiple Displays"), multi && func_is_visible("displayMultipleDisplays"));
        m_frameProxy->updateSearchData(module);
    };

    auto func_brightnessEnable_changed = [ = ] {
        bool isBrightnessEnable = func_is_visible("brightnessEnable", false);
        qDebug() << Q_FUNC_INFO << "autoLightAdjustIsValid:::" << m_displayModel->autoLightAdjustIsValid();
        m_frameProxy->setWidgetVisible(module, tr("Auto Brightness"), isBrightnessEnable && func_is_visible("displayLightLighting")
                                       && m_displayModel->autoLightAdjustIsValid() && !IsServerSystem);
        m_frameProxy->setWidgetVisible(module, tr("Brightness"), isBrightnessEnable && func_is_visible("displayLightLighting"));

        m_frameProxy->setWidgetVisible(module, tr("Color Temperature"), isBrightnessEnable && func_is_visible("displayColorTemperature")  && m_displayModel->redshiftIsValid());
        m_frameProxy->setWidgetVisible(module, tr("Night Shift"), isBrightnessEnable && func_is_visible("displayColorTemperature") && m_displayModel->redshiftIsValid());
        m_frameProxy->setWidgetVisible(module, tr("Change Color Temperature"), isBrightnessEnable && func_is_visible("displayColorTemperature") && m_displayModel->redshiftIsValid());
    };

    auto func_process_all = [ = ]() {
        func_brightnessEnable_changed();

        m_frameProxy->setWidgetVisible(module, tr("Mode"), isMulti && func_is_visible("displayMultipleDisplays"));
        m_frameProxy->setWidgetVisible(module, tr("Main Screen"), isMulti && func_is_visible("displayMultipleDisplays"));
        m_frameProxy->setWidgetVisible(module, tr("Multiple Displays"), isMulti && func_is_visible("displayMultipleDisplays"));

        m_frameProxy->setWidgetVisible(module, tr("Refresh Rate"), func_is_visible("displayRefreshRate"));
        m_frameProxy->setWidgetVisible(module, tr("Resolution"), func_is_visible("displayResolution"));
        m_frameProxy->setWidgetVisible(module, tr("Rotation"), func_is_visible("displayRotate"));
        m_frameProxy->setWidgetVisible(module, tr("Display Scaling"), func_is_visible("displayScaling"));

        func_mul_changed(isMulti);
     };

    connect(GSettingWatcher::instance(), &GSettingWatcher::notifyGSettingsChanged, [=](const QString &gsetting, const QString &state) {
        if ("" == gsetting || !gsettingsMap.contains(gsetting)) {
            return;
        }

        if (gsSecondList.contains(gsetting)) {
            if ("brightnessEnable" == gsetting) {
                func_brightnessEnable_changed();
            }
        } else {
            if (gsettingsMap.value(gsetting) == (GSettingWatcher::instance()->get(gsetting).toString() != "Hidden")) {
                return;
            }

            if ("displayLightLighting" == gsetting) {
                func_brightnessEnable_changed();
            } else if ("displayColorTemperature" == gsetting) {
                func_brightnessEnable_changed();
            } else if ("displayMultipleDisplays" == gsetting) {
                m_frameProxy->setWidgetVisible(module, tr("Mode"), isMulti && func_is_visible("displayMultipleDisplays"));
                m_frameProxy->setWidgetVisible(module, tr("Main Screen"), isMulti && func_is_visible("displayMultipleDisplays"));
                m_frameProxy->setWidgetVisible(module, tr("Multiple Displays"), isMulti && func_is_visible("displayMultipleDisplays"));
            } else if ("displayRefreshRate" == gsetting) {
                m_frameProxy->setWidgetVisible(module, tr("Refresh Rate"), func_is_visible("displayRefreshRate"));
            } else if ("displayResolution" == gsetting) {
                m_frameProxy->setWidgetVisible(module, tr("Resolution"), func_is_visible("displayResolution"));
            } else if ("displayRotate" == gsetting) {
                m_frameProxy->setWidgetVisible(module, tr("Rotation"), func_is_visible("displayRotate"));
            } else if ("displayScaling" == gsetting) {
                m_frameProxy->setWidgetVisible(module, tr("Display Scaling"), func_is_visible("displayScaling"));
            } else {
                qWarning() << " not contains the gsettings : " << gsetting << state;
                return;
            }
        }

        qInfo() << " [notifyGSettingsChanged]  gsetting, state :" << gsetting << state;
        m_frameProxy->updateSearchData(module);
    });

    connect(m_displayModel, &DisplayModel::monitorListChanged, this, [ = ]() {
        int count = m_displayModel->monitorList().size();
        qInfo() << " [monitorListChanged] screen count : " << count;
        func_mul_changed(count > 1);
    });

    connect(m_displayModel, &DisplayModel::autoLightAdjustVaildChanged, this, [ = ](bool enable) {
        bool isBrightnessEnable = func_is_visible("brightnessEnable", false);
        m_frameProxy->setWidgetVisible(module, tr("Auto Brightness"), isBrightnessEnable && func_is_visible("displayLightLighting") && enable);
    });

    connect(m_displayModel, &DisplayModel::redshiftVaildChanged, this, [ = ] (bool valid) {
        m_frameProxy->setWidgetVisible(module, tr("Color Temperature"), func_is_visible("brightnessEnable", false) && func_is_visible("displayColorTemperature"));
        m_frameProxy->setWidgetVisible(module, tr("Night Shift"), func_is_visible("brightnessEnable", false) && func_is_visible("displayColorTemperature"));
        m_frameProxy->setWidgetVisible(module, tr("Change Color Temperature"), func_is_visible("brightnessEnable", false) && func_is_visible("displayColorTemperature"));
    });

    func_process_all();
}

void DisplayModule::showDisplayRecognize()
{
    // 复制模式
    if (m_displayModel->displayMode() == MERGE_MODE) {
        QString text = m_displayModel->monitorList().first()->name();
        for (int idx = 1; idx < m_displayModel->monitorList().size(); idx++) {
            text += QString(" = %1").arg(m_displayModel->monitorList()[idx]->name());
        }

        // 所在显示器不存在显示框
        if (m_recognizeWidget.value(text) == nullptr) {
            RecognizeWidget *widget = new RecognizeWidget(m_displayModel->monitorList()[0], text);
            QTimer::singleShot(5000, this, [=] {
                widget->deleteLater();
                m_recognizeWidget.remove(text);
            });
            m_recognizeWidget[text] = widget;
        }
    } else { // 扩展模式
        for (auto monitor : m_displayModel->monitorList()) {
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
