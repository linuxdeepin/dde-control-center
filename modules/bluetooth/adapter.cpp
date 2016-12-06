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

Adapter::Adapter(QObject *parent) : QObject(parent)
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
    if (!m_devices.contains(device)) {
        m_devices.append(device);
        emit deviceAdded(device);
    }
}

void Adapter::removeDevice(const QString &deviceId) const
{
    emit deviceRemoved(deviceId);
}

QList<const Device *> Adapter::devices() const
{
    return m_devices;
}

void Adapter::setId(const QString &id)
{
    Q_ASSERT(m_id.isNull());

    m_id = id;
}

} // namespace bluetooth
} // namespace dcc
