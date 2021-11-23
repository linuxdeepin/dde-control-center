/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     donghualin <donghualin@uniontech.com>
 *
 * Maintainer: donghualin <donghualin@uniontech.com>
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

#include "realize/netinterface.h"
#include "networkdevicebase.h"
#include "utils.h"

using namespace dde::network;

bool NetworkDeviceBase::IPValid()
{
    // 判读获取IP地址失败需要满足最后一个状态为未连接，上一个状态为失败，并且包含Config和IpConfig
    if (m_statusQueue.size() == MaxQueueSize
            && m_statusQueue[MaxQueueSize - 1] == DeviceStatus::Disconnected
            && m_statusQueue[MaxQueueSize - 2] == DeviceStatus::Failed
            && m_statusQueue.contains(DeviceStatus::Config)
            && m_statusQueue.contains(DeviceStatus::IpConfig)) {
        return false;
    }

    return true;
}

DeviceStatus NetworkDeviceBase::deviceStatus() const
{
    return deviceRealize()->deviceStatus();
}

QString NetworkDeviceBase::interface() const
{
    return deviceRealize()->interface();
}

QString NetworkDeviceBase::driver() const
{
    return deviceRealize()->driver();
}

bool NetworkDeviceBase::managed() const
{
    return deviceRealize()->managed();
}

QString NetworkDeviceBase::vendor() const
{
    return deviceRealize()->vendor();
}

QString NetworkDeviceBase::uniqueUuid() const
{
    return deviceRealize()->uniqueUuid();
}

bool NetworkDeviceBase::usbDevice() const
{
    return deviceRealize()->usbDevice();
}

QString NetworkDeviceBase::path() const
{
    return deviceRealize()->path();
}

QString NetworkDeviceBase::activeAp() const
{
    return deviceRealize()->activeAp();
}

bool NetworkDeviceBase::supportHotspot() const
{
    return deviceRealize()->supportHotspot();
}

QString NetworkDeviceBase::realHwAdr() const
{
    return deviceRealize()->realHwAdr();
}

QString NetworkDeviceBase::usingHwAdr() const
{
    return deviceRealize()->usingHwAdr();
}

QString NetworkDeviceBase::ipv4() const
{
    return deviceRealize()->ipv4();
}

QString NetworkDeviceBase::ipv6() const
{
    return deviceRealize()->ipv6();
}

QJsonObject NetworkDeviceBase::activeConnectionInfo() const
{
    return deviceRealize()->activeConnectionInfo();
}

void NetworkDeviceBase::setEnabled(bool enabled)
{
    deviceRealize()->setEnabled(enabled);
}

void NetworkDeviceBase::disconnectNetwork()
{
    deviceRealize()->disconnectNetwork();
}

Connectivity NetworkDeviceBase::connectivity()
{
    return deviceRealize()->connectivity();
}

void NetworkDeviceBase::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        Q_EMIT nameChanged(name);
    }
}

QString NetworkDeviceBase::deviceName()
{
    return m_name;
}

NetworkDeviceBase::NetworkDeviceBase(NetworkDeviceRealize *deviceInter, QObject *parent)
    : QObject(parent)
    , m_deviceInterface(deviceInter)
    , m_enabled(true)
{
    Q_ASSERT(m_deviceInterface);
    m_deviceInterface->setDevice(this);

    connect(m_deviceInterface, &NetworkDeviceRealize::deviceStatusChanged, this, &NetworkDeviceBase::deviceStatusChanged);
    connect(m_deviceInterface, &NetworkDeviceRealize::enableChanged, this, &NetworkDeviceBase::enableChanged);
    connect(m_deviceInterface, &NetworkDeviceRealize::connectionChanged, this, &NetworkDeviceBase::connectionChanged);
    connect(m_deviceInterface, &NetworkDeviceRealize::nameChanged, this, &NetworkDeviceBase::nameChanged);
    connect(m_deviceInterface, &NetworkDeviceRealize::removed, this, &NetworkDeviceBase::removed);
    connect(m_deviceInterface, &NetworkDeviceRealize::activeConnectionChanged, this, &NetworkDeviceBase::activeConnectionChanged);
    connect(m_deviceInterface, &NetworkDeviceRealize::ipV4Changed, this, &NetworkDeviceBase::ipV4Changed);
}

NetworkDeviceBase::~NetworkDeviceBase()
{
    delete m_deviceInterface;
}

NetworkDeviceRealize *NetworkDeviceBase::deviceRealize() const
{
    return m_deviceInterface;
}

void NetworkDeviceBase::enqueueStatus(const DeviceStatus &status)
{
    if (m_statusQueue.size() >= MaxQueueSize)
        m_statusQueue.dequeue();

    m_statusQueue.enqueue(status);
}

QString NetworkDeviceBase::getStatusName()
{
    return deviceRealize()->getStatusName();
}

QString NetworkDeviceBase::statusStringDetail()
{
    return deviceRealize()->statusStringDetail();
}

bool NetworkDeviceBase::isEnabled() const
{
    return m_deviceInterface->isEnabled();
}
