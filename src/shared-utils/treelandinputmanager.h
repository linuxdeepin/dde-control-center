// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QWaylandClientExtension>
#include <memory>

// Qt Wayland C++ bindings generated from the protocol XML.
// Only included when Treeland support is enabled (treelandinputmanager.h is
// only added to the build in that configuration).
#include "qwayland-treeland-input-manager-unstable-v1.h"

namespace DCC_NAMESPACE {

class TreelandInputManagerPrivate;
class TreelandMouseSettings;
class TreelandTouchpadSettings;

// ---------------------------------------------------------------------------
// TreelandMouseSettings
// ---------------------------------------------------------------------------
/**
 * @brief 鼠标完整设置，对应 treeland_mouse_settings_v1 工厂 + treeland_pointer_device_configuration_v1。
 *
 * 持有唯一的 treeland_pointer_device_configuration_v1 对象（符合协议"每 factory 最多一个"要求）。
 * 同时处理共享指针事件（速度、自然滚动等）和鼠标专有事件（加速度模式等）。
 */
class TreelandMouseSettings
    : public QObject
    , public QtWayland::treeland_mouse_settings_v1
    , public QtWayland::treeland_pointer_device_configuration_v1
{
    Q_OBJECT
    Q_PROPERTY(uint32_t accelerationProfile READ accelerationProfile NOTIFY accelerationProfileChanged)
    Q_PROPERTY(uint32_t sendEventsMode READ sendEventsMode NOTIFY sendEventsModeChanged)
    Q_PROPERTY(double scrollFactor READ scrollFactor NOTIFY scrollFactorChanged)
    Q_PROPERTY(bool leftHanded READ leftHanded NOTIFY leftHandedChanged)
    Q_PROPERTY(double speed READ speed NOTIFY speedChanged)
    Q_PROPERTY(bool naturalScroll READ naturalScroll NOTIFY naturalScrollChanged)

public:
    /** Corresponds to treeland_pointer_device_configuration_v1::acceleration_profile */
    enum class AccelerationProfile : uint32_t {
        None     = 0,
        Flat     = 1,
        Adaptive = 2,
        Custom   = 4,
    };
    Q_ENUM(AccelerationProfile)
    explicit TreelandMouseSettings(TreelandInputManagerPrivate *priv, QObject *parent = nullptr);
    ~TreelandMouseSettings() override;

    uint32_t accelerationProfile() const;
    uint32_t sendEventsMode() const;
    double scrollFactor() const;
    bool leftHanded() const;
    double speed() const;
    bool naturalScroll() const;

public Q_SLOTS:
    void setAccelerationProfile(uint32_t profile);
    void setSendEventsMode(uint32_t mode);
    void setScrollFactor(double factor);
    void setLeftHanded(bool leftHanded);
    void setSpeed(double speed);
    void setNaturalScroll(bool enabled);

Q_SIGNALS:
    void accelerationProfileChanged(uint32_t profile);
    void sendEventsModeChanged(uint32_t mode);
    void scrollFactorChanged(double factor);
    void leftHandedChanged(bool leftHanded);
    void speedChanged(double speed);
    void naturalScrollChanged(bool enabled);
    void failed();
    void done();

protected:
    void treeland_pointer_device_configuration_v1_feature(uint32_t feature) override;
    void treeland_pointer_device_configuration_v1_scroll_factor(wl_fixed_t factor) override;
    void treeland_pointer_device_configuration_v1_handed_mode(uint32_t mode) override;
    void treeland_pointer_device_configuration_v1_accel_speed(wl_fixed_t accel_speed) override;
    void treeland_pointer_device_configuration_v1_acceleration_profile(uint32_t profile) override;
    void treeland_pointer_device_configuration_v1_send_events_mode(uint32_t mode) override;
    void treeland_pointer_device_configuration_v1_natural_scroll(uint32_t state) override;
    void treeland_pointer_device_configuration_v1_disable_while_typing(uint32_t state) override;
    void treeland_pointer_device_configuration_v1_tap_to_click(uint32_t state) override;
    void treeland_pointer_device_configuration_v1_failed() override;
    void treeland_pointer_device_configuration_v1_done(uint32_t serial) override;

private:
    friend class TreelandInputManagerPrivate;

    TreelandInputManagerPrivate *m_priv = nullptr;

    uint32_t m_accelerationProfile = 0;
    uint32_t m_sendEventsMode = 0;
    double m_scrollFactor = 1.0;
    bool m_leftHanded = false;
    double m_speed = 0.0;
    bool m_naturalScroll = false;
    uint32_t m_features = 0;    // accumulated feature bitmask
    uint32_t m_lastSerial = 0;

    void initPointerConfiguration(uint32_t serial = 0);
    void releaseHandle();
};

// ---------------------------------------------------------------------------
// TreelandTouchpadSettings
// ---------------------------------------------------------------------------
/**
 * @brief 触摸板完整设置，对应 treeland_touchpad_settings_v1 工厂 + treeland_pointer_device_configuration_v1。
 *
 * 持有唯一的 treeland_pointer_device_configuration_v1 对象。
 * 同时处理共享指针事件（速度、自然滚动等）和触摸板专有事件（打字禁用、轻触点击等）。
 */
class TreelandTouchpadSettings
    : public QObject
    , public QtWayland::treeland_touchpad_settings_v1
    , public QtWayland::treeland_pointer_device_configuration_v1
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ isEnabled NOTIFY enabledChanged)
    Q_PROPERTY(uint32_t sendEventsMode READ sendEventsMode NOTIFY sendEventsModeChanged)
    Q_PROPERTY(bool disableWhileTyping READ disableWhileTyping NOTIFY disableWhileTypingChanged)
    Q_PROPERTY(bool tapToClick READ tapToClick NOTIFY tapToClickChanged)
    Q_PROPERTY(uint32_t features READ features NOTIFY featuresChanged)
    Q_PROPERTY(double scrollFactor READ scrollFactor NOTIFY scrollFactorChanged)
    Q_PROPERTY(bool leftHanded READ leftHanded NOTIFY leftHandedChanged)
    Q_PROPERTY(double speed READ speed NOTIFY speedChanged)
    Q_PROPERTY(bool naturalScroll READ naturalScroll NOTIFY naturalScrollChanged)

