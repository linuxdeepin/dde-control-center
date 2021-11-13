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

#include "devicemanagerrealize.h"
#include "dslcontroller.h"
#include "hotspotcontroller.h"
#include "networkdevicebase.h"
#include "networkmanagerprocesser.h"
#include "proxycontroller.h"
#include "vpncontroller.h"

#include <networkmanagerqt/wireddevice.h>
#include <networkmanagerqt/wirelessdevice.h>
#include <networkmanagerqt/manager.h>

#include <wireddevice.h>
#include <wirelessdevice.h>

using namespace dde::network;
using namespace NetworkManager;

const static QString networkService = "com.deepin.daemon.Network";
const static QString networkPath = "/com/deepin/daemon/Network";

NetworkManagerProcesser::NetworkManagerProcesser(QObject *parent)
    : NetworkProcesser(parent)
    , m_proxyController(Q_NULLPTR)
    , m_vpnController(Q_NULLPTR)
    , m_dslController(Q_NULLPTR)
    , m_hotspotController(Q_NULLPTR)
    , m_networkInter(Q_NULLPTR)
    , m_connectivity(dde::network::Connectivity::Unknownconnectivity)
{
    onDeviceChanged();
    initConnections();
    onConnectivityChanged(NetworkManager::connectivity());
}

NetworkManagerProcesser::~NetworkManagerProcesser()
{
}

void NetworkManagerProcesser::initConnections()
{
    connect(NetworkManager::notifier(), &Notifier::deviceAdded, this, &NetworkManagerProcesser::onDeviceChanged);
    connect(NetworkManager::notifier(), &Notifier::deviceRemoved, this, &NetworkManagerProcesser::onDeviceChanged);
    connect(NetworkManager::notifier(), &Notifier::connectivityChanged, this, &NetworkManagerProcesser::onConnectivityChanged);
    QDBusConnection::systemBus().connect("com.deepin.system.Network", "/com/deepin/system/Network", "com.deepin.system.Network", "DeviceEnabled", this, SLOT(onDeviceEnabledChanged(QDBusObjectPath, bool)));
}

QList<NetworkDeviceBase *> NetworkManagerProcesser::devices()
{
    return m_devices;
}

dde::network::Connectivity NetworkManagerProcesser::connectivity()
{
    return m_connectivity;
}

QList<NetworkDetails *> NetworkManagerProcesser::networkDetails()
{
    return m_details;
}

ProxyController *NetworkManagerProcesser::proxyController()
{
    if (!m_proxyController)
        m_proxyController = new ProxyController(networkInter(), this);

    return m_proxyController;
}

VPNController *NetworkManagerProcesser::vpnController()
{
    if (!m_vpnController)
        m_vpnController = new VPNController(networkInter(), this);

    return m_vpnController;
}

DSLController *NetworkManagerProcesser::dslController()
{
    if (!m_dslController)
        m_dslController = new DSLController(networkInter(), this);

    return m_dslController;
}

HotspotController *NetworkManagerProcesser::hotspotController()
{
    if (!m_hotspotController)
        m_hotspotController = new HotspotController(networkInter(), this);

    return m_hotspotController;
}

void NetworkManagerProcesser::onDeviceChanged()
{
    QStringList devicePaths;
    QList<NetworkDeviceBase *> oldDevices = m_devices;
    QList<NetworkDeviceBase *> newDevice, currentDevices;
    Device::List allDevices = NetworkManager::networkInterfaces();
    for (QSharedPointer<Device> device : allDevices) {
        QString devPath = device->uni();
        if (!device->managed())
            continue;

        NetworkDeviceBase *dev = findDevice(devPath);
        if (!dev) {
            if (device->type() == NetworkManager::Device::Ethernet) {
                // 有线设备
                NetworkManager::WiredDevice::Ptr wDevice = device.staticCast<NetworkManager::WiredDevice>();
                DeviceManagerRealize *deviceRealize = new DeviceManagerRealize(wDevice);
                dev = new WiredDevice(deviceRealize, Q_NULLPTR);
            } else if (device->type() == NetworkManager::Device::Wifi) {
                // 无线设备
                NetworkManager::WirelessDevice::Ptr wDevice = device.staticCast<NetworkManager::WirelessDevice>();
                DeviceManagerRealize *deviceRealize = new DeviceManagerRealize(wDevice);
                dev = new WirelessDevice(deviceRealize, Q_NULLPTR);
            }
            if (dev) {
                newDevice << dev;
                devicePaths << devPath;
            }
        }
        if (dev)
            currentDevices << dev;
    }
    // 对比原来的
    QList<NetworkDeviceBase *> rmDevices;
    for (NetworkDeviceBase *device : m_devices) {
        if (!currentDevices.contains(device)) {
            rmDevices << device;
            m_devices.removeOne(device);
        }
    }

    m_devices = currentDevices;
    qSort(m_devices.begin(), m_devices.end(), [ = ] (NetworkDeviceBase *device1, NetworkDeviceBase *device2) {
        if (device1->deviceType() != device2->deviceType())
            return device1->deviceType() == DeviceType::Wired;

        return devicePaths.indexOf(device1->path()) < devicePaths.indexOf(device2->path());
    });

    updateDeviceName();

    if (newDevice.size() > 0 || rmDevices.size() > 0) {
        if (newDevice.size() > 0)
            Q_EMIT deviceAdded(newDevice);

        if (rmDevices.size() > 0) {
            for (NetworkDeviceBase *device : rmDevices)
                Q_EMIT device->removed();

            Q_EMIT deviceRemoved(rmDevices);
        }

        for (NetworkDeviceBase *device : rmDevices)
            delete device;
    }
}

void NetworkManagerProcesser::onConnectivityChanged(NetworkManager::Connectivity conntity)
{
    dde::network::Connectivity ctity;
    switch (conntity) {
    case NetworkManager::Connectivity::Full: {
        ctity = dde::network::Connectivity::Full;
        break;
    }
    case NetworkManager::Connectivity::Portal: {
        ctity = dde::network::Connectivity::Portal;
        break;
    }
    case NetworkManager::Connectivity::Limited: {
        ctity = dde::network::Connectivity::Limited;
        break;
    }
    case NetworkManager::Connectivity::NoConnectivity: {
        ctity = dde::network::Connectivity::Noconnectivity;
        break;
    }
    case NetworkManager::Connectivity::UnknownConnectivity: {
        ctity = dde::network::Connectivity::Unknownconnectivity;
        break;
    }
    }
    if (m_connectivity != ctity) {
        m_connectivity = ctity;
        Q_EMIT connectivityChanged(m_connectivity);
    }
}

void NetworkManagerProcesser::onDeviceEnabledChanged(QDBusObjectPath path, bool enabled)
{
    NetworkDeviceBase *device = findDevice(path.path());
    if (device)
        Q_EMIT device->enableChanged(enabled);
}

NetworkDeviceBase *NetworkManagerProcesser::findDevice(const QString devicePath)
{
    for (NetworkDeviceBase *device : m_devices) {
        if (device->path() == devicePath)
            return device;
    }

    return Q_NULLPTR;
}

NetworkInter *NetworkManagerProcesser::networkInter()
{
    if (!m_networkInter)
        m_networkInter = new NetworkInter(networkService, networkPath, QDBusConnection::sessionBus(), this);

    return m_networkInter;
}
