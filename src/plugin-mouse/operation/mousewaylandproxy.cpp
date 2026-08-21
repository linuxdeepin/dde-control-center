// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "mousewaylandproxy.h"

#include "treelandinputmanager.h"

#include <dguiapplicationhelper.h>
#include <QLoggingCategory>

using namespace DCC_NAMESPACE;

Q_LOGGING_CATEGORY(lcMouseWaylandProxy, "dde.dcc.mouse.wayland")

MouseWaylandProxy::MouseWaylandProxy(QObject *parent)
    : QObject(parent)
{
}

void MouseWaylandProxy::active()
{
    qCDebug(lcMouseWaylandProxy) << "MouseWaylandProxy::active()";
    m_inputManager = new TreelandInputManager(this);

    connect(m_inputManager, &TreelandInputManager::mouseAvailableChanged, this,
            [this](bool avail) {
                qCDebug(lcMouseWaylandProxy) << "mouseAvailableChanged:" << avail;
                Q_EMIT mouseExistChanged(avail);
                if (avail) {
                    connectMouseSettings(m_inputManager->mouseSettings());
                } else {
                    disconnectMouseSettings();
                }
            });

    connect(m_inputManager, &TreelandInputManager::touchpadAvailableChanged, this,
            [this](bool avail) {
                qCDebug(lcMouseWaylandProxy) << "touchpadAvailableChanged:" << avail;
                Q_EMIT tpadExistChanged(avail);
                if (avail) {
                    connectTouchpadSettings(m_inputManager->touchpadSettings());
                } else {
                    disconnectTouchpadSettings();
                }
            });

    // 立即发送已有状态
    if (m_inputManager->mouseAvailable()) {
        qCDebug(lcMouseWaylandProxy) << "Mouse already available, connecting immediately";
        Q_EMIT mouseExistChanged(true);
        connectMouseSettings(m_inputManager->mouseSettings());
    }
    if (m_inputManager->touchpadAvailable()) {
        qCDebug(lcMouseWaylandProxy) << "Touchpad already available, connecting immediately";
        Q_EMIT tpadExistChanged(true);
        connectTouchpadSettings(m_inputManager->touchpadSettings());
    }
}

void MouseWaylandProxy::connectMouseSettings(TreelandMouseSettings *mouse)
{
    if (!mouse || m_connectedMouseSettings == mouse)
        return;
    disconnectMouseSettings();
    m_connectedMouseSettings = mouse;
    qCDebug(lcMouseWaylandProxy) << "Connecting to TreelandMouseSettings";

    connect(mouse, &TreelandMouseSettings::leftHandedChanged,
            this, &MouseWaylandProxy::emitSharedLeftHanded);
    connect(mouse, &TreelandMouseSettings::naturalScrollChanged,
            this, &MouseWaylandProxy::emitMouseNaturalScroll);
    connect(mouse, &TreelandMouseSettings::speedChanged,
            this, &MouseWaylandProxy::emitMouseSpeed);
    connect(mouse, &TreelandMouseSettings::scrollFactorChanged,
            this, &MouseWaylandProxy::emitSharedScrollFactor);
    connect(mouse, &TreelandMouseSettings::accelerationProfileChanged,
            this, [this](uint32_t profile) { Q_EMIT accelProfileChanged(profileToState(profile)); });

    emitSharedLeftHanded(mouse->leftHanded());
    emitMouseNaturalScroll(mouse->naturalScroll());
    emitMouseSpeed(mouse->speed());
    emitSharedScrollFactor(mouse->scrollFactor());
    Q_EMIT accelProfileChanged(profileToState(mouse->accelerationProfile()));
}

