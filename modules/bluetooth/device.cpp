/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "device.h"

namespace dcc {
namespace bluetooth {

Device::Device(QObject *parent) :
    QObject(parent),
    m_id(""),
    m_name(""),
    m_paired(false),
    m_trusted(false),
    m_connecting(false),
    m_state(StateUnavailable)
{

}

Device::~Device()
{
    qDebug() << "~Device() " << m_id;
}

void Device::setId(const QString &id)
{
    m_id = id;
}

void Device::setName(const QString &name)
{
    if (name != m_name) {
        m_name = name;
        emit nameChanged(name);
    }
}

void Device::setPaired(bool paired)
{
    if (paired != m_paired) {
        m_paired = paired;
        emit pairedChanged(paired);
    }
}

void Device::setState(const State &state)
{
    if (state != m_state) {
        m_state = state;
        emit stateChanged(state);
    }
}

void Device::setTrusted(bool trusted)
{
    if (trusted != m_trusted) {
        m_trusted = trusted;
        emit trustedChanged(trusted);
    }
}

void Device::setConnecting(bool connecting)
{
    if (connecting != m_connecting) {
        m_connecting = connecting;
        emit connectingChanged(connecting);
    }
}

QDebug &operator<<(QDebug &stream, const Device *device)
{
    stream << "Device name:" << device->name() << " paired:" << device->paired() << " state:" << device->state();

    return stream;
}

} // namespace bluetooth
} // namespace dcc
