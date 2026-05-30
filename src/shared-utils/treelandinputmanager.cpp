// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "treelandinputmanager.h"

#include <QGuiApplication>
#include <QLoggingCategory>
#include <wayland-client.h>

// Note: qwayland-treeland-input-manager-unstable-v1.h is already included
// transitively via treelandinputmanager.h.

namespace DCC_NAMESPACE {

Q_LOGGING_CATEGORY(lcTreelandInput, "dde.dcc.treeland.input")

namespace {

// device_type bitfield values from the protocol enum
constexpr uint32_t kMouseBit    = TREELAND_INPUT_MANAGER_V1_DEVICE_TYPE_MOUSE;
constexpr uint32_t kTouchpadBit = TREELAND_INPUT_MANAGER_V1_DEVICE_TYPE_TOUCHPAD;
constexpr uint32_t kKeyboardBit = TREELAND_INPUT_MANAGER_V1_DEVICE_TYPE_KEYBOARD;

template<typename T>
bool updateValue(T &current, const T &next)
{
    if (current == next)
        return false;
    current = next;
    return true;
}

} // namespace

class TreelandInputManagerPrivate
{
public:
    explicit TreelandInputManagerPrivate(TreelandInputManager *qq)
        : q(qq)
    {
    }

    ~TreelandInputManagerPrivate()
    {
        shutdown();
    }

    void initialize();
    void flush() const;
    void shutdown();

    void onCapabilityAvailable(uint32_t type, wl_seat *capSeat);
    void onCapabilityUnavailable(uint32_t type, wl_seat *capSeat);

    void ensureMouseSettings();
    void releaseMouseSettings();
    void ensureTouchpadSettings();
    void releaseTouchpadSettings();
    void ensureKeyboardSettings();
    void releaseKeyboardSettings();
    void refreshMouse();
    void refreshKeyboard();

public:
    TreelandInputManager *q = nullptr;
    wl_display *display = nullptr;
    wl_seat *seat = nullptr;

    bool initialized = false;
    bool shutDown = false;
    bool mouseAvail = false;
    bool touchpadAvail = false;
    bool keyboardAvail = false;

