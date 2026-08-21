// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QObject>
#include <QPointer>

namespace DCC_NAMESPACE {

class TreelandMouseSettings;
class TreelandTouchpadSettings;
class TreelandInputManager;

/**
 * @brief 封装 treeland 输入管理协议的代理类，对外提供与 MouseDBusProxy 相同的信号/槽接口，
 *        使 MouseWorker 无需感知底层协议细节。
 *
 * 在 treeland 会话下替代 MouseDBusProxy 承担状态上报和写入操作。
 * 所有数值均与 DBus 接口保持相同单位，转换逻辑在本类内部完成。
 */
class MouseWaylandProxy : public QObject
{
    Q_OBJECT
public:
    explicit MouseWaylandProxy(QObject *parent = nullptr);

    /** 初始化并连接 TreelandInputManager 信号 */
    void active();

Q_SIGNALS:
    // ---- 状态变化信号（与 MouseDBusProxy 保持相同接口）----
    void mouseExistChanged(bool exist);
    void tpadExistChanged(bool exist);
    void tpadEnabledChanged(bool enabled);
    void disTouchPadChanged(bool state);
    void leftHandStateChanged(bool state);
    void mouseNaturalScrollStateChanged(bool state);
    void touchNaturalScrollStateChanged(bool state);
    void disTypingChanged(bool state);
    void tapClickChanged(bool state);
    /** libinput 加速度值（与 MouseDBusProxy::mouseMotionAccelerationChanged 相同单位） */
    void mouseMotionAccelerationChanged(double value);
    /** false=平坦, true=自适应（与 MouseDBusProxy::accelProfileChanged 相同语义） */
    void accelProfileChanged(bool state);
    /** libinput 加速度值（与 MouseDBusProxy::touchpadMotionAccelerationChanged 相同单位） */
    void touchpadMotionAccelerationChanged(double value);
    /** 滚动速度 1~10（与 MouseDBusProxy::scrollSpeedChanged / 旧 daemon WheelSpeed 相同单位） */
    void scrollSpeedChanged(uint speed);

public Q_SLOTS:
    // ---- 写入槽（与 MouseDBusProxy 保持相同接口）----
    void setLeftHandState(bool state);
    void setMouseNaturalScrollState(bool state);
    void setTouchNaturalScrollState(bool state);
    void setDisTyping(bool state);
    void setTapClick(bool state);
    /** @param value libinput 加速度值 */
    void setMouseMotionAcceleration(double value);
    void setAccelProfile(bool state);
    /** @param value libinput 加速度值 */
    void setTouchpadMotionAcceleration(double value);
    void setScrollSpeed(uint speed);
    void setTouchpadEnabled(bool state);
    /** 使用 treeland send_events_mode 的 disabled_on_external_mouse 模式 */
    void setDisableTouchPadWhenMouseExist(bool state);

    void refreshMouse();

private:
    void connectMouseSettings(TreelandMouseSettings *mouse);
    void connectTouchpadSettings(TreelandTouchpadSettings *tpad);
    void disconnectMouseSettings();
    void disconnectTouchpadSettings();
    bool shouldUseTouchpadSharedState() const;
    bool isTouchpadEnabledInModel(uint32_t mode) const;
    bool isDisableTouchpadWhenMouseExistEnabled(uint32_t mode) const;
    void emitSharedLeftHanded(bool value);
    void emitSharedScrollFactor(double factor);
    void emitMouseNaturalScroll(bool value);
    void emitTouchpadNaturalScroll(bool value);
    void emitMouseSpeed(double value);
    void emitTouchpadSpeed(double value);
    void emitTouchpadMode(uint32_t mode);
    uint32_t touchpadSendEventsMode(bool touchpadEnabled, bool disableWhenMouseExist) const;

    // libinput 值（DBus 单位）<-> treeland speed [-1, 1]
    static double toInputMotionAccelration(double libinput);
    static double fromInputMotionAccelration(double speed);

    // 滚动速度（uint, 1..10）<-> treeland scroll factor（double，保持旧 daemon 1:1 兼容）
    static uint   scrollFactorToSpeed(double factor);
    static double speedToScrollFactor(uint speed);

    // 加速度模式：bool（DBus）<-> uint32（treeland）
    static bool     profileToState(uint32_t profile);
    static uint32_t stateToProfile(bool state);

    TreelandInputManager *m_inputManager = nullptr;
    QPointer<TreelandMouseSettings> m_connectedMouseSettings;
    QPointer<TreelandTouchpadSettings> m_connectedTouchpadSettings;
    bool m_leftHandedInitialized = false;
    bool m_leftHanded = false;
    bool m_mouseNaturalScrollInitialized = false;
    bool m_mouseNaturalScroll = false;
    bool m_touchpadNaturalScrollInitialized = false;
    bool m_touchpadNaturalScroll = false;
    bool m_mouseSpeedInitialized = false;
    double m_mouseSpeed = 0.0;
    bool m_touchpadSpeedInitialized = false;
    double m_touchpadSpeed = 0.0;
    bool m_scrollFactorInitialized = false;
    double m_scrollFactor = 1.0;
    bool m_disTouchPadInitialized = false;
    bool m_disTouchPad = false;
};

} // namespace DCC_NAMESPACE