void MouseWaylandProxy::connectTouchpadSettings(TreelandTouchpadSettings *tpad)
{
    if (!tpad || m_connectedTouchpadSettings == tpad)
        return;
    disconnectTouchpadSettings();
    m_connectedTouchpadSettings = tpad;
    qCDebug(lcMouseWaylandProxy) << "Connecting to TreelandTouchpadSettings";

    connect(tpad, &TreelandTouchpadSettings::leftHandedChanged,
            this, [this](bool value) {
                if (shouldUseTouchpadSharedState())
                    emitSharedLeftHanded(value);
            });
    connect(tpad, &TreelandTouchpadSettings::naturalScrollChanged,
            this, &MouseWaylandProxy::emitTouchpadNaturalScroll);
    connect(tpad, &TreelandTouchpadSettings::speedChanged,
            this, &MouseWaylandProxy::emitTouchpadSpeed);
    connect(tpad, &TreelandTouchpadSettings::scrollFactorChanged,
            this, [this](double value) {
                if (shouldUseTouchpadSharedState())
                    emitSharedScrollFactor(value);
            });
    connect(tpad, &TreelandTouchpadSettings::sendEventsModeChanged,
            this, &MouseWaylandProxy::emitTouchpadMode);
    connect(tpad, &TreelandTouchpadSettings::disableWhileTypingChanged,
            this, [this](bool v) { Q_EMIT disTypingChanged(v); });
    connect(tpad, &TreelandTouchpadSettings::tapToClickChanged,
            this, [this](bool v) { Q_EMIT tapClickChanged(v); });

    if (shouldUseTouchpadSharedState()) {
        emitSharedLeftHanded(tpad->leftHanded());
        emitSharedScrollFactor(tpad->scrollFactor());
    }
    emitTouchpadNaturalScroll(tpad->naturalScroll());
    emitTouchpadSpeed(tpad->speed());
    emitTouchpadMode(tpad->sendEventsMode());
    Q_EMIT disTypingChanged(tpad->disableWhileTyping());
    Q_EMIT tapClickChanged(tpad->tapToClick());
}

void MouseWaylandProxy::disconnectMouseSettings()
{
    if (!m_connectedMouseSettings)
        return;
    qCDebug(lcMouseWaylandProxy) << "Disconnecting from TreelandMouseSettings";
    disconnect(m_connectedMouseSettings, nullptr, this, nullptr);
    m_connectedMouseSettings = nullptr;
}

void MouseWaylandProxy::disconnectTouchpadSettings()
{
    if (!m_connectedTouchpadSettings)
        return;
    qCDebug(lcMouseWaylandProxy) << "Disconnecting from TreelandTouchpadSettings";
    disconnect(m_connectedTouchpadSettings, nullptr, this, nullptr);
    m_connectedTouchpadSettings = nullptr;
}

bool MouseWaylandProxy::shouldUseTouchpadSharedState() const
{
    return m_connectedMouseSettings == nullptr;
}

void MouseWaylandProxy::emitSharedLeftHanded(bool value)
{
    if (m_leftHandedInitialized && m_leftHanded == value)
        return;
    m_leftHandedInitialized = true;
    m_leftHanded = value;
    Q_EMIT leftHandStateChanged(value);
}

void MouseWaylandProxy::emitMouseNaturalScroll(bool value)
{
    if (m_mouseNaturalScrollInitialized && m_mouseNaturalScroll == value)
        return;
    m_mouseNaturalScrollInitialized = true;
    m_mouseNaturalScroll = value;
    Q_EMIT mouseNaturalScrollStateChanged(value);
}

void MouseWaylandProxy::emitTouchpadNaturalScroll(bool value)
{
    if (m_touchpadNaturalScrollInitialized && m_touchpadNaturalScroll == value)
        return;
    m_touchpadNaturalScrollInitialized = true;
    m_touchpadNaturalScroll = value;
    Q_EMIT touchNaturalScrollStateChanged(value);
}

void MouseWaylandProxy::emitMouseSpeed(double value)
{
    const double libinputValue = fromInputMotionAccelration(value);
    if (m_mouseSpeedInitialized
        && (qFuzzyCompare(m_mouseSpeed, libinputValue) || qFuzzyIsNull(m_mouseSpeed - libinputValue))) {
        return;
    }
    m_mouseSpeedInitialized = true;
    m_mouseSpeed = libinputValue;
    Q_EMIT mouseMotionAccelerationChanged(m_mouseSpeed);
}

