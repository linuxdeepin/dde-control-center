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

#include "netinterface.h"
#include "networkdevicebase.h"

namespace dde {
namespace network {

NetworkProcesser::NetworkProcesser(QObject *parent)
    : QObject (parent)
{
}

NetworkProcesser::~NetworkProcesser()
{
}

QList<NetworkDetails *> NetworkProcesser::networkDetails()
{
    return QList<NetworkDetails *>();
}

ProxyController *NetworkProcesser::proxyController()
{
    return Q_NULLPTR;
}

VPNController *NetworkProcesser::vpnController()
{
    return Q_NULLPTR;
}

DSLController *NetworkProcesser::dslController()
{
    return Q_NULLPTR;
}

HotspotController *NetworkProcesser::hotspotController()
{
    return Q_NULLPTR;
}

void NetworkProcesser::updateDeviceName()
{
    QList<NetworkDeviceBase *> wiredDevice;
    QList<NetworkDeviceBase *> wirelessDevice;

    QList<NetworkDeviceBase *> allDevices = this->devices();
    for (NetworkDeviceBase *device : allDevices) {
        if (device->deviceType() == DeviceType::Wired)
            wiredDevice << device;
        else if (device->deviceType() == DeviceType::Wireless)
            wirelessDevice << device;
    }

    // 更改有线设备的名称
    if (wiredDevice.size() == 1) {
        wiredDevice[0]->setName(tr("Wired Network"));
    } else if (wiredDevice.size() > 1) {
        for (int i = 0; i < wiredDevice.size(); i++) {
            QString name = QString(tr("Wired Network %1")).arg(i + 1);
            wiredDevice[i]->setName(name);
        }
    }

    // 更改无线设备的名称
    if (wirelessDevice.size() == 1) {
        wirelessDevice[0]->setName(tr("Wireless Network"));
    } else if (wirelessDevice.size() > 1) {
        for (int i = 0; i < wirelessDevice.size(); i++) {
            QString name = QString(tr("Wireless Network %1")).arg(i + 1);
            wirelessDevice[i]->setName(name);
        }
    }
}

/**
 * @brief 设备接口基类,具体实现在子类中实现
 * @return
 */
void NetworkDeviceRealize::setDevice(NetworkDeviceBase *device)
{
    m_device = device;
}

bool NetworkDeviceRealize::isEnabled() const
{
    return false;
}

bool NetworkDeviceRealize::IPValid()
{
    return false;
}

QString NetworkDeviceRealize::interface() const
{
    return QString("");
}

QString NetworkDeviceRealize::driver() const
{
    return QString("");
}

bool NetworkDeviceRealize::managed() const
{
    return false;
}

QString NetworkDeviceRealize::vendor() const
{
    return QString();
}

QString NetworkDeviceRealize::uniqueUuid() const
{
    return QString();
}

bool NetworkDeviceRealize::usbDevice() const
{
    return false;
}

QString NetworkDeviceRealize::path() const
{
    return QString();
}

QString NetworkDeviceRealize::activeAp() const
{
    return QString();
}

bool NetworkDeviceRealize::supportHotspot() const
{
    return false;
}

QString NetworkDeviceRealize::realHwAdr() const
{
    return QString();
}

QString NetworkDeviceRealize::usingHwAdr() const
{
    return QString();
}

QString NetworkDeviceRealize::ipv4() const
{
    return QString();
}

QString NetworkDeviceRealize::ipv6() const
{
    return QString();
}

QJsonObject NetworkDeviceRealize::activeConnectionInfo() const
{
    return QJsonObject();
}

void NetworkDeviceRealize::setEnabled(bool enabled)
{
    Q_UNUSED(enabled);
}

void NetworkDeviceRealize::disconnectNetwork()
{
}

Connectivity NetworkDeviceRealize::connectivity()
{
    return m_connectivity;
}

DeviceStatus NetworkDeviceRealize::deviceStatus() const
{
    return DeviceStatus::Disconnected;
}

QList<AccessPoints *> NetworkDeviceRealize::accessPointItems() const
{
    return QList<AccessPoints *>();
}

void NetworkDeviceRealize::connectNetwork(const AccessPoints *item)
{
    Q_UNUSED(item);
}

QList<WirelessConnection *> NetworkDeviceRealize::wirelessItems() const
{
    return QList<WirelessConnection *>();
}

AccessPoints *NetworkDeviceRealize::activeAccessPoints() const
{
    return nullptr;
}

bool NetworkDeviceRealize::connectNetwork(WiredConnection *connection)
{
    Q_UNUSED(connection);
    return false;
}

QList<WiredConnection *> NetworkDeviceRealize::wiredItems() const
{
    return QList<WiredConnection *>();
}

NetworkDeviceRealize::NetworkDeviceRealize(QObject *parent)
    : QObject (parent)
    , m_device(nullptr)
    , m_connectivity(Connectivity::Full)
{
}

NetworkDeviceRealize::~NetworkDeviceRealize()
{
}

NetworkDeviceBase *NetworkDeviceRealize::device() const
{
    return m_device;
}

}
}
