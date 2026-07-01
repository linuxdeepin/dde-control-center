// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "keyboardstatenotify.h"

#include <QGuiApplication>
#include <QLoggingCategory>
#include <wayland-client.h>

namespace DCC_NAMESPACE {

Q_LOGGING_CATEGORY(lcKbdStateNotify, "dde.dcc.keyboard.statenotify")

// ---------------------------------------------------------------------------
// Watcher
// ---------------------------------------------------------------------------

KeyboardStateNotify::Watcher::Watcher(KeyboardStateNotify *parent,
                                      struct ::treeland_keyboard_state_watcher_v1 *obj)
    : QObject(parent)
    , QtWayland::treeland_keyboard_state_watcher_v1(obj)
{
    qCDebug(lcKbdStateNotify) << "Watcher created, configuring to watch num_lock with all flags";

    // Watch num_lock modifier
    set_modifiers(modifier_num_lock);

    // Receive both locked and unlocked events
    set_flags(watch_flag_locked | watch_flag_unlocked);

    apply();
}

KeyboardStateNotify::Watcher::~Watcher()
{
    if (isInitialized())
        destroy();
}

void KeyboardStateNotify::Watcher::treeland_keyboard_state_watcher_v1_current_state(
    uint32_t modifier, uint32_t state)
{
    qCDebug(lcKbdStateNotify) << "current_state: modifier=" << modifier << "state=" << state;

    if (modifier == modifier_num_lock) {
        bool on = (state == modifier_state_locked);
        Q_EMIT numLockChanged(on);
    }
}

void KeyboardStateNotify::Watcher::treeland_keyboard_state_watcher_v1_state_changed(
    uint32_t modifier, uint32_t state)
{
    qCDebug(lcKbdStateNotify) << "state_changed: modifier=" << modifier << "state=" << state;

    if (modifier == modifier_num_lock) {
        bool on = (state == modifier_state_locked);
        Q_EMIT numLockChanged(on);
    }
}

// ---------------------------------------------------------------------------
// KeyboardStateNotify
// ---------------------------------------------------------------------------

KeyboardStateNotify::KeyboardStateNotify(QObject *parent)
    : QWaylandClientExtensionTemplate<KeyboardStateNotify>(1)
    , QtWayland::treeland_keyboard_state_notify_manager_v1()
{
    setParent(parent);

    qCDebug(lcKbdStateNotify) << "KeyboardStateNotify initializing";

    connect(this, &QWaylandClientExtension::activeChanged, this, [this]() {
        const bool avail = isActive();
        qCDebug(lcKbdStateNotify) << "active changed:" << avail;

        if (m_available != avail) {
            m_available = avail;
            Q_EMIT availableChanged(avail);
        }

        if (avail) {
            auto *waylandApp = qGuiApp->nativeInterface<QNativeInterface::QWaylandApplication>();
            if (waylandApp) {
                createWatcher(nullptr);
            }
        } else {
            delete m_watcher;
            m_watcher = nullptr;
        }
    });


}

KeyboardStateNotify::~KeyboardStateNotify()
{
    delete m_watcher;
    m_watcher = nullptr;
}

bool KeyboardStateNotify::available() const
{
    return m_available;
}

void KeyboardStateNotify::createWatcher(wl_seat *seat)
{
    if (m_watcher) {
        delete m_watcher;
        m_watcher = nullptr;
    }

    if (!isActive() || !QtWayland::treeland_keyboard_state_notify_manager_v1::isInitialized())
        return;

    // Pass null seat to monitor all seats
    auto *watcherObj = get_keyboard_state_watcher(seat);
    m_watcher = new Watcher(this, watcherObj);

    connect(m_watcher, &Watcher::numLockChanged, this, &KeyboardStateNotify::numLockChanged);

    // Flush the Wayland display to ensure the request is sent
    auto *waylandApp = qGuiApp->nativeInterface<QNativeInterface::QWaylandApplication>();
    if (waylandApp && waylandApp->display())
        wl_display_flush(waylandApp->display());

    qCDebug(lcKbdStateNotify) << "Watcher created and configured";
}

} // namespace DCC_NAMESPACE