    TreelandMouseSettings *mouseSettings = nullptr;
    TreelandTouchpadSettings *touchpadSettings = nullptr;
    TreelandKeyboardSettings *keyboardSettings = nullptr;
};

// ---------------------------------------------------------------------------
// Private implementation
// ---------------------------------------------------------------------------

void TreelandInputManagerPrivate::initialize()
{
    if (initialized)
        return;

    initialized = true;

    auto *waylandApp = qGuiApp->nativeInterface<QNativeInterface::QWaylandApplication>();
    if (!waylandApp) {
        qCWarning(lcTreelandInput) << "No Wayland application interface available, treeland input manager disabled";
        return;
    }

    display = waylandApp->display();
    if (!display) {
        qCWarning(lcTreelandInput) << "Failed to get Wayland display";
        return;
    }
}

void TreelandInputManagerPrivate::flush() const
{
    if (display)
        wl_display_flush(display);
}

void TreelandInputManagerPrivate::shutdown()
{
    if (shutDown)
        return;
    shutDown = true;

    releaseMouseSettings();
    releaseTouchpadSettings();
    releaseKeyboardSettings();

    if (q->QtWayland::treeland_input_manager_v1::isInitialized())
        q->QtWayland::treeland_input_manager_v1::destroy();
    seat = nullptr;
    display = nullptr;
}

void TreelandInputManagerPrivate::onCapabilityAvailable(uint32_t type, wl_seat *capSeat)
{
    // Accept the first seat that reports capabilities; ignore others.
    if (!seat)
        seat = capSeat;
    else if (capSeat != seat)
        return;

    qCDebug(lcTreelandInput) << "Capability available: type=" << type
                             << "mouse=" << bool(type & kMouseBit)
                             << "touchpad=" << bool(type & kTouchpadBit)
                             << "keyboard=" << bool(type & kKeyboardBit);

    if (type & kMouseBit) {
        if (updateValue(mouseAvail, true)) {
            qCDebug(lcTreelandInput) << "Mouse capability acquired, creating mouse settings";
            ensureMouseSettings();
            Q_EMIT q->mouseAvailableChanged(true);
        }
    }

    if (type & kTouchpadBit) {
        if (updateValue(touchpadAvail, true)) {
            qCDebug(lcTreelandInput) << "Touchpad capability acquired, creating touchpad settings";
            ensureTouchpadSettings();
            Q_EMIT q->touchpadAvailableChanged(true);
        }
    }

    if (type & kKeyboardBit) {
        if (updateValue(keyboardAvail, true)) {
            qCDebug(lcTreelandInput) << "Keyboard capability acquired, creating keyboard settings";
            ensureKeyboardSettings();
            Q_EMIT q->keyboardAvailableChanged(true);
        }
    }
}

void TreelandInputManagerPrivate::onCapabilityUnavailable(uint32_t type, wl_seat *capSeat)
{
    if (capSeat != seat)
        return;

    qCDebug(lcTreelandInput) << "Capability unavailable: type=" << type
                             << "mouse=" << bool(type & kMouseBit)
                             << "touchpad=" << bool(type & kTouchpadBit)
                             << "keyboard=" << bool(type & kKeyboardBit);

    if (type & kMouseBit) {
        if (updateValue(mouseAvail, false)) {
            qCDebug(lcTreelandInput) << "Mouse capability lost, releasing mouse settings";
            releaseMouseSettings();
            Q_EMIT q->mouseAvailableChanged(false);
        }
    }

    if (type & kTouchpadBit) {
        if (updateValue(touchpadAvail, false)) {
            qCDebug(lcTreelandInput) << "Touchpad capability lost, releasing touchpad settings";
            releaseTouchpadSettings();
            Q_EMIT q->touchpadAvailableChanged(false);
        }
    }

    if (type & kKeyboardBit) {
        if (updateValue(keyboardAvail, false)) {
            qCDebug(lcTreelandInput) << "Keyboard capability lost, releasing keyboard settings";
            releaseKeyboardSettings();
            Q_EMIT q->keyboardAvailableChanged(false);
        }
    }

    // If no capabilities remain on this seat, release it so a new one can be accepted.
    if (!mouseAvail && !touchpadAvail && !keyboardAvail) {
        qCDebug(lcTreelandInput) << "All capabilities lost, releasing seat";
        seat = nullptr;
    }
}

void TreelandInputManagerPrivate::ensureMouseSettings()
{
    if (!q->isActive() || !seat || mouseSettings)
        return;

    qCDebug(lcTreelandInput) << "Creating mouse-specific settings";
    auto *handle = q->get_mouse_settings(seat);
    if (!handle) {
        qCWarning(lcTreelandInput) << "Failed to get mouse settings handle from compositor";
        return;
    }

    mouseSettings = new TreelandMouseSettings(this, q);
    mouseSettings->QtWayland::treeland_mouse_settings_v1::init(handle);
    mouseSettings->initPointerConfiguration();

    qCDebug(lcTreelandInput) << "Mouse-specific settings created";
}

void TreelandInputManagerPrivate::releaseMouseSettings()
{
    if (!mouseSettings)
        return;
    mouseSettings->releaseHandle();
    delete mouseSettings;
    mouseSettings = nullptr;
}

void TreelandInputManagerPrivate::ensureTouchpadSettings()
{
    if (!q->isActive() || !seat || touchpadSettings)
        return;

    qCDebug(lcTreelandInput) << "Creating touchpad-specific settings";
    auto *handle = q->get_touchpad_settings(seat);
    if (!handle) {
        qCWarning(lcTreelandInput) << "Failed to get touchpad settings handle from compositor";
        return;
    }

    touchpadSettings = new TreelandTouchpadSettings(this, q);
    touchpadSettings->QtWayland::treeland_touchpad_settings_v1::init(handle);
    touchpadSettings->initPointerConfiguration();

    qCDebug(lcTreelandInput) << "Touchpad-specific settings created";
}

void TreelandInputManagerPrivate::releaseTouchpadSettings()
{
    if (!touchpadSettings)
        return;
    touchpadSettings->releaseHandle();
    delete touchpadSettings;
    touchpadSettings = nullptr;
}

void TreelandInputManagerPrivate::ensureKeyboardSettings()
{
    if (!q->isActive() || !seat || keyboardSettings)
        return;

    qCDebug(lcTreelandInput) << "Creating keyboard settings";
    auto *handle = q->get_keyboard_settings(seat);
    if (!handle) {
        qCWarning(lcTreelandInput) << "Failed to get keyboard settings handle from compositor";
        return;
    }

    keyboardSettings = new TreelandKeyboardSettings(this, q);
    // init() registers the listener table on the underlying Wayland object.
    keyboardSettings->init(handle);
    qCDebug(lcTreelandInput) << "Keyboard settings created";
}

void TreelandInputManagerPrivate::releaseKeyboardSettings()
{
    if (!keyboardSettings)
        return;
    keyboardSettings->releaseHandle();
    delete keyboardSettings;
    keyboardSettings = nullptr;
}

void TreelandInputManagerPrivate::refreshMouse()
{
    qCDebug(lcTreelandInput) << "Refreshing mouse/touchpad settings";

    releaseMouseSettings();
    releaseTouchpadSettings();

    if (mouseAvail) {
        ensureMouseSettings();
    }
    if (touchpadAvail) {
        ensureTouchpadSettings();
    }

    qCDebug(lcTreelandInput) << "Mouse refresh complete:"
                             << "mouse=" << (mouseSettings != nullptr)
                             << "touchpad=" << (touchpadSettings != nullptr);
}

void TreelandInputManagerPrivate::refreshKeyboard()
{
    qCDebug(lcTreelandInput) << "Refreshing keyboard settings";

    // Release keyboard settings only
    releaseKeyboardSettings();

    // Re-create based on current capability state
    if (keyboardAvail) {
        ensureKeyboardSettings();
    }

    qCDebug(lcTreelandInput) << "Keyboard refresh complete:"
                             << "keyboard=" << (keyboardSettings != nullptr);
}

// ===========================================================================
// TreelandMouseSettings
// ===========================================================================

TreelandMouseSettings::TreelandMouseSettings(TreelandInputManagerPrivate *priv, QObject *parent)
    : QObject(parent)
    , m_priv(priv)
{
}

TreelandMouseSettings::~TreelandMouseSettings()
{
    releaseHandle();
}

uint32_t TreelandMouseSettings::accelerationProfile() const { return m_accelerationProfile; }
uint32_t TreelandMouseSettings::sendEventsMode() const { return m_sendEventsMode; }
double TreelandMouseSettings::scrollFactor() const { return m_scrollFactor; }
bool TreelandMouseSettings::leftHanded() const { return m_leftHanded; }
double TreelandMouseSettings::speed() const { return m_speed; }
bool TreelandMouseSettings::naturalScroll() const { return m_naturalScroll; }

void TreelandMouseSettings::initPointerConfiguration(uint32_t serial)
{
    if (!QtWayland::treeland_mouse_settings_v1::isInitialized()
        || QtWayland::treeland_pointer_device_configuration_v1::isInitialized()) {
        return;
    }

    qCDebug(lcTreelandInput) << "MouseSettings: requesting pointer_device_configuration, serial=" << serial;
    auto *rawCfg = get_pointer_configuration(serial);
    if (!rawCfg) {
        qCWarning(lcTreelandInput) << "MouseSettings: failed to get pointer_device_configuration from compositor";
        return;
    }

    QtWayland::treeland_pointer_device_configuration_v1::init(rawCfg);
    qCDebug(lcTreelandInput) << "MouseSettings: pointer_device_configuration initialized";
}

void TreelandMouseSettings::setAccelerationProfile(uint32_t profile)
{
    if (!QtWayland::treeland_pointer_device_configuration_v1::isInitialized())
        return;
    if (!(m_features & TREELAND_POINTER_DEVICE_CONFIGURATION_V1_FEATURE_ACCELERATION_PROFILE)) {
        qCDebug(lcTreelandInput) << "MouseSettings::setAccelerationProfile ignored: feature not supported";
        return;
    }
    qCDebug(lcTreelandInput) << "MouseSettings::setAccelerationProfile:" << profile;
    set_acceleration_profile(profile);
    apply();
    m_priv->flush();
}

void TreelandMouseSettings::setSendEventsMode(uint32_t mode)
{
    if (!QtWayland::treeland_pointer_device_configuration_v1::isInitialized())
        return;
    if (!(m_features & TREELAND_POINTER_DEVICE_CONFIGURATION_V1_FEATURE_SEND_EVENTS_MODE)) {
        qCDebug(lcTreelandInput) << "MouseSettings::setSendEventsMode ignored: feature not supported";
        return;
    }
    qCDebug(lcTreelandInput) << "MouseSettings::setSendEventsMode:" << mode;
    set_send_events_mode(mode);
    apply();
    m_priv->flush();
}

void TreelandMouseSettings::setScrollFactor(double factor)
{
    if (!QtWayland::treeland_pointer_device_configuration_v1::isInitialized()) return;
    if (!(m_features & TREELAND_POINTER_DEVICE_CONFIGURATION_V1_FEATURE_SCROLL_FACTOR)) {
        qCDebug(lcTreelandInput) << "MouseSettings::setScrollFactor ignored: feature not supported";
        return;
    }
    qCDebug(lcTreelandInput) << "MouseSettings::setScrollFactor:" << factor;
    set_scroll_factor(wl_fixed_from_double(factor));
    apply();
    m_priv->flush();
}

void TreelandMouseSettings::setLeftHanded(bool leftHanded)
{
    if (!QtWayland::treeland_pointer_device_configuration_v1::isInitialized()) return;
    if (!(m_features & TREELAND_POINTER_DEVICE_CONFIGURATION_V1_FEATURE_HANDED_MODE)) {
        qCDebug(lcTreelandInput) << "MouseSettings::setLeftHanded ignored: feature not supported";
        return;
    }
    qCDebug(lcTreelandInput) << "MouseSettings::setLeftHanded:" << leftHanded;
    set_handed_mode(leftHanded ? handed_mode_left : handed_mode_right);
    apply();
    m_priv->flush();
}

void TreelandMouseSettings::setSpeed(double speed)
{
    if (!QtWayland::treeland_pointer_device_configuration_v1::isInitialized()) return;
    if (!(m_features & TREELAND_POINTER_DEVICE_CONFIGURATION_V1_FEATURE_ACCEL_SPEED)) {
        qCDebug(lcTreelandInput) << "MouseSettings::setSpeed ignored: feature not supported";
        return;
    }
    qCDebug(lcTreelandInput) << "MouseSettings::setSpeed:" << speed;
    set_accel_speed(wl_fixed_from_double(speed));
    apply();
    m_priv->flush();
}

void TreelandMouseSettings::setNaturalScroll(bool enabled)
{
    if (!QtWayland::treeland_pointer_device_configuration_v1::isInitialized()) return;
    if (!(m_features & TREELAND_POINTER_DEVICE_CONFIGURATION_V1_FEATURE_NATURAL_SCROLL)) {
        qCDebug(lcTreelandInput) << "MouseSettings::setNaturalScroll ignored: feature not supported";
        return;
    }
    qCDebug(lcTreelandInput) << "MouseSettings::setNaturalScroll:" << enabled;
    set_natural_scroll(enabled ? TREELAND_INPUT_MANAGER_V1_STATE_ENABLED
                               : TREELAND_INPUT_MANAGER_V1_STATE_DISABLED);
    apply();
    m_priv->flush();
}

void TreelandMouseSettings::releaseHandle()
{
    if (QtWayland::treeland_pointer_device_configuration_v1::isInitialized())
        QtWayland::treeland_pointer_device_configuration_v1::destroy();
    if (QtWayland::treeland_mouse_settings_v1::isInitialized())
        QtWayland::treeland_mouse_settings_v1::destroy();
}

// ---------------------------------------------------------------------------
// TreelandMouseSettings — Qt Wayland virtual overrides
// ---------------------------------------------------------------------------

void TreelandMouseSettings::treeland_pointer_device_configuration_v1_feature(uint32_t feature)
{
    m_features = feature;
}

void TreelandMouseSettings::treeland_pointer_device_configuration_v1_scroll_factor(wl_fixed_t factor)
{
    double v = wl_fixed_to_double(factor);
    if (m_scrollFactor == v)
        return;
    m_scrollFactor = v;
    Q_EMIT scrollFactorChanged(m_scrollFactor);
}

void TreelandMouseSettings::treeland_pointer_device_configuration_v1_handed_mode(uint32_t mode)
{
    bool lh = (mode == handed_mode_left);
    if (m_leftHanded == lh)
        return;
    m_leftHanded = lh;
    Q_EMIT leftHandedChanged(m_leftHanded);
}

void TreelandMouseSettings::treeland_pointer_device_configuration_v1_accel_speed(wl_fixed_t accel_speed)
{
    double v = wl_fixed_to_double(accel_speed);
    if (m_speed == v)
        return;
    m_speed = v;
    Q_EMIT speedChanged(m_speed);
}

void TreelandMouseSettings::treeland_pointer_device_configuration_v1_acceleration_profile(uint32_t profile)
{
    if (m_accelerationProfile == profile)
        return;
    m_accelerationProfile = profile;
    Q_EMIT accelerationProfileChanged(m_accelerationProfile);
}

void TreelandMouseSettings::treeland_pointer_device_configuration_v1_send_events_mode(uint32_t mode)
{
    if (m_sendEventsMode == mode)
        return;
    m_sendEventsMode = mode;
    Q_EMIT sendEventsModeChanged(m_sendEventsMode);
}

void TreelandMouseSettings::treeland_pointer_device_configuration_v1_natural_scroll(uint32_t state)
{
    bool enabled = (state == TREELAND_INPUT_MANAGER_V1_STATE_ENABLED);
    if (m_naturalScroll == enabled)
        return;
    m_naturalScroll = enabled;
    Q_EMIT naturalScrollChanged(m_naturalScroll);
}

void TreelandMouseSettings::treeland_pointer_device_configuration_v1_disable_while_typing(uint32_t state)
{
    qCWarning(lcTreelandInput) << "MouseSettings: unexpected disable_while_typing event from compositor, state="
                               << state;
}

void TreelandMouseSettings::treeland_pointer_device_configuration_v1_tap_to_click(uint32_t state)
{
    qCWarning(lcTreelandInput) << "MouseSettings: unexpected tap_to_click event from compositor, state="
                               << state;
}

void TreelandMouseSettings::treeland_pointer_device_configuration_v1_failed()
{
    qCWarning(lcTreelandInput) << "MouseSettings: compositor failed to apply pending configuration";
    Q_EMIT failed();
}

void TreelandMouseSettings::treeland_pointer_device_configuration_v1_done(uint32_t serial)
{
    qCDebug(lcTreelandInput) << "MouseSettings: state batch done, serial=" << serial;
    m_lastSerial = serial;
    Q_EMIT done();
}

// ===========================================================================
// TreelandTouchpadSettings
// ===========================================================================

TreelandTouchpadSettings::TreelandTouchpadSettings(TreelandInputManagerPrivate *priv, QObject *parent)
    : QObject(parent)
    , m_priv(priv)
{
}

TreelandTouchpadSettings::~TreelandTouchpadSettings()
{
    releaseHandle();
}

bool TreelandTouchpadSettings::isEnabled() const { return m_enabled; }
uint32_t TreelandTouchpadSettings::sendEventsMode() const { return m_sendEventsMode; }
bool TreelandTouchpadSettings::disableWhileTyping() const { return m_disableWhileTyping; }
bool TreelandTouchpadSettings::tapToClick() const { return m_tapToClick; }
uint32_t TreelandTouchpadSettings::features() const { return m_features; }
double TreelandTouchpadSettings::scrollFactor() const { return m_scrollFactor; }
bool TreelandTouchpadSettings::leftHanded() const { return m_leftHanded; }
double TreelandTouchpadSettings::speed() const { return m_speed; }
bool TreelandTouchpadSettings::naturalScroll() const { return m_naturalScroll; }

void TreelandTouchpadSettings::initPointerConfiguration(uint32_t serial)
{
    if (!QtWayland::treeland_touchpad_settings_v1::isInitialized()
        || QtWayland::treeland_pointer_device_configuration_v1::isInitialized()) {
        return;
    }

    qCDebug(lcTreelandInput) << "TouchpadSettings: requesting pointer_device_configuration, serial=" << serial;
    auto *rawCfg = get_pointer_configuration(serial);
    if (!rawCfg) {
        qCWarning(lcTreelandInput) << "TouchpadSettings: failed to get pointer_device_configuration from compositor";
        return;
    }

    QtWayland::treeland_pointer_device_configuration_v1::init(rawCfg);
    qCDebug(lcTreelandInput) << "TouchpadSettings: pointer_device_configuration initialized";
}

void TreelandTouchpadSettings::setEnabled(bool enabled)
{
    if (!QtWayland::treeland_pointer_device_configuration_v1::isInitialized())
        return;
    qCDebug(lcTreelandInput) << "TouchpadSettings::setEnabled:" << enabled;
    setSendEventsMode(enabled ? send_events_mode_enabled : send_events_mode_disabled);
}

void TreelandTouchpadSettings::setSendEventsMode(uint32_t mode)
{
    if (!QtWayland::treeland_pointer_device_configuration_v1::isInitialized())
        return;
    if (!(m_features & TREELAND_POINTER_DEVICE_CONFIGURATION_V1_FEATURE_SEND_EVENTS_MODE)) {
        qCDebug(lcTreelandInput) << "TouchpadSettings::setSendEventsMode ignored: feature not supported";
        return;
    }
    qCDebug(lcTreelandInput) << "TouchpadSettings::setSendEventsMode:" << mode;
    set_send_events_mode(mode);
    apply();
    m_priv->flush();
}

void TreelandTouchpadSettings::setDisableWhileTyping(bool enabled)
{
    if (!QtWayland::treeland_pointer_device_configuration_v1::isInitialized())
        return;
    qCDebug(lcTreelandInput) << "TouchpadSettings::setDisableWhileTyping:" << enabled;
    set_disable_while_typing(enabled ? TREELAND_INPUT_MANAGER_V1_STATE_ENABLED
                                     : TREELAND_INPUT_MANAGER_V1_STATE_DISABLED);
    apply();
    m_priv->flush();
}

void TreelandTouchpadSettings::setTapToClick(bool enabled)
{
    if (!QtWayland::treeland_pointer_device_configuration_v1::isInitialized())
        return;
    qCDebug(lcTreelandInput) << "TouchpadSettings::setTapToClick:" << enabled;
    set_tap_to_click(enabled ? TREELAND_INPUT_MANAGER_V1_STATE_ENABLED
                             : TREELAND_INPUT_MANAGER_V1_STATE_DISABLED);
    apply();
    m_priv->flush();
}

void TreelandTouchpadSettings::setScrollFactor(double factor)
{
    if (!QtWayland::treeland_pointer_device_configuration_v1::isInitialized()) return;
    if (!(m_features & TREELAND_POINTER_DEVICE_CONFIGURATION_V1_FEATURE_SCROLL_FACTOR)) {
        qCDebug(lcTreelandInput) << "TouchpadSettings::setScrollFactor ignored: feature not supported";
        return;
    }
    qCDebug(lcTreelandInput) << "TouchpadSettings::setScrollFactor:" << factor;
    set_scroll_factor(wl_fixed_from_double(factor));
    apply();
    m_priv->flush();
}

void TreelandTouchpadSettings::setLeftHanded(bool leftHanded)
{
    if (!QtWayland::treeland_pointer_device_configuration_v1::isInitialized()) return;
    if (!(m_features & TREELAND_POINTER_DEVICE_CONFIGURATION_V1_FEATURE_HANDED_MODE)) {
        qCDebug(lcTreelandInput) << "TouchpadSettings::setLeftHanded ignored: feature not supported";
        return;
    }
    qCDebug(lcTreelandInput) << "TouchpadSettings::setLeftHanded:" << leftHanded;
    set_handed_mode(leftHanded ? handed_mode_left : handed_mode_right);
    apply();
    m_priv->flush();
}

void TreelandTouchpadSettings::setSpeed(double speed)
{
    if (!QtWayland::treeland_pointer_device_configuration_v1::isInitialized()) return;
    if (!(m_features & TREELAND_POINTER_DEVICE_CONFIGURATION_V1_FEATURE_ACCEL_SPEED)) {
        qCDebug(lcTreelandInput) << "TouchpadSettings::setSpeed ignored: feature not supported";
        return;
    }
    qCDebug(lcTreelandInput) << "TouchpadSettings::setSpeed:" << speed;
    set_accel_speed(wl_fixed_from_double(speed));
    apply();
    m_priv->flush();
}

void TreelandTouchpadSettings::setNaturalScroll(bool enabled)
{
    if (!QtWayland::treeland_pointer_device_configuration_v1::isInitialized()) return;
    if (!(m_features & TREELAND_POINTER_DEVICE_CONFIGURATION_V1_FEATURE_NATURAL_SCROLL)) {
        qCDebug(lcTreelandInput) << "TouchpadSettings::setNaturalScroll ignored: feature not supported";
        return;
    }
    qCDebug(lcTreelandInput) << "TouchpadSettings::setNaturalScroll:" << enabled;
    set_natural_scroll(enabled ? TREELAND_INPUT_MANAGER_V1_STATE_ENABLED
                               : TREELAND_INPUT_MANAGER_V1_STATE_DISABLED);
    apply();
    m_priv->flush();
}

void TreelandTouchpadSettings::releaseHandle()
{
    if (QtWayland::treeland_pointer_device_configuration_v1::isInitialized())
        QtWayland::treeland_pointer_device_configuration_v1::destroy();
    if (QtWayland::treeland_touchpad_settings_v1::isInitialized())
        QtWayland::treeland_touchpad_settings_v1::destroy();
}

// ---------------------------------------------------------------------------
// TreelandTouchpadSettings — Qt Wayland virtual overrides
// ---------------------------------------------------------------------------

void TreelandTouchpadSettings::treeland_pointer_device_configuration_v1_feature(uint32_t feature)
{
    if (m_features == feature)
        return;
    m_features = feature;
    Q_EMIT featuresChanged(m_features);
}

void TreelandTouchpadSettings::treeland_pointer_device_configuration_v1_scroll_factor(wl_fixed_t factor)
{
    double v = wl_fixed_to_double(factor);
    if (m_scrollFactor == v)
        return;
    m_scrollFactor = v;
    Q_EMIT scrollFactorChanged(m_scrollFactor);
}

void TreelandTouchpadSettings::treeland_pointer_device_configuration_v1_handed_mode(uint32_t mode)
{
    bool lh = (mode == handed_mode_left);
    if (m_leftHanded == lh)
        return;
    m_leftHanded = lh;
    Q_EMIT leftHandedChanged(m_leftHanded);
}

void TreelandTouchpadSettings::treeland_pointer_device_configuration_v1_accel_speed(wl_fixed_t accel_speed)
{
    double v = wl_fixed_to_double(accel_speed);
    if (m_speed == v)
        return;
    m_speed = v;
    Q_EMIT speedChanged(m_speed);
}

void TreelandTouchpadSettings::treeland_pointer_device_configuration_v1_acceleration_profile(uint32_t /*profile*/)
{
}

void TreelandTouchpadSettings::treeland_pointer_device_configuration_v1_send_events_mode(uint32_t mode)
{
    const bool enabled = (mode != send_events_mode_disabled);
    const bool modeChanged = (m_sendEventsMode != mode);
    const bool enabledChanged = (m_enabled != enabled);

    m_sendEventsMode = mode;
    m_enabled = enabled;

    if (modeChanged)
        Q_EMIT sendEventsModeChanged(m_sendEventsMode);
    if (enabledChanged)
        Q_EMIT this->enabledChanged(m_enabled);
}

void TreelandTouchpadSettings::treeland_pointer_device_configuration_v1_natural_scroll(uint32_t state)
{
    bool enabled = (state == TREELAND_INPUT_MANAGER_V1_STATE_ENABLED);
    if (m_naturalScroll == enabled)
        return;
    m_naturalScroll = enabled;
    Q_EMIT naturalScrollChanged(m_naturalScroll);
}

void TreelandTouchpadSettings::treeland_pointer_device_configuration_v1_disable_while_typing(uint32_t state)
{
    bool enabled = (state == TREELAND_INPUT_MANAGER_V1_STATE_ENABLED);
    if (m_disableWhileTyping == enabled)
        return;
    m_disableWhileTyping = enabled;
    Q_EMIT disableWhileTypingChanged(m_disableWhileTyping);
}

void TreelandTouchpadSettings::treeland_pointer_device_configuration_v1_tap_to_click(uint32_t state)
{
    bool enabled = (state == TREELAND_INPUT_MANAGER_V1_STATE_ENABLED);
    if (m_tapToClick == enabled)
        return;
    m_tapToClick = enabled;
    Q_EMIT tapToClickChanged(m_tapToClick);
}

void TreelandTouchpadSettings::treeland_pointer_device_configuration_v1_failed()
{
    qCWarning(lcTreelandInput) << "TouchpadSettings: compositor failed to apply pending configuration";
    Q_EMIT failed();
}

void TreelandTouchpadSettings::treeland_pointer_device_configuration_v1_done(uint32_t serial)
{
    qCDebug(lcTreelandInput) << "TouchpadSettings: state batch done, serial=" << serial;
    m_lastSerial = serial;
    Q_EMIT done();
}

// ===========================================================================
// TreelandKeyboardSettings
// ===========================================================================

TreelandKeyboardSettings::TreelandKeyboardSettings(TreelandInputManagerPrivate *priv, QObject *parent)
    : QObject(parent)
    , m_priv(priv)
{
}

TreelandKeyboardSettings::~TreelandKeyboardSettings()
{
    releaseHandle();
}

int TreelandKeyboardSettings::repeatRate() const { return m_repeatRate; }
int TreelandKeyboardSettings::repeatDelay() const { return m_repeatDelay; }
bool TreelandKeyboardSettings::numLock() const { return m_numLock; }

void TreelandKeyboardSettings::setRepeatRate(int rate)
{
    if (!isInitialized())
        return;
    qCDebug(lcTreelandInput) << "KeyboardSettings::setRepeatRate:" << rate;
    set_repeat(rate, m_repeatDelay);
    apply();
    m_priv->flush();
}

void TreelandKeyboardSettings::setRepeatDelay(int delay)
{
    if (!isInitialized())
        return;
    qCDebug(lcTreelandInput) << "KeyboardSettings::setRepeatDelay:" << delay;
    set_repeat(m_repeatRate, delay);
    apply();
    m_priv->flush();
}

void TreelandKeyboardSettings::setNumLock(bool on)
{
    if (!isInitialized())
        return;
    if (!(m_features & TREELAND_KEYBOARD_SETTINGS_V1_FEATURE_NUM_LOCK)) {
        qCDebug(lcTreelandInput) << "KeyboardSettings::setNumLock ignored: feature not supported";
        return;
    }
    qCDebug(lcTreelandInput) << "KeyboardSettings::setNumLock:" << on;
    set_num_lock(on ? toggle_state_on : toggle_state_off);
    apply();
    m_priv->flush();
}

void TreelandKeyboardSettings::treeland_keyboard_settings_v1_feature(uint32_t feature)
{
    m_features = feature;
}

void TreelandKeyboardSettings::treeland_keyboard_settings_v1_repeat(int32_t rate, int32_t delay)
{
    if (m_repeatRate == rate && m_repeatDelay == delay)
        return;
    m_repeatRate = rate;
    m_repeatDelay = delay;
    Q_EMIT repeatChanged(m_repeatRate, m_repeatDelay);
}

void TreelandKeyboardSettings::treeland_keyboard_settings_v1_num_lock(uint32_t state)
{
    bool on = (state == toggle_state_on);
    if (m_numLock == on)
        return;
    m_numLock = on;
    Q_EMIT numLockChanged(m_numLock);
}

void TreelandKeyboardSettings::treeland_keyboard_settings_v1_failed()
{
    qCWarning(lcTreelandInput) << "KeyboardSettings: compositor failed to apply pending configuration";
    Q_EMIT failed();
}

void TreelandKeyboardSettings::treeland_keyboard_settings_v1_done()
{
    qCDebug(lcTreelandInput) << "KeyboardSettings: state batch done";
    Q_EMIT done();
}

void TreelandKeyboardSettings::releaseHandle()
{
    if (isInitialized())
        destroy();
}

// ===========================================================================
// TreelandInputManager
// ===========================================================================

TreelandInputManager::TreelandInputManager(QObject *parent)
    : QWaylandClientExtensionTemplate<TreelandInputManager>(1)
    , d(std::make_unique<TreelandInputManagerPrivate>(this))
{
    setParent(parent);
    d->initialize();

    connect(this, &QWaylandClientExtension::activeChanged, this, [this]() {
        const bool available = isActive();
        qCDebug(lcTreelandInput) << "treeland_input_manager_v1 active changed:" << available;
        Q_EMIT availableChanged(available);
        if (available)
            return;

        const bool hadMouse = d->mouseAvail;
        const bool hadTouchpad = d->touchpadAvail;
        const bool hadKeyboard = d->keyboardAvail;
        d->mouseAvail = false;
        d->touchpadAvail = false;
        d->keyboardAvail = false;
        d->seat = nullptr;
        d->releaseMouseSettings();
        d->releaseTouchpadSettings();
        d->releaseKeyboardSettings();
        if (hadMouse)
            Q_EMIT mouseAvailableChanged(false);
        if (hadTouchpad)
            Q_EMIT touchpadAvailableChanged(false);
        if (hadKeyboard)
            Q_EMIT keyboardAvailableChanged(false);
    });

    qCDebug(lcTreelandInput) << "Treeland input manager initializing";

    // Destroy Wayland proxy objects while the display connection is still alive.
    // Q_GLOBAL_STATIC destruction order is undefined — the wl_display may be
    // gone by the time our destructor runs, causing a crash in wl_proxy_destroy.
    connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, [this]() {
        d->shutdown();
    });
}

