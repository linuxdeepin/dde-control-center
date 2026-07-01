// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QWaylandClientExtension>
#include <memory>

// Qt Wayland C++ bindings generated from the protocol XML.
#include "qwayland-treeland-keyboard-state-notify-unstable-v1.h"

namespace DCC_NAMESPACE {

/**
 * @brief 封装 treeland_keyboard_state_notify_v1 Wayland 协议，监听键盘修饰键状态变化。
 *
 * 用法：
 * @code
 *   auto *notify = new KeyboardStateNotify(this);
 *   connect(notify, &KeyboardStateNotify::numLockChanged, this, [](bool on) {
 *       // 处理 NumLock 状态变化
 *   });
 * @endcode
 *
 * 自动创建 watcher 并订阅 NumLock 状态变化。
 */
class KeyboardStateNotify
    : public QWaylandClientExtensionTemplate<KeyboardStateNotify>
    , public QtWayland::treeland_keyboard_state_notify_manager_v1
{
    Q_OBJECT
    Q_PROPERTY(bool available READ available NOTIFY availableChanged)

public:
    explicit KeyboardStateNotify(QObject *parent = nullptr);
    ~KeyboardStateNotify() override;

    bool available() const;

Q_SIGNALS:
    void availableChanged(bool available);
    void numLockChanged(bool on);

private:
    class Watcher;
    Watcher *m_watcher = nullptr;
    bool m_available = false;

    void createWatcher(wl_seat *seat);
};

/**
 * @brief 封装 treeland_keyboard_state_watcher_v1，负责具体的修饰键监听。
 */
class KeyboardStateNotify::Watcher
    : public QObject
    , public QtWayland::treeland_keyboard_state_watcher_v1
{
    Q_OBJECT
public:
    explicit Watcher(KeyboardStateNotify *parent, struct ::treeland_keyboard_state_watcher_v1 *obj);
    ~Watcher() override;

Q_SIGNALS:
    void numLockChanged(bool on);

protected:
    void treeland_keyboard_state_watcher_v1_current_state(uint32_t modifier, uint32_t state) override;
    void treeland_keyboard_state_watcher_v1_state_changed(uint32_t modifier, uint32_t state) override;
};

} // namespace DCC_NAMESPACE
