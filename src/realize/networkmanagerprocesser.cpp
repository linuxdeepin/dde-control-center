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
#include "ipconfilctchecker.h"

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
    , m_ipChecker(new IPConfilctChecker(this, false))
{
    Device::List allDevices = NetworkManager::networkInterfaces();
    for (Device::Ptr device : allDevices)
        onDeviceAdded(device->uni());

    initConnections();
    onConnectivityChanged(NetworkManager::connectivity());
}

NetworkManagerProcesser::~NetworkManagerProcesser()
{
    delete m_ipChecker;
}

void NetworkManagerProcesser::initConnections()
{
    connect(NetworkManager::notifier(), &Notifier::deviceAdded, this, &NetworkManagerProcesser::onDeviceAdded);
    connect(NetworkManager::notifier(), &Notifier::deviceRemoved, this, &NetworkManagerProcesser::onDeviceRemove);
    connect(NetworkManager::notifier(), &Notifier::connectivityChanged, this, &NetworkManagerProcesser::onConnectivityChanged);
    connect(m_ipChecker, &IPConfilctChecker::conflictStatusChanged, this, [ ] (NetworkDeviceBase *device, const bool &confilct) {
        Q_EMIT device->deviceStatusChanged(confilct ? DeviceStatus::IpConfilct : device->deviceStatus());
    });
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

void NetworkManagerProcesser::sortDevice()
{
    auto getPathIndex = [](const QString path)->int {
        int index = path.lastIndexOf("/");
        QString tmpIndexValue = path.mid(index + 1);
        return tmpIndexValue.toInt();
    };
    // 有线网络始终在无线网络的前面，如果两者都是有线或者无线网络，则按照path的顺序来排序
    qSort(m_devices.begin(), m_devices.end(),  [ = ](NetworkDeviceBase *device1, NetworkDeviceBase *device2) {
        if (device1->deviceType() == DeviceType::Wired && device2->deviceType() == DeviceType::Wireless)
            return true;

        if (device1->deviceType() == DeviceType::Wireless && device2->deviceType() == DeviceType::Wired)
            return false;

        return getPathIndex(device1->path()) < getPathIndex(device2->path());
    });
}

void NetworkManagerProcesser::onDeviceAdded(const QString &uni)
{
    auto deviceExist = [ this ] (const QString &uni)->bool {
        for (NetworkDeviceBase *device : m_devices) {
            if (device->path() == uni)
                return true;
        }

        return false;
    };

    if (deviceExist(uni))
        return;

    Device::Ptr currentDevice(nullptr);
    Device::List allDevices = NetworkManager::networkInterfaces();
    for (Device::Ptr device : allDevices) {
        if (device->uni() != uni)
            continue;

        if (device->type() == Device::Wifi || device->type() == Device::Ethernet)
            currentDevice = device;

        break;
    }

    if (!currentDevice)
        return;

    auto createDevice = [ = ](const Device::Ptr &device)->NetworkDeviceBase * {
        if (device->type() == Device::Wifi) {
            // 无线网络
            NetworkManager::WirelessDevice::Ptr wDevice = device.staticCast<NetworkManager::WirelessDevice>();
            DeviceManagerRealize *deviceRealize = new DeviceManagerRealize(m_ipChecker, wDevice);
            return new WirelessDevice(deviceRealize, Q_NULLPTR);
        }

        if (device->type() == Device::Ethernet) {
            // 有线网络
            NetworkManager::WiredDevice::Ptr wDevice = device.staticCast<NetworkManager::WiredDevice>();
            DeviceManagerRealize *deviceRealize = new DeviceManagerRealize(m_ipChecker, wDevice);
            return new WiredDevice(deviceRealize, Q_NULLPTR);
        }

        return nullptr;
    };

    auto deviceCreateOrRemove = [ this, deviceExist, createDevice ](const Device::Ptr &device) {
        if (device->managed() && (device->interfaceFlags() & DEVICE_INTERFACE_FLAG_UP)) {
            // 如果由非manager变成manager的模式，则新增设备
            if (!deviceExist(device->uni())) {
                NetworkDeviceBase *newDevice = createDevice(device);
                if (newDevice) {
                    m_devices << newDevice;
                    sortDevice();
                    updateDeviceName();
                    Q_EMIT deviceAdded({ newDevice });
                }
            }
        } else {
            // 如果由manager变成非manager模式，则移除设备
            NetworkDeviceBase *rmDevice = nullptr;
            for (NetworkDeviceBase *dev : m_devices) {
                if (dev->path() == device->uni()) {
                    m_devices.removeOne(dev);
                    rmDevice = dev;
                    break;
                }
            }
            if (rmDevice) {
                Q_EMIT rmDevice->removed();
                sortDevice();
                updateDeviceName();
                Q_EMIT deviceRemoved({ rmDevice });
                delete rmDevice;
                rmDevice = nullptr;
            }
        }
    };

    NetworkDeviceBase *newDevice = Q_NULLPTR;
    if (currentDevice->managed() && (currentDevice->interfaceFlags() & DEVICE_INTERFACE_FLAG_UP))
        newDevice = createDevice(currentDevice);

    connect(currentDevice.get(), &Device::interfaceFlagsChanged, this, [ currentDevice, deviceCreateOrRemove ] {
        deviceCreateOrRemove(currentDevice);
    });
    connect(currentDevice.get(), &Device::managedChanged, this, [ currentDevice, deviceCreateOrRemove ] {
        deviceCreateOrRemove(currentDevice);
    });

    if (newDevice) {
        m_devices << newDevice;
        sortDevice();
        updateDeviceName();
        Q_EMIT deviceAdded({ newDevice });
    }
}

void NetworkManagerProcesser::onDeviceRemove(const QString &uni)
{
    NetworkDeviceBase *rmDevice = Q_NULLPTR;
    for (NetworkDeviceBase *device : m_devices) {
        if (device->path() == uni) {
            m_devices.removeOne(device);
            rmDevice = device;
            break;
        }
    }

    if (rmDevice) {
        Q_EMIT rmDevice->removed();
        sortDevice();
        updateDeviceName();
        Q_EMIT deviceRemoved({ rmDevice });
        delete rmDevice;
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
        for (NetworkDeviceBase *device : m_devices) {
            DeviceManagerRealize *deviceRealize = static_cast<DeviceManagerRealize *>(device->deviceRealize());
            deviceRealize->m_connectivity = m_connectivity;
        }
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