TreelandInputManager::~TreelandInputManager() = default;

bool TreelandInputManager::available() const
{
    return isActive();
}

bool TreelandInputManager::mouseAvailable() const
{
    return d->mouseAvail;
}

bool TreelandInputManager::touchpadAvailable() const
{
    return d->touchpadAvail;
}

bool TreelandInputManager::keyboardAvailable() const
{
    return d->keyboardAvail;
}

TreelandMouseSettings *TreelandInputManager::mouseSettings() const
{
    return d->mouseSettings;
}

TreelandTouchpadSettings *TreelandInputManager::touchpadSettings() const
{
    return d->touchpadSettings;
}

TreelandKeyboardSettings *TreelandInputManager::keyboardSettings() const
{
    return d->keyboardSettings;
}

void TreelandInputManager::refreshMouse()
{
    d->refreshMouse();
}

void TreelandInputManager::refreshKeyboard()
{
    d->refreshKeyboard();
}

void TreelandInputManager::treeland_input_manager_v1_capability_available(uint32_t type, wl_seat *seat)
{
    d->onCapabilityAvailable(type, seat);
}

void TreelandInputManager::treeland_input_manager_v1_capability_unavailable(uint32_t type, wl_seat *seat)
{
    d->onCapabilityUnavailable(type, seat);
}

} // namespace DCC_NAMESPACE
