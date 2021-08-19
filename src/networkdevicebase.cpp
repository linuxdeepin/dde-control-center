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

QString NetworkDeviceBase::ipv4() const
{
    // 返回IPv4地址
    if (m_activeInfoData.contains("Ip4")) {
        QJsonObject objIpv4 = m_activeInfoData["Ip4"].toObject();
        return objIpv4.value("Address").toString();
    }

    return QString();
}

QString NetworkDeviceBase::ipv6() const
{
    // 返回IPv6地址
    if (m_activeInfoData.contains("Ip6")) {
        QJsonObject objIpv4 = m_activeInfoData["Ip6"].toObject();
        return objIpv4.value("Address").toString();
    }

    return QString();
}

QJsonObject NetworkDeviceBase::activeConnectionInfo() const
{
    return m_activeInfoData;
}

void NetworkDeviceBase::setEnabled(bool enabled)
{
    m_networkInter->EnableDevice(QDBusObjectPath(path()), enabled);
}

Connectivity NetworkDeviceBase::connectivity()
{
    return m_connectivity;
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

NetworkDeviceBase::NetworkDeviceBase(NetworkInter *networkInter, QObject *parent)
    : QObject(parent)
    , m_networkInter(networkInter)
    , m_deviceStatus(DeviceStatus::Unknown)
    , m_enabled(true)
    , m_connectivity(Connectivity::Full)
{
}

NetworkDeviceBase::~NetworkDeviceBase()
{
}

NetworkInter *NetworkDeviceBase::networkInter()
{
    return m_networkInter;
}

void NetworkDeviceBase::updateDeviceInfo(const QJsonObject &info)
{
    m_data = info;
    DeviceStatus stat = convertDeviceStatus(info.value("State").toInt());

    if (stat != m_deviceStatus) {
        m_deviceStatus = stat;

        enqueueStatus(stat);
        // 状态发生变化后，需要向外抛出一个信号
        Q_EMIT deviceStatusChanged(stat);
    }
}

void NetworkDeviceBase::initDeviceInfo()
{
    if (m_networkInter) {
        // 状态发生变化后，获取设备的实时信息
        QDBusPendingReply<bool> netEnabled = m_networkInter->IsDeviceEnabled(QDBusObjectPath(path()));
        m_enabled = netEnabled.value();
    }
}

void NetworkDeviceBase::setDeviceEnabledStatus(const bool &enabled)
{
    m_enabled = enabled;
    Q_EMIT enableChanged(enabled);
}

void NetworkDeviceBase::updateActiveInfo(const QList<QJsonObject> &info)
{
    // 更新活动连接信息，查找当前的设备的最新的状态
    // 这里无需向外发送connectionChanged()信号，因为连接发生变化后，紧接着会获取IP地址等信息，
    // 获取IP地址信息是一个异步过程，所以它将会最后发送
    for (const QJsonValue jsonValue : info) {
        QJsonObject activeInfo = jsonValue.toObject();
        int activeStatus = activeInfo.value("State").toInt();
        if (activeStatus == static_cast<int>(ConnectionStatus::Activated)) {
            m_deviceStatus = DeviceStatus::Activated;
            break;
        }
    }
}

void NetworkDeviceBase::updateActiveConnectionInfo(const QList<QJsonObject> &infos)
{
    m_activeInfoData = QJsonObject();
    for (const QJsonValue jsonValue : infos) {
        const QJsonObject &info = jsonValue.toObject();
        if (info.value("ConnectionType").toString() == deviceKey()) {
            // 如果找到了当前硬件地址的连接信息，则直接使用这个数据
            m_activeInfoData = info;
            break;
        }
    }

    // 获取到完整的IP地址后，向外发送连接改变的信号
    if (!m_activeInfoData.isEmpty())
        Q_EMIT connectionChanged();
}

void NetworkDeviceBase::enqueueStatus(const DeviceStatus &status)
{
    if (m_statusQueue.size() >= MaxQueueSize)
        m_statusQueue.dequeue();

    m_statusQueue.enqueue(status);
}

QString NetworkDeviceBase::getStatusName()
{
    switch (m_deviceStatus) {
    case DeviceStatus::Unmanaged:
    case DeviceStatus::Unavailable:
    case DeviceStatus::Disconnected:  return tr("Disconnected");
    case DeviceStatus::Prepare:
    case DeviceStatus::Config:        return tr("Connecting");
    case DeviceStatus::Needauth:      return tr("Authenticating");
    case DeviceStatus::IpConfig:
    case DeviceStatus::IpCheck:       return tr("Obtaining Address");
    case DeviceStatus::Activated:     return tr("Connected");
    case DeviceStatus::Deactivation:  return tr("Disconnected");
    case DeviceStatus::Failed:        return tr("Failed");
    default:;
    }

    return QString();
}

QString NetworkDeviceBase::statusStringDetail()
{
    if (!m_enabled)
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
        switch (deviceType()) {
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
    default: break;
    }

    return tr("Failed");
}