void MouseWaylandProxy::emitTouchpadSpeed(double value)
{
    const double libinputValue = fromInputMotionAccelration(value);
    if (m_touchpadSpeedInitialized
        && (qFuzzyCompare(m_touchpadSpeed, libinputValue) || qFuzzyIsNull(m_touchpadSpeed - libinputValue))) {
        return;
    }
    m_touchpadSpeedInitialized = true;
    m_touchpadSpeed = libinputValue;

    Q_EMIT touchpadMotionAccelerationChanged(m_touchpadSpeed);
}

void MouseWaylandProxy::emitSharedScrollFactor(double factor)
{
    if (m_scrollFactorInitialized && qFuzzyCompare(m_scrollFactor, factor))
        return;
    m_scrollFactorInitialized = true;
    m_scrollFactor = factor;
    Q_EMIT scrollSpeedChanged(scrollFactorToSpeed(factor));
}

void MouseWaylandProxy::emitTouchpadMode(uint32_t mode)
{
    const bool touchpadEnabled = isTouchpadEnabledInModel(mode);
    const bool disableWhenMouseExist = isDisableTouchpadWhenMouseExistEnabled(mode);

    if (!m_disTouchPadInitialized || m_disTouchPad != disableWhenMouseExist) {
        m_disTouchPadInitialized = true;
        m_disTouchPad = disableWhenMouseExist;
        Q_EMIT disTouchPadChanged(disableWhenMouseExist);
    }
    Q_EMIT tpadEnabledChanged(touchpadEnabled);
}

uint32_t MouseWaylandProxy::touchpadSendEventsMode(bool touchpadEnabled,
                                                   bool disableWhenMouseExist) const
{
    if (!touchpadEnabled)
        return TreelandTouchpadSettings::send_events_mode_disabled;

    return disableWhenMouseExist
            ? TreelandTouchpadSettings::send_events_mode_disabled_on_external_mouse
            : TreelandTouchpadSettings::send_events_mode_enabled;
}

// ---------------------------------------------------------------------------
// Setters
// ---------------------------------------------------------------------------

void MouseWaylandProxy::setLeftHandState(bool state)
{
    qCDebug(lcMouseWaylandProxy) << "setLeftHandState:" << state;
    if (auto *mouse = m_inputManager->mouseSettings())
        mouse->setLeftHanded(state);
    if (auto *tpad = m_inputManager->touchpadSettings())
        tpad->setLeftHanded(state);
}

void MouseWaylandProxy::setMouseNaturalScrollState(bool state)
{
    qCDebug(lcMouseWaylandProxy) << "setMouseNaturalScrollState:" << state;
    if (auto *mouse = m_inputManager->mouseSettings())
        mouse->setNaturalScroll(state);
}

void MouseWaylandProxy::setTouchNaturalScrollState(bool state)
{
    qCDebug(lcMouseWaylandProxy) << "setTouchNaturalScrollState:" << state;
    if (auto *tpad = m_inputManager->touchpadSettings())
        tpad->setNaturalScroll(state);
}

void MouseWaylandProxy::setDisTyping(bool state)
{
    qCDebug(lcMouseWaylandProxy) << "setDisTyping:" << state;
    if (auto *tpad = m_inputManager->touchpadSettings())
        tpad->setDisableWhileTyping(state);
}

void MouseWaylandProxy::setTapClick(bool state)
{
    qCDebug(lcMouseWaylandProxy) << "setTapClick:" << state;
    if (auto *tpad = m_inputManager->touchpadSettings())
        tpad->setTapToClick(state);
}

void MouseWaylandProxy::setMouseMotionAcceleration(double value)
{
    qCDebug(lcMouseWaylandProxy) << "setMouseMotionAcceleration: libinput=" << value
                                 << "-> treeland=" << toInputMotionAccelration(value);
    const double speed = toInputMotionAccelration(value);
    if (auto *mouse = m_inputManager->mouseSettings())
        mouse->setSpeed(speed);
}

void MouseWaylandProxy::setAccelProfile(bool state)
{
    qCDebug(lcMouseWaylandProxy) << "setAccelProfile: adaptive=" << state;
    if (auto *mouse = m_inputManager->mouseSettings())
        mouse->setAccelerationProfile(stateToProfile(state));
}

