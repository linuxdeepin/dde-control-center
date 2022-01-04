/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <wangwei_cm@deepin.com>
 *
 * Maintainer: andywang <wangwei_cm@deepin.com>
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

#include "mousemodule.h"
#include "mousewidget.h"
#include "generalsettingwidget.h"
#include "mousesettingwidget.h"
#include "touchpadsettingwidget.h"
#include "trackpointsettingwidget.h"
#include "modules/mouse/mousemodel.h"
#include "modules/mouse/mouseworker.h"
#include "modules/mouse/mousedbusproxy.h"
#include "window/mainwindow.h"
#include "window/gsettingwatcher.h"

#include <QDebug>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::mouse;
using namespace dcc::mouse;

MouseModule::MouseModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
    , m_mouseWidget(nullptr)
    , m_generalSettingWidget(nullptr)
    , m_mouseSettingWidget(nullptr)
    , m_touchpadSettingWidget(nullptr)
    , m_trackPointSettingWidget(nullptr)
{
     m_pMainWindow = dynamic_cast<MainWindow *>(m_frameProxy);
     GSettingWatcher::instance()->insertState("mouseGeneral");
     GSettingWatcher::instance()->insertState("mouseMouse");
     GSettingWatcher::instance()->insertState("mouseTouch");
     GSettingWatcher::instance()->insertState("mouseTrackpoint");
}

void MouseModule::preInitialize(bool sync, FrameProxyInterface::PushType pushtype)
{
    Q_UNUSED(pushtype);
    Q_UNUSED(sync)
    //添加此判断是因为公共功能可能泄露。在分配指针“m_model”之前未释放它
    if (m_model) {
        delete m_model;
    }
    m_model  = new dcc::mouse::MouseModel(this);
    m_worker = new dcc::mouse::MouseWorker(m_model, this);
    m_dbusProxy = new dcc::mouse::MouseDBusProxy(m_worker, this);
    m_model->moveToThread(qApp->thread());
    m_worker->moveToThread(qApp->thread());
    m_dbusProxy->moveToThread(qApp->thread());
    m_dbusProxy->active();

    initSearchData();

    connect(m_model, &MouseModel::tpadExistChanged, this, [this](bool state) {
        qDebug() << "[Mouse] Touchpad , exist state : " << state;
        m_frameProxy->setRemoveableDeviceStatus(tr("Touchpad"), state);
    });//触控板

    connect(m_model, &MouseModel::redPointExistChanged, this, [this](bool state) {
        qDebug() << "[Mouse] TrackPoint , exist state : " << state;
        m_frameProxy->setRemoveableDeviceStatus(tr("TrackPoint"), state);
    });//指点杆
}

void MouseModule::initialize()
{

}

void MouseModule::reset()
{
    m_dbusProxy->onDefaultReset();
}

void MouseModule::active()
{
    m_mouseWidget = new MouseWidget;
    m_mouseWidget->setVisible(false);
    m_mouseWidget->init(m_model->tpadExist(), m_model->redPointExist());
    connect(m_model, &MouseModel::tpadExistChanged, m_mouseWidget, &MouseWidget::tpadExistChanged);
    connect(m_model, &MouseModel::redPointExistChanged, m_mouseWidget, &MouseWidget::redPointExistChanged);
    connect(m_mouseWidget, &MouseWidget::showGeneralSetting, this, &MouseModule::showGeneralSetting);
    connect(m_mouseWidget, &MouseWidget::showMouseSetting, this, &MouseModule::showMouseSetting);
    connect(m_mouseWidget, &MouseWidget::showTouchpadSetting, this, &MouseModule::showTouchpadSetting);
    connect(m_mouseWidget, &MouseWidget::showTrackPointSetting, this, &MouseModule::showTrackPointSetting);
    m_frameProxy->pushWidget(this, m_mouseWidget);
    m_mouseWidget->setVisible(true);
    m_mouseWidget->setDefaultWidget();
    connect(m_mouseWidget, &MouseWidget::requestUpdateSecondMenu, this, [ = ](const bool needPop) {
        if (m_pMainWindow->getcontentStack().size() >= 2 && needPop) {
            m_frameProxy->popWidget(this);
        }
        m_mouseWidget->setDefaultWidget();
    });
}

void MouseModule::showGeneralSetting()
{
    m_generalSettingWidget = new GeneralSettingWidget;
    m_generalSettingWidget->setVisible(false);
    m_generalSettingWidget->setModel(m_model);

    connect(m_generalSettingWidget, &GeneralSettingWidget::requestSetLeftHand, m_worker, &MouseWorker::onLeftHandStateChanged);
    connect(m_generalSettingWidget, &GeneralSettingWidget::requestSetDisTyping, m_worker, &MouseWorker::onDisTypingChanged);
    connect(m_generalSettingWidget, &GeneralSettingWidget::requestScrollSpeed, m_worker, &MouseWorker::onScrollSpeedChanged);
    connect(m_generalSettingWidget, &GeneralSettingWidget::requestSetDouClick, m_worker, &MouseWorker::onDouClickChanged);

    m_frameProxy->pushWidget(this, m_generalSettingWidget);
    m_generalSettingWidget->setVisible(true);
}

