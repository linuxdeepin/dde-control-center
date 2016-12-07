/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "adapter.h"

namespace dcc {
namespace bluetooth {

Adapter::Adapter(QObject *parent) :
    QObject(parent),
    m_id(""),
    m_name(""),
    m_powered("false")
{

}

void Adapter::setName(const QString &name)
{
    if (name != m_name) {
        m_name = name;
        emit nameChanged(name);
    }
}

void Adapter::addDevice(const Device *device)
{
    if (!deviceById(device->id())) {
        m_devices.append(device);
        emit deviceAdded(device);
    }
}

void Adapter::removeDevice(const QString &deviceId)
{
    const Device *device = deviceById(deviceId);
    if (device) m_devices.removeAll(device);

    emit deviceRemoved(deviceId);
}

void Adapter::setPowered(bool powered)
{
    if (powered != m_powered) {
        m_powered = powered;
        emit poweredChanged(powered);
    }
}

QList<const Device *> Adapter::devices() const
{
    return m_devices;
}

const Device *Adapter::deviceById(const QString &id)
{
    for (const Device *device: m_devices) {
        if (device->id() == id) {
            return device;
        }
    }

    return nullptr;
}

void Adapter::setId(const QString &id)
{
   m_id = id;
}

} // namespace bluetooth
} // namespace dcc