void MouseWaylandProxy::setTouchpadMotionAcceleration(double value)
{
    qCDebug(lcMouseWaylandProxy) << "setTouchpadMotionAcceleration: libinput=" << value
                                 << "-> treeland=" << toInputMotionAccelration(value);
    if (auto *tpad = m_inputManager->touchpadSettings())
        tpad->setSpeed(toInputMotionAccelration(value));
}

void MouseWaylandProxy::setScrollSpeed(uint speed)
{
    qCDebug(lcMouseWaylandProxy) << "setScrollSpeed:" << speed
                                 << "-> scrollFactor=" << speedToScrollFactor(speed);
    // X11 侧 WheelSpeed 是全局输入设备属性，Wayland 侧需同时同步到 mouse/touchpad。
    const double factor = speedToScrollFactor(speed);
    if (auto *mouse = m_inputManager->mouseSettings())
        mouse->setScrollFactor(factor);
    if (auto *tpad = m_inputManager->touchpadSettings())
        tpad->setScrollFactor(factor);
}

void MouseWaylandProxy::setTouchpadEnabled(bool state)
{
    qCDebug(lcMouseWaylandProxy) << "setTouchpadEnabled:" << state;
    if (auto *tpad = m_inputManager->touchpadSettings())
        tpad->setSendEventsMode(touchpadSendEventsMode(state, m_disTouchPad));
}

void MouseWaylandProxy::setDisableTouchPadWhenMouseExist(bool state)
{
    qCDebug(lcMouseWaylandProxy) << "setDisableTouchPadWhenMouseExist:" << state;
    m_disTouchPadInitialized = true;
    m_disTouchPad = state;
    if (auto *tpad = m_inputManager->touchpadSettings()) {
        const uint32_t currentMode = tpad->sendEventsMode();
        const bool touchpadEnabled = isTouchpadEnabledInModel(currentMode);
        tpad->setSendEventsMode(touchpadSendEventsMode(touchpadEnabled, state));
    }
}

void MouseWaylandProxy::refreshMouse()
{
    qCDebug(lcMouseWaylandProxy) << "refreshMouse: requesting mouse settings refresh";

    disconnectMouseSettings();
    disconnectTouchpadSettings();

    m_inputManager->refreshMouse();

    if (m_inputManager->mouseAvailable()) {
        connectMouseSettings(m_inputManager->mouseSettings());
    }
    if (m_inputManager->touchpadAvailable()) {
        connectTouchpadSettings(m_inputManager->touchpadSettings());
    }
}

// ---------------------------------------------------------------------------
// Conversion helpers
// ---------------------------------------------------------------------------

double MouseWaylandProxy::toInputMotionAccelration(double libinput)
{
    auto accel = 1 - libinput / 1.5;
    accel = qBound(-1.0, accel, 1.0);
    return accel;
}

double MouseWaylandProxy::fromInputMotionAccelration(double speed)
{
    return qBound(0.0, (1 - speed) * 1.5, 3.0);
}

uint MouseWaylandProxy::scrollFactorToSpeed(double factor)
{
    return static_cast<uint>(qBound(1.0, static_cast<double>(qRound(factor)), 10.0));
}

double MouseWaylandProxy::speedToScrollFactor(uint speed)
{
    return static_cast<double>(qBound(1u, speed, 10u));
}

bool MouseWaylandProxy::profileToState(uint32_t profile)
{
    return profile
            == static_cast<uint32_t>(TreelandMouseSettings::AccelerationProfile::Adaptive);
}

uint32_t MouseWaylandProxy::stateToProfile(bool state)
{
    return state
            ? static_cast<uint32_t>(TreelandMouseSettings::AccelerationProfile::Adaptive)
            : static_cast<uint32_t>(TreelandMouseSettings::AccelerationProfile::Flat);
}

bool MouseWaylandProxy::isTouchpadEnabledInModel(uint32_t mode) const
{
    return mode != TreelandTouchpadSettings::send_events_mode_disabled;
}

bool MouseWaylandProxy::isDisableTouchpadWhenMouseExistEnabled(uint32_t mode) const
{
    return mode == TreelandTouchpadSettings::send_events_mode_disabled_on_external_mouse;
}