void MouseModule::showMouseSetting()
{
    m_mouseSettingWidget = new MouseSettingWidget;
    m_mouseSettingWidget->setVisible(false);
    m_mouseSettingWidget->setModel(m_model);

    connect(m_mouseSettingWidget, &MouseSettingWidget::requestSetMouseMotionAcceleration, m_worker, &MouseWorker::onMouseMotionAccelerationChanged);
    connect(m_mouseSettingWidget, &MouseSettingWidget::requestSetAccelProfile, m_worker, &MouseWorker::onAccelProfileChanged);
    connect(m_mouseSettingWidget, &MouseSettingWidget::requestSetDisTouchPad, m_worker, &MouseWorker::onDisTouchPadChanged);
    connect(m_mouseSettingWidget, &MouseSettingWidget::requestSetMouseNaturalScroll, m_worker, &MouseWorker::onMouseNaturalScrollStateChanged);

    m_frameProxy->pushWidget(this, m_mouseSettingWidget);
    m_mouseSettingWidget->setVisible(true);
}

void MouseModule::showTouchpadSetting()
{
    m_touchpadSettingWidget = new TouchPadSettingWidget;
    m_touchpadSettingWidget->setVisible(false);
    m_touchpadSettingWidget->setModel(m_model);

    connect(m_touchpadSettingWidget, &TouchPadSettingWidget::requestSetTouchpadMotionAcceleration, m_worker, &MouseWorker::onTouchpadMotionAccelerationChanged);
    connect(m_touchpadSettingWidget, &TouchPadSettingWidget::requestSetTapClick, m_worker, &MouseWorker::onTapClick);
    connect(m_touchpadSettingWidget, &TouchPadSettingWidget::requestSetTouchNaturalScroll, m_worker, &MouseWorker::onTouchNaturalScrollStateChanged);

    connect(m_touchpadSettingWidget, &TouchPadSettingWidget::requestDetectState, m_worker, &MouseWorker::onPalmDetectChanged);
    connect(m_touchpadSettingWidget, &TouchPadSettingWidget::requestContact, m_worker, &MouseWorker::onPalmMinWidthChanged);
    connect(m_touchpadSettingWidget, &TouchPadSettingWidget::requestPressure, m_worker, &MouseWorker::onPalmMinzChanged);

    m_frameProxy->pushWidget(this, m_touchpadSettingWidget);
    m_touchpadSettingWidget->setVisible(true);
}

void MouseModule::showTrackPointSetting()
{
    m_trackPointSettingWidget = new TrackPointSettingWidget;
    m_trackPointSettingWidget->setVisible(false);
    m_trackPointSettingWidget->setModel(m_model);
    connect(m_trackPointSettingWidget, &TrackPointSettingWidget::requestSetTrackPointMotionAcceleration, m_worker, &MouseWorker::onTrackPointMotionAccelerationChanged);
    m_frameProxy->pushWidget(this, m_trackPointSettingWidget);
    m_trackPointSettingWidget->setVisible(true);
}

const QString MouseModule::name() const
{
    return QStringLiteral("mouse");
}

const QString MouseModule::displayName() const
{
    return tr("Mouse");
}

int MouseModule::load(const QString &path)
{
    int hasPage = -1;
    QString loadPath = path.split("/").at(0);
    if (loadPath == QStringLiteral("General")) {
        hasPage = 0;
        m_mouseWidget->initSetting(0);
    } else if (loadPath == QStringLiteral("Mouse")) {
        hasPage = 0;
        m_mouseWidget->initSetting(1);
    } else if (loadPath == QStringLiteral("Touchpad")) {
        hasPage = 0;
        m_mouseWidget->initSetting(2);
    } else if (loadPath == QStringLiteral("TrackPoint")) {
        hasPage = 0;
        m_mouseWidget->initSetting(3);
    }

    return hasPage;
}

QStringList MouseModule::availPage() const
{
    QStringList sl;
    sl << "General" << "Mouse";
    if (m_model->tpadExist()) {
        sl << "Touchpad";
    }
    if (m_model->redPointExist()) {
        sl << "TrackPoint";
    }
    return sl;
}

