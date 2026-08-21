// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "ikeyboarddeviceproxy.h"
#include "treelandinputmanager.h"

#include <QObject>
#include <QPointer>

namespace DCC_NAMESPACE {

class TreelandInputManager;
class TreelandKeyboardSettings;
class KeyboardStateNotify;

/**
 * @brief 封装 treeland 输入管理协议的键盘代理类，对外提供与 KeyboardDBusProxy 相同的信号/槽接口，
 *        使 KeyboardWorker 无需感知底层协议细节。
 *
 * 在 treeland 会话下替代 KeyboardDBusProxy 承担键盘重复参数和 NumLock
 * 状态的上报与写入操作。
 * 所有数值均与 DBus 接口保持相同单位，转换逻辑在本类内部完成。
 *
 * 同时实现 IKeyboardDeviceProxy 接口，以便 KeyboardWorker 通过多态调用。
 */
class KeyboardWaylandProxy : public QObject, public IKeyboardDeviceProxy
{
    Q_OBJECT
public:
    explicit KeyboardWaylandProxy(QObject *parent = nullptr);

    /** 初始化并连接 TreelandInputManager 信号，发射当前初始状态 */
    void active() override;
    void deactive() override;
    bool keyboardAvailable() const;

    // ---- 同步读取当前缓存值（与 KeyboardDBusProxy 接口保持兼容）----
    /** 重复延迟，单位毫秒 */
    uint repeatDelay()    const override;
    /** 重复速率兼容值，沿用 DBus/旧 daemon 的离散值（100/80/65/50/35/25/20） */
    uint repeatInterval() const override;
    /** NumLock 状态（0=off, 1=on） */
    int  numLockState()   const override;

Q_SIGNALS:
    // ---- 状态变化信号（与 KeyboardDBusProxy 保持相同接口）----
    /** 重复延迟变化，单位毫秒 */
    void RepeatDelayChanged(uint value);
    /** 重复速率兼容值变化，沿用 DBus/旧 daemon 的离散值 */
    void RepeatIntervalChanged(uint value);
    /** NumLock 状态变化（0=off, 1=on） */
    void NumLockStateChanged(int value);
    /** 键盘能力可用性变化 */
    void KeyboardAvailableChanged(bool value);

public Q_SLOTS:
    // ---- 写入槽（与 KeyboardDBusProxy 保持相同接口，同时实现 IKeyboardDeviceProxy）----
    /** @param value 重复延迟，单位毫秒 */
    void setRepeatDelay(uint value)    override;
    /** @param value 重复速率兼容值，Wayland 下直接按旧 daemon 兼容值传给协议 */
    void setRepeatInterval(uint value) override;
    /** @param value NumLock 状态（0=off, 1=on） */
    void setNumLockState(int value)    override;

    /** 刷新键盘 settings 对象（重新从 compositor 获取状态） */
    void refreshKeyboard();

private:
    void connectKeyboardSettings(TreelandKeyboardSettings *kbd);
    void disconnectKeyboardSettings();

    // treeland rate <-> DBus RepeatInterval 兼容值
    // Interval(ms) = 1000 / Rate(Hz)
    static uint  rateToInterval(int rate);
    static int   intervalToRate(uint intervalValue);

    TreelandInputManager *m_inputManager = nullptr;
    QPointer<TreelandKeyboardSettings> m_keyboardSettings;
    KeyboardStateNotify *m_keyboardStateNotify = nullptr;
};

} // namespace DCC_NAMESPACE
