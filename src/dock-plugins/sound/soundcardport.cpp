// SPDX-FileCopyrightText: 2019 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "soundcardport.h"

#include <QApplication>

SoundCardPort::SoundCardPort(QObject* parent)
    : QObject(parent)
    , m_cardId(0)
    , m_active(false)
    , m_enabled(true)
    , m_bluetooth(false)
    , m_direction(Out)
    , m_portType(Other)
{
}

void SoundCardPort::setPortName(const QString& name)
{
    if (name != m_portName) {
        m_portName = name;
        Q_EMIT idChanged(name);
    }
}

void SoundCardPort::setDescription(const QString& description)
{
    if (description != m_description) {
        m_description = description;
        Q_EMIT nameChanged(description);
    }
}

void SoundCardPort::setCardName(const QString& cardName)
{
    if (cardName != m_cardName) {
        m_cardName = cardName;
        Q_EMIT cardNameChanged(cardName);
    }
}

void SoundCardPort::setActive(bool active)
{
    if (active != m_active) {
        m_active = active;
        Q_EMIT activityChanged(active);
    }
}

void SoundCardPort::setDirection(const Direction& direction)
{
    if (direction != m_direction) {
        m_direction = direction;
        Q_EMIT directionChanged(direction);
    }
}

void SoundCardPort::setCardId(const uint& cardId)
{
    if (cardId != m_cardId) {
        m_cardId = cardId;
        Q_EMIT cardIdChanged(cardId);
    }
}

void SoundCardPort::setEnabled(bool enabled)
{
    if (enabled != m_enabled) {
        m_enabled = enabled;
        Q_EMIT enabledChanged(m_enabled);
    }
}

void SoundCardPort::setBluetooth(bool bluetooth)
{
    m_bluetooth = bluetooth;
}

void SoundCardPort::setPortType(int type)
{
    m_portType = (PortType)type;
}