void MouseModule::initSearchData()
{
    QString module = tr("Mouse");
    QString general = tr("General");
    QString mouse = tr("Mouse");
    QString touchpad = tr("Touchpad");
    QString trackPoint = tr("TrackPoint");
    QStringList boolGsList;
    boolGsList << "mouseGeneral" << "mouseMouse" << "mouseTrackpoint" << "mouseTouch";

    static QMap<QString, bool> gsettingsMap;

    auto func_is_visible = [=](const QString &gsettings, bool state = false) {
        if ("" == gsettings) {
            return false;
        }

        bool ret = false;
        QVariant value = GSettingWatcher::instance()->get(gsettings);
        if (state) {
            ret = value.toString() != "Hidden";
        } else {
            ret = value.toBool();
        }
        gsettingsMap.insert(gsettings, ret);
        return ret;
    };

    auto func_general_changed = [ = ]() {
        bool bGeneral = func_is_visible("mouseGeneral");
        m_frameProxy->setWidgetVisible(module, general, bGeneral);
        m_frameProxy->setDetailVisible(module, general, tr("Double-click Test"), bGeneral);
        m_frameProxy->setDetailVisible(module, general, tr("Left Hand"), bGeneral && func_is_visible("mouseLeftHand", true));
        m_frameProxy->setDetailVisible(module, general, tr("Disable touchpad while typing"), bGeneral && (m_model ? m_model->tpadExist() : true));
        m_frameProxy->setDetailVisible(module, general, tr("Scrolling Speed"), bGeneral);
        m_frameProxy->setDetailVisible(module, general, tr("Double-click Speed"), bGeneral);
    };

    auto func_mouse_changed = [ = ]() {
        bool bModule = func_is_visible("mouseMouse");
        m_frameProxy->setWidgetVisible(module, mouse, bModule);
        m_frameProxy->setDetailVisible(module, mouse, tr("Pointer Speed"), bModule && func_is_visible("mouseSpeedSlider", true));
        m_frameProxy->setDetailVisible(module, mouse, tr("Mouse Acceleration"), bModule);
        m_frameProxy->setDetailVisible(module, mouse, tr("Disable touchpad when a mouse is connected"), bModule && (m_model ? m_model->tpadExist() : true));
        m_frameProxy->setDetailVisible(module, mouse, tr("Natural Scrolling"), bModule);
    };

    auto func_touchpad_changed = [ = ](bool tpadExist) {
        bool bTouchpad = func_is_visible("mouseTouch") && tpadExist;
        m_frameProxy->setWidgetVisible(module, touchpad, bTouchpad);
        m_frameProxy->setDetailVisible(module, touchpad, tr("Pointer Speed"), bTouchpad);
        m_frameProxy->setDetailVisible(module, touchpad, tr("Tap to Click"), bTouchpad);
        m_frameProxy->setDetailVisible(module, touchpad, tr("Natural Scrolling"), bTouchpad);
    };

     auto func_trackPoint_changed = [ = ](bool redPointExist) {
         bool bTrackPoint = func_is_visible("mouseTrackpoint") && redPointExist;
         m_frameProxy->setWidgetVisible(module, trackPoint, bTrackPoint);
         m_frameProxy->setDetailVisible(module, trackPoint, tr("Pointer Speed"), bTrackPoint);
     };

    auto func_process_all = [ = ]() {

        m_frameProxy->setModuleVisible(module, true);

        func_general_changed();

        func_mouse_changed();

        func_touchpad_changed(m_model->tpadExist());

        func_trackPoint_changed(m_model->redPointExist());
     };

    connect(m_model, &MouseModel::redPointExistChanged, this, [ = ](bool state) {
        QTimer::singleShot(0, this, [ = ]() {
            func_trackPoint_changed(state);
            m_frameProxy->updateSearchData(module);
        });
    });
    connect(m_model, &MouseModel::tpadExistChanged, this, [ = ](bool state) {
        QTimer::singleShot(0, this, [ = ]() {
            func_touchpad_changed(state);
            m_frameProxy->updateSearchData(module);
        });
    });

    //mouseGeneral,mouseMouse,mouseTrackpoint,mouseTouch
    //mouseLeftHand, mouseTouchpad, mouseSpeedSlider(鼠标/指针速度)
    connect(GSettingWatcher::instance(), &GSettingWatcher::notifyGSettingsChanged, this, [ = ](const QString &gsetting, const QString &state) {
        if (!gsettingsMap.contains(gsetting) || !m_frameProxy) {
            return;
        }

        if (boolGsList.contains(gsetting)) {
            if (gsettingsMap.value(gsetting) == GSettingWatcher::instance()->get(gsetting).toBool()) {
                return;
            }

            if ("mouseGeneral" == gsetting) {
                func_general_changed();
            } else if ("mouseMouse" == gsetting) {
                func_mouse_changed();
            } else if ("mouseTrackpoint" == gsetting) {
                func_trackPoint_changed(m_model->redPointExist());
            } else if ("mouseTouch" == gsetting) {
                func_touchpad_changed(m_model->tpadExist());
            } else {
                qDebug() << " not contains the gsettings : " << gsetting << state;
                return;
            }
        } else {
            if (gsettingsMap.value(gsetting) == (state != "Hidden")) {
                return;
            }

            if ("mouseLeftHand" == gsetting) {
                m_frameProxy->setDetailVisible(module, general, tr("Left Hand"), func_is_visible("mouseGeneral") && func_is_visible("mouseLeftHand", true));
            } else if ("mouseSpeedSlider" == gsetting) {
                m_frameProxy->setDetailVisible(module, mouse, tr("Pointer Speed"), func_is_visible("mouseMouse") && func_is_visible("mouseSpeedSlider", true));
            } else {
                qDebug() << " not contains the gsettings : " << gsetting << state;
                return;
            }
        }

        qWarning() << " [notifyGSettingsChanged]  gsetting, state :" << gsetting << state;
        m_frameProxy->updateSearchData(module);
    });

    func_process_all();
}

void MouseModule::contentPopped(QWidget *const w)
{
    Q_UNUSED(w);
}
