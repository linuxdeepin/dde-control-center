// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "port.h"


void Port::setId(const QString &id)
{
    if (id != m_id) {
        m_id = id;
        Q_EMIT idChanged(id);
    }
}

void Port::setName(const QString &name)
{
    if (name != m_name) {
        m_name = name;
        Q_EMIT nameChanged(name);
    }
}

void Port::setCardName(const QString &cardName)
{
    if (cardName != m_cardName) {
        m_cardName = cardName;
        Q_EMIT cardNameChanged(cardName);
    }
}

void Port::setIsActive(bool isActive)
{
    if (isActive != m_isActive) {
        m_isActive = isActive;
        if (m_direction == Port::In)
            Q_EMIT isInputActiveChanged(isActive);
        else
            Q_EMIT isOutputActiveChanged(isActive);
    }
}

void Port::setDirection(const Direction &direction)
{
    if (direction != m_direction) {
        m_direction = direction;
        Q_EMIT directionChanged(direction);
    }
}

void Port::setCardId(const uint &cardId)
{
    if (cardId != m_cardId) {
        m_cardId = cardId;
        Q_EMIT cardIdChanged(cardId);
    }
}

void Port::setEnabled(const bool enabled)
{
    if (enabled != m_enabled) {
        m_enabled = enabled;
        Q_EMIT currentPortEnabled(enabled);
    }
}

void Port::setIsBluetoothPort(const bool isBlue)
{
    if (m_isBluetoothPort != isBlue) {
        m_isBluetoothPort = isBlue;
        Q_EMIT currentBluetoothPortChanged(isBlue);
    }
}