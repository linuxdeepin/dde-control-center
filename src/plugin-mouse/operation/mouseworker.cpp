//SPDX-FileCopyrightText: 2018 - 2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "mouseworker.h"

#include "mousedbusproxy.h"
#ifdef ENABLE_TREELAND_INPUT_MANAGER
#include "mousewaylandproxy.h"
#endif

#include <array>
#include <cmath>
#include <QLoggingCategory>

using namespace DCC_NAMESPACE;
const QString Service = "org.deepin.dde.InputDevices1";

Q_LOGGING_CATEGORY(lcMouseWorker, "dde.dcc.mouse.worker")

namespace {
constexpr std::array<double, 7> kMotionAccelerationLevels { 3.2, 2.3, 1.6, 1.0, 0.6, 0.3, 0.2 };
}

MouseWorker::MouseWorker(MouseModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_isTreelandSession(Dtk::Gui::DGuiApplicationHelper::testAttribute(
               Dtk::Gui::DGuiApplicationHelper::IsWaylandPlatform))
    , m_mouseProxy(new MouseDBusProxy(this))
    , m_treelandWorker(new TreeLandWorker(this))
{
    qCDebug(lcMouseWorker) << "MouseWorker: session type =" << (m_isTreelandSession ? "Treeland/Wayland" : "X11/DBus");
#ifdef ENABLE_TREELAND_INPUT_MANAGER
    if (m_isTreelandSession) {
        m_waylandProxy = new MouseWaylandProxy(this);
        qCDebug(lcMouseWorker) << "MouseWorker: MouseWaylandProxy created for treeland session";
    }
#endif
    bindProxySignals();
    bindRequestSignals();
#ifdef ENABLE_TREELAND_INPUT_MANAGER
    if (m_isTreelandSession) {
        qCDebug(lcMouseWorker) << "MouseWorker: activating Wayland proxy";
        m_waylandProxy->active();
    }
#endif
    QMetaObject::invokeMethod(m_mouseProxy, "active", Qt::QueuedConnection);
#ifdef Enable_Treeland
    m_treelandWorker->active();
#endif
}

void MouseWorker::active()
{
}

void MouseWorker::deactive()
{
}

void MouseWorker::init()
{
}

void MouseWorker::refreshMouse()
{
#ifdef ENABLE_TREELAND_INPUT_MANAGER
    if (m_waylandProxy) {
        qCDebug(lcMouseWorker) << "MouseWorker::refreshMouse()";
        m_waylandProxy->refreshMouse();
    }
#endif
}

void MouseWorker::bindProxySignals()
{
    // 设备存在状态：treeland 下由 waylandProxy 上报，DBus 侧跳过
    if (!m_isTreelandSession) {
        connect(m_mouseProxy, &MouseDBusProxy::mouseExistChanged, this, &MouseWorker::setMouseExist);
        connect(m_mouseProxy, &MouseDBusProxy::tpadExistChanged, this, &MouseWorker::setTpadExist);
        connect(m_mouseProxy, &MouseDBusProxy::tpadEnabledChanged, this, &MouseWorker::setTpadEnabled);
        connect(m_mouseProxy, &MouseDBusProxy::leftHandStateChanged, this, &MouseWorker::setLeftHandState);
        connect(m_mouseProxy, &MouseDBusProxy::mouseNaturalScrollStateChanged, this, &MouseWorker::setMouseNaturalScrollState);
        connect(m_mouseProxy, &MouseDBusProxy::touchNaturalScrollStateChanged, this, &MouseWorker::setTouchNaturalScrollState);
        connect(m_mouseProxy, &MouseDBusProxy::disTypingChanged, this, &MouseWorker::setDisTyping);
        connect(m_mouseProxy, &MouseDBusProxy::tapClickChanged, this, &MouseWorker::setTapClick);
        connect(m_mouseProxy, &MouseDBusProxy::douClickChanged, this, &MouseWorker::setDouClick);
        connect(m_mouseProxy, &MouseDBusProxy::mouseMotionAccelerationChanged, this, &MouseWorker::setMouseMotionAcceleration);
        connect(m_mouseProxy, &MouseDBusProxy::accelProfileChanged, this, &MouseWorker::setAccelProfile);
        connect(m_mouseProxy, &MouseDBusProxy::touchpadMotionAccelerationChanged, this, &MouseWorker::setTouchpadMotionAcceleration);
        connect(m_mouseProxy, &MouseDBusProxy::scrollSpeedChanged, this, &MouseWorker::setScrollSpeed);
    }
    #ifdef ENABLE_TREELAND_INPUT_MANAGER
        if (m_waylandProxy) {
        connect(m_waylandProxy, &MouseWaylandProxy::mouseExistChanged,
            this, &MouseWorker::setMouseExist);
        connect(m_waylandProxy, &MouseWaylandProxy::tpadExistChanged,
            this, &MouseWorker::setTpadExist);
        connect(m_waylandProxy, &MouseWaylandProxy::tpadEnabledChanged,
            this, &MouseWorker::setTpadEnabled);
        connect(m_waylandProxy, &MouseWaylandProxy::disTouchPadChanged,
            this, &MouseWorker::setDisTouchPad);
        connect(m_waylandProxy, &MouseWaylandProxy::leftHandStateChanged,
            this, &MouseWorker::setLeftHandState);
        connect(m_waylandProxy, &MouseWaylandProxy::mouseNaturalScrollStateChanged,
            this, &MouseWorker::setMouseNaturalScrollState);
        connect(m_waylandProxy, &MouseWaylandProxy::touchNaturalScrollStateChanged,
            this, &MouseWorker::setTouchNaturalScrollState);
        connect(m_waylandProxy, &MouseWaylandProxy::disTypingChanged,
            this, &MouseWorker::setDisTyping);
        connect(m_waylandProxy, &MouseWaylandProxy::tapClickChanged,
            this, &MouseWorker::setTapClick);
        connect(m_waylandProxy, &MouseWaylandProxy::mouseMotionAccelerationChanged,
            this, &MouseWorker::setMouseMotionAcceleration);
        connect(m_waylandProxy, &MouseWaylandProxy::accelProfileChanged,
            this, &MouseWorker::setAccelProfile);
        connect(m_waylandProxy, &MouseWaylandProxy::touchpadMotionAccelerationChanged,
            this, &MouseWorker::setTouchpadMotionAcceleration);
        connect(m_waylandProxy, &MouseWaylandProxy::scrollSpeedChanged,
            this, &MouseWorker::setScrollSpeed);
        }
    #endif
    if (!m_isTreelandSession)
        connect(m_mouseProxy, &MouseDBusProxy::disTouchPadChanged, this, &MouseWorker::setDisTouchPad);
    connect(m_mouseProxy, &MouseDBusProxy::redPointExistChanged, this, &MouseWorker::setRedPointExist);
    connect(m_mouseProxy, &MouseDBusProxy::trackPointMotionAccelerationChanged, this, &MouseWorker::setTrackPointMotionAcceleration);
    connect(m_mouseProxy, &MouseDBusProxy::palmDetectChanged, this, &MouseWorker::setPalmDetect);
    connect(m_mouseProxy, &MouseDBusProxy::palmMinWidthChanged, this, &MouseWorker::setPalmMinWidth);
    connect(m_mouseProxy, &MouseDBusProxy::palmMinzChanged, this, &MouseWorker::setPalmMinz);
    connect(m_mouseProxy, &MouseDBusProxy::gestureDataChanged, this, [this](const GestureData &data) {
        setGestureData(data);
        initFingerGestures();
    });
    connect(m_mouseProxy, &MouseDBusProxy::cursorSizeChanged, this, &MouseWorker::setCursorSize);
    connect(m_mouseProxy, &MouseDBusProxy::availableCursorSizesChanged, this, &MouseWorker::setAvailableCursorSizes);
    connect(m_mouseProxy, &MouseDBusProxy::lidIsPresentChanged, this, &MouseWorker::setLidIsPresent);
}

void MouseWorker::bindRequestSignals()
{
    connect(this, &MouseWorker::requestSetPalmDetect, m_mouseProxy, &MouseDBusProxy::setPalmDetect);
    connect(this, &MouseWorker::requestSetPalmMinWidth, m_mouseProxy, &MouseDBusProxy::setPalmMinWidth);
    connect(this, &MouseWorker::requestSetPalmMinz, m_mouseProxy, &MouseDBusProxy::setPalmMinz);
    connect(this, &MouseWorker::requestSetTrackPointMotionAcceleration, m_mouseProxy, &MouseDBusProxy::setTrackPointMotionAcceleration);
    connect(this, &MouseWorker::requestSetGesture, m_mouseProxy, &MouseDBusProxy::setGesture);
    connect(this, &MouseWorker::requestSetCursorSize, m_mouseProxy, &MouseDBusProxy::setCursorSize);
    // treeland 下由 MouseWaylandProxy 处理的写请求，X11 下走 DBus
    if (!m_isTreelandSession) {
        connect(this, &MouseWorker::requestSetDouClick, m_mouseProxy, &MouseDBusProxy::setDouClick);
        connect(this, &MouseWorker::requestSetScrollSpeed, m_mouseProxy, &MouseDBusProxy::setScrollSpeed);
        connect(this, &MouseWorker::requestSetLeftHandState, m_mouseProxy, &MouseDBusProxy::setLeftHandState);
        connect(this, &MouseWorker::requestSetMouseNaturalScrollState, m_mouseProxy, &MouseDBusProxy::setMouseNaturalScrollState);
        connect(this, &MouseWorker::requestSetTouchNaturalScrollState, m_mouseProxy, &MouseDBusProxy::setTouchNaturalScrollState);
        connect(this, &MouseWorker::requestSetDisTyping, m_mouseProxy, &MouseDBusProxy::setDisTyping);
        connect(this, &MouseWorker::requestSetDisTouchPad, m_mouseProxy, &MouseDBusProxy::setDisableTouchPadWhenMouseExist);
        connect(this, &MouseWorker::requestSetTapClick, m_mouseProxy, &MouseDBusProxy::setTapClick);
        connect(this, &MouseWorker::requestSetMouseMotionAcceleration, m_mouseProxy, &MouseDBusProxy::setMouseMotionAcceleration);
        connect(this, &MouseWorker::requestSetAccelProfile, m_mouseProxy, &MouseDBusProxy::setAccelProfile);
        connect(this, &MouseWorker::requestSetTouchpadMotionAcceleration, m_mouseProxy, &MouseDBusProxy::setTouchpadMotionAcceleration);
        connect(this, &MouseWorker::requestSetTouchpadEnabled, m_mouseProxy, &MouseDBusProxy::setTouchpadEnabled);
    }
#ifdef ENABLE_TREELAND_INPUT_MANAGER
    if (m_waylandProxy) {
        connect(this, &MouseWorker::requestSetLeftHandState,
                m_waylandProxy, &MouseWaylandProxy::setLeftHandState);
        connect(this, &MouseWorker::requestSetMouseNaturalScrollState,
                m_waylandProxy, &MouseWaylandProxy::setMouseNaturalScrollState);
        connect(this, &MouseWorker::requestSetTouchNaturalScrollState,
                m_waylandProxy, &MouseWaylandProxy::setTouchNaturalScrollState);
        connect(this, &MouseWorker::requestSetDisTyping,
                m_waylandProxy, &MouseWaylandProxy::setDisTyping);
        connect(this, &MouseWorker::requestSetDisTouchPad,
                m_waylandProxy, &MouseWaylandProxy::setDisableTouchPadWhenMouseExist);
        connect(this, &MouseWorker::requestSetTapClick,
                m_waylandProxy, &MouseWaylandProxy::setTapClick);
        connect(this, &MouseWorker::requestSetMouseMotionAcceleration,
                m_waylandProxy, &MouseWaylandProxy::setMouseMotionAcceleration);
        connect(this, &MouseWorker::requestSetAccelProfile,
                m_waylandProxy, &MouseWaylandProxy::setAccelProfile);
        connect(this, &MouseWorker::requestSetTouchpadMotionAcceleration,
                m_waylandProxy, &MouseWaylandProxy::setTouchpadMotionAcceleration);
        connect(this, &MouseWorker::requestSetScrollSpeed,
                m_waylandProxy, &MouseWaylandProxy::setScrollSpeed);
        connect(this, &MouseWorker::requestSetTouchpadEnabled,
                m_waylandProxy, &MouseWaylandProxy::setTouchpadEnabled);
    }
#endif
}

void MouseWorker::initFingerGestures()
{
    m_model->updateFigerAniPath();
}

void MouseWorker::setMouseExist(bool exist)
{
    m_model->setMouseExist(exist);
}

void MouseWorker::setTpadExist(bool exist)
{
    m_model->setTpadExist(exist);
}

void MouseWorker::setTpadEnabled(bool enabled)
{
    m_model->m_syncingFromBackend = true;
    m_model->setTapEnabled(enabled);
    m_model->m_syncingFromBackend = false;
}

void MouseWorker::setRedPointExist(bool exist)
{
    m_model->setRedPointExist(exist);
}

void MouseWorker::setLeftHandState(const bool state)
{
    m_model->m_syncingFromBackend = true;
    m_model->setLeftHandState(state);
    m_model->m_syncingFromBackend = false;
}

void MouseWorker::setMouseNaturalScrollState(const bool state)
{
    m_model->m_syncingFromBackend = true;
    m_model->setMouseNaturalScroll(state);
    m_model->m_syncingFromBackend = false;
}

void MouseWorker::setTouchNaturalScrollState(const bool state)
{
    m_model->m_syncingFromBackend = true;
    m_model->setTpadNaturalScroll(state);
    m_model->m_syncingFromBackend = false;
}

void MouseWorker::setDisTyping(const bool state)
{
    m_model->m_syncingFromBackend = true;
    m_model->setDisIfTyping(state);
    m_model->m_syncingFromBackend = false;
}

void MouseWorker::setDisTouchPad(const bool state)
{
    m_model->m_syncingFromBackend = true;
    m_model->setDisTpad(state);
    m_model->m_syncingFromBackend = false;
}

void MouseWorker::setTapClick(const bool state)
{
    m_model->m_syncingFromBackend = true;
    m_model->setTapClick(state);
    m_model->m_syncingFromBackend = false;
}

void MouseWorker::setDouClick(const int &value)
{
    m_model->m_syncingFromBackend = true;
    m_model->setDoubleSpeed(converToDoubleModel(value));
    m_model->m_syncingFromBackend = false;
}

void MouseWorker::setMouseMotionAcceleration(const double &value)
{
    m_model->m_syncingFromBackend = true;
    m_model->setMouseMoveSpeed(converToModelMotionAcceleration(value));
    m_model->m_syncingFromBackend = false;
}

void MouseWorker::setAccelProfile(const bool state)
{
    m_model->m_syncingFromBackend = true;
    m_model->setAccelProfile(state);
    m_model->m_syncingFromBackend = false;
}

void MouseWorker::setTouchpadMotionAcceleration(const double &value)
{
    m_model->m_syncingFromBackend = true;
    m_model->setTpadMoveSpeed(converToModelMotionAcceleration(value));
    m_model->m_syncingFromBackend = false;
}

void MouseWorker::setTrackPointMotionAcceleration(const double &value)
{
    m_model->setRedPointMoveSpeed(converToModelMotionAcceleration(value));
}

void MouseWorker::setPalmDetect(bool palmDetect)
{
    m_model->setPalmDetect(palmDetect);
}

void MouseWorker::setPalmMinWidth(int palmMinWidth)
{
    m_model->setPalmMinWidth(palmMinWidth);
}

void MouseWorker::setPalmMinz(int palmMinz)
{
    m_model->setPalmMinz(palmMinz);
}

void MouseWorker::setScrollSpeed(uint speed)
{
    m_model->m_syncingFromBackend = true;
    m_model->setScrollSpeed(speed);
    m_model->m_syncingFromBackend = false;
}

void MouseWorker::setGestureData(const GestureData &data)
{
    m_model->updateGesturesData(data);
}

void MouseWorker::setCursorSize(const int cursorSize)
{
    m_model->m_syncingFromBackend = true;
    m_model->setCursorSize(cursorSize);
    m_model->m_syncingFromBackend = false;
}

void MouseWorker::setAvailableCursorSizes(QList<int> sizes)
{
    m_model->setAvailableCursorSizes(sizes);
}

void MouseWorker::setLidIsPresent(bool lidIsPresent)
{
    m_model->setLidIsPresent(lidIsPresent);
}

void MouseWorker::onPalmDetectChanged(bool palmDetect)
{
    Q_EMIT requestSetPalmDetect(palmDetect);
}

void MouseWorker::onPalmMinWidthChanged(int palmMinWidth)
{
    Q_EMIT requestSetPalmMinWidth(palmMinWidth);
}

void MouseWorker::onPalmMinzChanged(int palmMinz)
{
    Q_EMIT requestSetPalmMinz(palmMinz);
}

void MouseWorker::onScrollSpeedChanged(int speed)
{
    Q_EMIT requestSetScrollSpeed(static_cast<uint>(speed));
}

void MouseWorker::onTouchpadEnabledChanged(const bool state)
{
    Q_EMIT requestSetTouchpadEnabled(state);
}

void MouseWorker::onCursorSizeChanged(const int cursorSize)
{
    Q_EMIT requestSetCursorSize(cursorSize);
#ifdef Enable_Treeland
    if (Dtk::Gui::DGuiApplicationHelper::testAttribute(
                Dtk::Gui::DGuiApplicationHelper::IsWaylandPlatform)) {
        m_treelandWorker->setCursorSize(cursorSize);
    }
#endif
}

void MouseWorker::onLeftHandStateChanged(const bool state)
{
    Q_EMIT requestSetLeftHandState(state);
}

void MouseWorker::onMouseNaturalScrollStateChanged(const bool state)
{
    Q_EMIT requestSetMouseNaturalScrollState(state);
}

void MouseWorker::onTouchNaturalScrollStateChanged(const bool state)
{
    Q_EMIT requestSetTouchNaturalScrollState(state);
}

void MouseWorker::onDisTypingChanged(const bool state)
{
    Q_EMIT requestSetDisTyping(state);
}

void MouseWorker::onDisTouchPadChanged(const bool state)
{
    Q_EMIT requestSetDisTouchPad(state);
}

void MouseWorker::onTapClick(const bool state)
{
    Q_EMIT requestSetTapClick(state);
}

void MouseWorker::onDouClickChanged(const int &value)
{
    Q_EMIT requestSetDouClick(converToDouble(value));
}

void MouseWorker::onMouseMotionAccelerationChanged(const int &value)
{
    Q_EMIT requestSetMouseMotionAcceleration(converToMotionAcceleration(value));
}

void MouseWorker::onAccelProfileChanged(const bool state)
{
    Q_EMIT requestSetAccelProfile(state);
}

void MouseWorker::onTouchpadMotionAccelerationChanged(const int &value)
{
    Q_EMIT requestSetTouchpadMotionAcceleration(converToMotionAcceleration(value));
}

void MouseWorker::onTrackPointMotionAccelerationChanged(const int &value)
{
    Q_EMIT requestSetTrackPointMotionAcceleration(converToMotionAcceleration(value));
}

int MouseWorker::converToDouble(int value)
{
    return 800 - value * 100;
}

int MouseWorker::converToDoubleModel(int value)
{
    return (800 - value) / 100;
}
//conver slider value to real value
double MouseWorker::converToMotionAcceleration(int value)
{
    if (value < 0 || value >= static_cast<int>(kMotionAccelerationLevels.size()))
        return 1.0;

    return kMotionAccelerationLevels[static_cast<size_t>(value)];
}
//conver real value to slider value
int MouseWorker::converToModelMotionAcceleration(double value)
{
    int nearestIndex = 3;
    double nearestDistance = std::abs(value - kMotionAccelerationLevels[nearestIndex]);

    for (size_t i = 0; i < kMotionAccelerationLevels.size(); ++i) {
        const double distance = std::abs(value - kMotionAccelerationLevels[i]);
        if (distance < nearestDistance) {
            nearestDistance = distance;
            nearestIndex = static_cast<int>(i);
        }
    }

    return nearestIndex;
}