public:
    explicit TreelandTouchpadSettings(TreelandInputManagerPrivate *priv, QObject *parent = nullptr);
    ~TreelandTouchpadSettings() override;

    bool isEnabled() const;
    uint32_t sendEventsMode() const;
    bool disableWhileTyping() const;
    bool tapToClick() const;
    uint32_t features() const;
    double scrollFactor() const;
    bool leftHanded() const;
    double speed() const;
    bool naturalScroll() const;

public Q_SLOTS:
    void setEnabled(bool enabled);
    void setSendEventsMode(uint32_t mode);
    void setDisableWhileTyping(bool enabled);
    void setTapToClick(bool enabled);
    void setScrollFactor(double factor);
    void setLeftHanded(bool leftHanded);
    void setSpeed(double speed);
    void setNaturalScroll(bool enabled);

Q_SIGNALS:
    void enabledChanged(bool enabled);
    void sendEventsModeChanged(uint32_t mode);
    void disableWhileTypingChanged(bool enabled);
    void tapToClickChanged(bool enabled);
    void featuresChanged(uint32_t features);
    void scrollFactorChanged(double factor);
    void leftHandedChanged(bool leftHanded);
    void speedChanged(double speed);
    void naturalScrollChanged(bool enabled);
    void failed();
    void done();

protected:
    void treeland_pointer_device_configuration_v1_feature(uint32_t feature) override;
    void treeland_pointer_device_configuration_v1_scroll_factor(wl_fixed_t factor) override;
    void treeland_pointer_device_configuration_v1_handed_mode(uint32_t mode) override;
    void treeland_pointer_device_configuration_v1_accel_speed(wl_fixed_t accel_speed) override;
    void treeland_pointer_device_configuration_v1_acceleration_profile(uint32_t profile) override;
    void treeland_pointer_device_configuration_v1_send_events_mode(uint32_t mode) override;
    void treeland_pointer_device_configuration_v1_natural_scroll(uint32_t state) override;
    void treeland_pointer_device_configuration_v1_disable_while_typing(uint32_t state) override;
    void treeland_pointer_device_configuration_v1_tap_to_click(uint32_t state) override;
    void treeland_pointer_device_configuration_v1_failed() override;
    void treeland_pointer_device_configuration_v1_done(uint32_t serial) override;

private:
    friend class TreelandInputManagerPrivate;

    TreelandInputManagerPrivate *m_priv = nullptr;

    bool m_enabled = true;
    uint32_t m_sendEventsMode = send_events_mode_enabled;
    bool m_disableWhileTyping = false;
    bool m_tapToClick = false;
    uint32_t m_features = 0;    // accumulated feature bitmask
    double m_scrollFactor = 1.0;
    bool m_leftHanded = false;
    double m_speed = 0.0;
    bool m_naturalScroll = false;
    uint32_t m_lastSerial = 0;

    void initPointerConfiguration(uint32_t serial = 0);
    void releaseHandle();
};

// ---------------------------------------------------------------------------
// TreelandKeyboardSettings
// ---------------------------------------------------------------------------
/**
 * @brief 键盘设置，对应 treeland_keyboard_settings_v1。
 *
 * 涵盖：重复速率/延迟、数字锁定、大写锁定 OSD。
 */
