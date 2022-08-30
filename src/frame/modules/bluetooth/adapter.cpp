// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "adapter.h"

namespace dcc {
namespace bluetooth {

Adapter::Adapter(QObject *parent)
    : QObject(parent)
    , m_id("")
    , m_name("")
    , m_powered(false)
    , m_discovering(false)
    , m_discoverable(false)
{

}

void Adapter::setName(const QString &name)
{
    if (name != m_name) {
        m_name = name;
        Q_EMIT nameChanged(name);
    }
}

void Adapter::addDevice(const Device *device)
{
    if (!deviceById(device->id())) {
        m_devicesId << device->id();
        m_devices[device->id()] = device;
        //打印配对设备信息,方便查看设备显示顺序
        if (!device->name().isEmpty() && device->paired())
            qDebug() << "Adapter add device " << device->name();
        Q_EMIT deviceAdded(device);
    }
}

void Adapter::removeDevice(const QString &deviceId)
{
    const Device *device = nullptr;

    device = deviceById(deviceId);
    if (device) {
        m_devicesId.removeOne(deviceId);
        m_devices.remove(deviceId);
        Q_EMIT deviceRemoved(deviceId);
    }
}

void Adapter::setPowered(bool powered, bool discovering)
{
    if (!powered) {
        Q_EMIT closeDetailPage();
    }
    if (powered != m_powered || (powered && m_powered && discovering != m_discovering)) {
        m_powered = powered;
        m_discovering = discovering;
        Q_EMIT poweredChanged(powered, discovering);
    }
}

void Adapter::setDiscoverabled(const bool discoverable)
{
    if (m_discoverable == discoverable) {
        return;
    }
    m_discoverable = discoverable;
    Q_EMIT discoverableChanged(discoverable);
}

QMap<QString,const Device *> Adapter::devices() const
{
    return m_devices;
}

QList<QString> Adapter::devicesId() const
{
    return m_devicesId;
}

const Device *Adapter::deviceById(const QString &id) const
{
    return m_devices.keys().contains(id) ? m_devices[id] : nullptr;
}

void Adapter::setId(const QString &id)
{
   m_id = id;
}

} // namespace bluetooth
} // namespace dcc
