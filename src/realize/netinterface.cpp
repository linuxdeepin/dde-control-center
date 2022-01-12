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

#include <ipconfilctchecker.h>
#include <wireddevice.h>

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
    // 判读获取IP地址失败需要满足最后一个状态为未连接，上一个状态为失败，并且包含Config和IpConfig
    return !(m_statusQueue.size() == MaxQueueSize
            && m_statusQueue[MaxQueueSize - 1] == DeviceStatus::Disconnected
            && m_statusQueue[MaxQueueSize - 2] == DeviceStatus::Failed
            && m_statusQueue.contains(DeviceStatus::Config)
            && m_statusQueue.contains(DeviceStatus::IpConfig));
}

QString NetworkDeviceRealize::interface() const
{
    return QString();
}

QString NetworkDeviceRealize::driver() const
{
    return QString();
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

QStringList NetworkDeviceRealize::ipv4() const
{
    return QStringList();
}

QStringList NetworkDeviceRealize::ipv6() const
{
    return QStringList();
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
    if (m_ipConflicted)
        return DeviceStatus::IpConfilct;

    return m_deviceStatus;
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

NetworkDeviceRealize::NetworkDeviceRealize(IPConfilctChecker *ipConflictChecker, QObject *parent)
    : QObject (parent)
    , m_device(nullptr)
    , m_connectivity(Connectivity::Full)
    , m_deviceStatus(DeviceStatus::Unknown)
    , m_ipConflictChecker(ipConflictChecker)
    , m_ipConflicted(false)
{
    Q_ASSERT(m_ipConflictChecker);
    connect(m_ipConflictChecker, &IPConfilctChecker::conflictStatusChanged, this, &NetworkDeviceRealize::onConflictStatusChanged);
}

NetworkDeviceRealize::~NetworkDeviceRealize()
{
}

NetworkDeviceBase *NetworkDeviceRealize::device() const
{
    return m_device;
}

QString NetworkDeviceRealize::statusStringDetail()
{
    if (!isEnabled() || !m_device)
        return tr("Device disabled");

    if (m_deviceStatus == DeviceStatus::Activated && m_connectivity != Connectivity::Full)
        return tr("Connected but no Internet access");

    // 确认 没有获取IP显示未连接状态（DHCP服务关闭）
    if (!IPValid())
        return tr("Not connected");

    switch (m_deviceStatus) {
    case DeviceStatus::Unknown:
    case DeviceStatus::Unmanaged:
    case DeviceStatus::Unavailable: {
        switch (m_device->deviceType()) {
        case DeviceType::Unknown:      return QString();
        case DeviceType::Wired:       return tr("Network cable unplugged");
        default: break;
        }
        break;
    }
    case DeviceStatus::Disconnected:  return tr("Not connected");
    case DeviceStatus::Prepare:
    case DeviceStatus::Config:        return tr("Connecting");
    case DeviceStatus::Needauth:      return tr("Authenticating");
    case DeviceStatus::IpConfig:
    case DeviceStatus::IpCheck:
    case DeviceStatus::Secondaries:   return tr("Obtaining IP address");
    case DeviceStatus::Activated:     return tr("Connected");
    case DeviceStatus::Deactivation:  return tr("Disconnected");
    case DeviceStatus::IpConfilct:    return tr("IP conflict");
    default: break;
    }

    return tr("Failed");
}

QString NetworkDeviceRealize::getStatusName()
{
    if (hotspotEnabled())
        return tr("Disconnected");

    switch (m_deviceStatus) {
    case DeviceStatus::Unmanaged:
    case DeviceStatus::Unavailable:
    case DeviceStatus::Disconnected:  return tr("Disconnected");
    case DeviceStatus::Prepare:
    case DeviceStatus::Config:        return tr("Connecting");
    case DeviceStatus::Needauth:      return tr("Authenticating");
    case DeviceStatus::IpConfig:
    case DeviceStatus::IpCheck:       return tr("Obtaining address");
    case DeviceStatus::Activated:     return tr("Connected");
    case DeviceStatus::Deactivation:  return tr("Disconnected");
    case DeviceStatus::Failed:        return tr("Failed");
    case DeviceStatus::IpConfilct:    return tr("IP conflict");
    default:;
    }

    return QString();
}

void NetworkDeviceRealize::enqueueStatus(const DeviceStatus &status)
{
    if (m_statusQueue.size() >= MaxQueueSize)
        m_statusQueue.dequeue();

    m_statusQueue.enqueue(status);
}

void NetworkDeviceRealize::setDeviceStatus(const DeviceStatus &status)
{
    if (m_deviceStatus == status)
        return;

    m_deviceStatus = status;
    enqueueStatus(status);
    // 状态发生变化后，需要向外抛出一个信号
    Q_EMIT deviceStatusChanged(status);
}

void NetworkDeviceRealize::sortWiredItem(QList<WiredConnection *> &items)
{
    // 按照最后面的数字从小到大排序，如果名字中没有数字，就按照创建的先后顺序来排序(path数字的大小)
    qSort(items.begin(), items.end(), [ ] (WiredConnection *conn1, WiredConnection *conn2) {
        QString lastChar1 = conn1->connection()->id().right(1);
        QString lastChar2 = conn2->connection()->id().right(1);
        int rightCount = 3;
        int index1 = 0;
        int index2 = 0;
        bool ok1 = false, ok2 = false;
        do {
            if (!ok1) {
                lastChar1 = conn1->connection()->id().right(rightCount);
                index1 = lastChar1.toInt(&ok1);
            }
            if (!ok2) {
                lastChar2 = conn2->connection()->id().right(rightCount);
                index2 = lastChar2.toInt(&ok2);
            }
            if ((ok1 && ok2) || rightCount <= 0)
                break;

            rightCount--;
        } while (true);

        if (ok1 && ok2)
            return index1 < index2;
        QString path1 = conn1->connection()->path();
        QString path2 = conn2->connection()->path();
        path1 = path1.replace("\\", "/");
        path2 = path2.replace("\\", "/");
        QString configName1 = path1.mid(path1.lastIndexOf("/") + 1);
        QString configName2 = path2.mid(path2.lastIndexOf("/") + 1);
        return configName1.toInt() < configName2.toInt();
    });
}

void NetworkDeviceRealize::onConflictStatusChanged(NetworkDeviceBase *device, const bool confilcted)
{
    if (device != m_device)
        return;

    m_ipConflicted = confilcted;
}

}
}