class TreelandKeyboardSettings
    : public QObject
    , public QtWayland::treeland_keyboard_settings_v1
{
    Q_OBJECT
    Q_PROPERTY(int repeatRate READ repeatRate NOTIFY repeatChanged)
    Q_PROPERTY(int repeatDelay READ repeatDelay NOTIFY repeatChanged)
    Q_PROPERTY(bool numLock READ numLock NOTIFY numLockChanged)

public:
    explicit TreelandKeyboardSettings(TreelandInputManagerPrivate *priv, QObject *parent = nullptr);
    ~TreelandKeyboardSettings() override;

    int repeatRate() const;
    int repeatDelay() const;
    bool numLock() const;

public Q_SLOTS:
    void setRepeatRate(int rate);
    void setRepeatDelay(int delay);
    void setNumLock(bool on);

Q_SIGNALS:
    void repeatChanged(int rate, int delay);
    void numLockChanged(bool on);
    void failed();
    void done();

protected:
    // Qt Wayland virtual overrides — called by the generated listener dispatcher.
    void treeland_keyboard_settings_v1_feature(uint32_t feature) override;
    void treeland_keyboard_settings_v1_repeat(int32_t rate, int32_t delay) override;
    void treeland_keyboard_settings_v1_num_lock(uint32_t state) override;
    void treeland_keyboard_settings_v1_failed() override;
    void treeland_keyboard_settings_v1_done() override;

private:
    friend class TreelandInputManagerPrivate;

    TreelandInputManagerPrivate *m_priv = nullptr;

    int m_repeatRate = 25;
    int m_repeatDelay = 500;
    bool m_numLock = false;
    uint32_t m_features = 0;

    void releaseHandle();
};

// ---------------------------------------------------------------------------
// TreelandInputManager
// ---------------------------------------------------------------------------
/**
 * @brief 封装 treeland_input_manager_v1 Wayland 协议的 Qt 管理器。
 *
 * 用法：
 * @code
 *   auto *mgr = new TreelandInputManager(this);
 *   connect(mgr, &TreelandInputManager::mouseAvailableChanged, this, [mgr](bool avail) {
 *       if (avail) {
 *           auto *mouse = mgr->mouseSettings();
 *           // 等待 done() 信号后读取初始值
 *       }
 *   });
 * @endcode
 *
 * 当 mouseAvailable() 为 true 时，mouseSettings() 非空。
 * 当 touchpadAvailable() 为 true 时，touchpadSettings() 非空。
 * 当 keyboardAvailable() 为 true 时，keyboardSettings() 非空。
 *
 * @note 必须在 Wayland 会话（treeland 合成器）下运行，否则 available() 始终为 false。
 */
class TreelandInputManager
    : public QWaylandClientExtensionTemplate<TreelandInputManager>
    , public QtWayland::treeland_input_manager_v1
{
    Q_OBJECT
    Q_PROPERTY(bool available READ available NOTIFY availableChanged)
    Q_PROPERTY(bool mouseAvailable READ mouseAvailable NOTIFY mouseAvailableChanged)
    Q_PROPERTY(bool touchpadAvailable READ touchpadAvailable NOTIFY touchpadAvailableChanged)
    Q_PROPERTY(bool keyboardAvailable READ keyboardAvailable NOTIFY keyboardAvailableChanged)

public:
    explicit TreelandInputManager(QObject *parent = nullptr);
    ~TreelandInputManager() override;

    /** 协议对象是否已成功绑定 */
    bool available() const;

    bool mouseAvailable() const;
    bool touchpadAvailable() const;
    bool keyboardAvailable() const;

    /**
     * 鼠标专有设置。仅在 mouseAvailable() 时非空。
     */
    TreelandMouseSettings *mouseSettings() const;

    /**
     * 触摸板专有设置。仅在 touchpadAvailable() 时非空。
     */
    TreelandTouchpadSettings *touchpadSettings() const;

    /**
     * 键盘设置。仅在 keyboardAvailable() 时非空。
     */
    TreelandKeyboardSettings *keyboardSettings() const;

    /**
     * 仅刷新鼠标/触摸板相关 settings。
     * 键盘 settings 不受影响。
     */
    void refreshMouse();

    /**
     * 仅刷新键盘 settings。鼠标/触摸板 settings 不受影响。
     */
    void refreshKeyboard();

protected:
    void treeland_input_manager_v1_capability_available(uint32_t type, wl_seat *seat) override;
    void treeland_input_manager_v1_capability_unavailable(uint32_t type, wl_seat *seat) override;

Q_SIGNALS:
    void availableChanged(bool available);
    void mouseAvailableChanged(bool available);
    void touchpadAvailableChanged(bool available);
    void keyboardAvailableChanged(bool available);

private:
    std::unique_ptr<TreelandInputManagerPrivate> d;
};

} // namespace DCC_NAMESPACE
