/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

const Device *Adapter::removeDevice(const QString &deviceId)
{
    const Device *device = nullptr;

    device = deviceById(deviceId);
    if (device) {
        m_devices.removeAll(device);
        emit deviceRemoved(deviceId);
    }

    return device;
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

const Device *Adapter::deviceById(const QString &id) const
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
