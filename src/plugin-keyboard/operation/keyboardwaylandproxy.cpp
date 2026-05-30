// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "keyboardwaylandproxy.h"

#include "treelandinputmanager.h"

#include <QLoggingCategory>

using namespace DCC_NAMESPACE;

Q_LOGGING_CATEGORY(lcKeyboardWaylandProxy, "dde.dcc.keyboard.wayland")

KeyboardWaylandProxy::KeyboardWaylandProxy(QObject *parent)
    : QObject(parent)
{
}

void KeyboardWaylandProxy::active()
{
    qCDebug(lcKeyboardWaylandProxy) << "KeyboardWaylandProxy::active()";
    m_inputManager = new TreelandInputManager(this);

    connect(m_inputManager, &TreelandInputManager::keyboardAvailableChanged, this,
            [this](bool avail) {
                qCDebug(lcKeyboardWaylandProxy) << "keyboardAvailableChanged:" << avail;
                if (avail) {
                    connectKeyboardSettings(m_inputManager->keyboardSettings());
                } else {
                    disconnectKeyboardSettings();
                }
                Q_EMIT KeyboardAvailableChanged(avail);
            });

    if (m_inputManager->keyboardAvailable()) {
        qCDebug(lcKeyboardWaylandProxy) << "Keyboard already available, connecting immediately";
        connectKeyboardSettings(m_inputManager->keyboardSettings());
    } else {
        qCDebug(lcKeyboardWaylandProxy) << "Keyboard not yet available, waiting for signal";
    }
    Q_EMIT KeyboardAvailableChanged(m_inputManager->keyboardAvailable());
}

void KeyboardWaylandProxy::connectKeyboardSettings(TreelandKeyboardSettings *kbd)
{
    if (!kbd || m_keyboardSettings == kbd)
        return;
    m_keyboardSettings = kbd;
    qCDebug(lcKeyboardWaylandProxy) << "Connecting to TreelandKeyboardSettings";

    connect(kbd, &TreelandKeyboardSettings::repeatChanged, this,
            [this](int rate, int delay) {
                qCDebug(lcKeyboardWaylandProxy) << "repeatChanged: rate=" << rate << "Hz delay=" << delay << "ms";
                Q_EMIT RepeatDelayChanged(static_cast<uint>(delay));
                Q_EMIT RepeatIntervalChanged(rateToInterval(rate));
            });

    connect(kbd, &TreelandKeyboardSettings::numLockChanged, this,
            [this](bool on) {
                qCDebug(lcKeyboardWaylandProxy) << "numLockChanged:" << on;
                Q_EMIT NumLockStateChanged(on ? 1 : 0);
            });

    // 发射当前已缓存的初始状态，使 Worker 立即更新 Model
    qCDebug(lcKeyboardWaylandProxy) << "Emitting initial state: rate=" << kbd->repeatRate()
                                    << "delay=" << kbd->repeatDelay()
                                    << "numLock=" << kbd->numLock();
    Q_EMIT RepeatDelayChanged(static_cast<uint>(kbd->repeatDelay()));
    Q_EMIT RepeatIntervalChanged(rateToInterval(kbd->repeatRate()));
    Q_EMIT NumLockStateChanged(kbd->numLock() ? 1 : 0);
}

void KeyboardWaylandProxy::disconnectKeyboardSettings()
{
    if (!m_keyboardSettings)
        return;

    disconnect(m_keyboardSettings, nullptr, this, nullptr);
    m_keyboardSettings = nullptr;
}

bool KeyboardWaylandProxy::keyboardAvailable() const
{
    return m_inputManager && m_inputManager->keyboardAvailable();
}

// ---------------------------------------------------------------------------
// Getters
// ---------------------------------------------------------------------------

uint KeyboardWaylandProxy::repeatDelay() const
{
    if (m_keyboardSettings)
        return static_cast<uint>(m_keyboardSettings->repeatDelay());
    return 480; // 默认回退到合法档位
}

uint KeyboardWaylandProxy::repeatInterval() const
{
    if (m_keyboardSettings)
        return rateToInterval(m_keyboardSettings->repeatRate());
    return 50; // 默认回退到合法档位
}

int KeyboardWaylandProxy::numLockState() const
{
    if (m_keyboardSettings)
        return m_keyboardSettings->numLock() ? 1 : 0;
    return 0;
}

// ---------------------------------------------------------------------------
// Setters
// ---------------------------------------------------------------------------

void KeyboardWaylandProxy::setRepeatDelay(uint value)
{
    qCDebug(lcKeyboardWaylandProxy) << "setRepeatDelay:" << value << "ms";
    if (m_keyboardSettings)
        m_keyboardSettings->setRepeatDelay(static_cast<int>(value));
}

void KeyboardWaylandProxy::setRepeatInterval(uint value)
{
    qCDebug(lcKeyboardWaylandProxy) << "setRepeatInterval:" << value
                                    << "-> protocol rate=" << intervalToRate(value);
    if (m_keyboardSettings)
        m_keyboardSettings->setRepeatRate(intervalToRate(value));
}

void KeyboardWaylandProxy::setNumLockState(int value)
{
    qCDebug(lcKeyboardWaylandProxy) << "setNumLockState:" << value;
    if (m_keyboardSettings)
        m_keyboardSettings->setNumLock(value != 0);
}

void KeyboardWaylandProxy::refreshKeyboard()
{
    qCDebug(lcKeyboardWaylandProxy) << "refreshKeyboard: requesting keyboard settings refresh";

    disconnectKeyboardSettings();

    // 执行刷新
    m_inputManager->refreshKeyboard();

    // 重新连接
    if (m_inputManager->keyboardAvailable()) {
        connectKeyboardSettings(m_inputManager->keyboardSettings());
    }
    Q_EMIT KeyboardAvailableChanged(m_inputManager->keyboardAvailable());
}

// ---------------------------------------------------------------------------
// Conversion helpers
// ---------------------------------------------------------------------------
uint KeyboardWaylandProxy::rateToInterval(int rate)
{
    if (rate <= 0)
        return 50;
    return 1000 / static_cast<uint>(rate);
}

int KeyboardWaylandProxy::intervalToRate(uint intervalValue)
{
    if (intervalValue == 0)
        return 50;
    return static_cast<int>(1000 / intervalValue);
}
