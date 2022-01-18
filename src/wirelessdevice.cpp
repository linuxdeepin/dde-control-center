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

#include "wirelessdevice.h"
#include "netutils.h"
#include "realize/netinterface.h"

using namespace dde::network;

bool WirelessDevice::isConnected() const
{
    QList<AccessPoints *> aps = deviceRealize()->accessPointItems();
    for (AccessPoints *ap : aps) {
        if (ap->status() == ConnectionStatus::Activated)
            return true;
    }

    return false;
}

DeviceType WirelessDevice::deviceType() const
{
    return DeviceType::Wireless;
}

QList<AccessPoints *> WirelessDevice::accessPointItems() const
{
    /* 理论上，如果网卡是禁用状态，这里应该直接返回空列表
     * 但是由于在禁用网卡或者打开网卡的时候，后台的反应比较慢，控制中心无法实时获取，所以这里暂时让其先返回所有的网络列表，
     * 等后台反应慢的问题改好后，再把注释打开
     * if (!isEnabled())
        return QList<UAccessPoints *>();*/

    return deviceRealize()->accessPointItems();
}

void WirelessDevice::scanNetwork()
{
    deviceRealize()->scanNetwork();
}

void WirelessDevice::connectNetwork(const AccessPoints *item)
{
    deviceRealize()->connectNetwork(item);
}

AccessPoints *WirelessDevice::activeAccessPoints() const
{
    return deviceRealize()->activeAccessPoints();
}

bool WirelessDevice::hotspotEnabled() const
{
    return deviceRealize()->hotspotEnabled();
}

void WirelessDevice::connectNetwork(const QString &ssid)
{
    AccessPoints *apConnection = findAccessPoint(ssid);
    if (apConnection)
        connectNetwork(apConnection);
}

QList<WirelessConnection *> WirelessDevice::items() const
{
    return deviceRealize()->wirelessItems();
}

WirelessDevice::WirelessDevice(NetworkDeviceRealize *networkInter, QObject *parent)
    : NetworkDeviceBase(networkInter, parent)
{
    connect(networkInter, &NetworkDeviceRealize::networkAdded, this, &WirelessDevice::networkAdded);
    connect(networkInter, &NetworkDeviceRealize::networkRemoved, this, &WirelessDevice::networkRemoved);
    connect(networkInter, &NetworkDeviceRealize::connectionFailed, this, &WirelessDevice::connectionFailed);
    connect(networkInter, &NetworkDeviceRealize::connectionSuccess, this, &WirelessDevice::connectionSuccess);
    connect(networkInter, &NetworkDeviceRealize::hotspotEnableChanged, this, &WirelessDevice::hotspotEnableChanged);
    connect(networkInter, &NetworkDeviceRealize::accessPointInfoChanged, this, &WirelessDevice::accessPointInfoChanged);
}

WirelessDevice::~WirelessDevice()
{
}

AccessPoints *WirelessDevice::findAccessPoint(const QString &ssid)
{
    QList<AccessPoints *> accessPoints = deviceRealize()->accessPointItems();
    for (AccessPoints *accessPoint : accessPoints) {
        if (accessPoint->ssid() == ssid)
            return accessPoint;
    }

    return Q_NULLPTR;
}

/**
 * @brief 无线网络连接
 */
AccessPoints::AccessPoints(const QJsonObject &json, QObject *parent)
    : QObject(parent)
    , m_json(json)
    , m_status(ConnectionStatus::Unknown)
{
}

AccessPoints::~AccessPoints()
{
}

QString AccessPoints::ssid() const
{
    return m_json.value("Ssid").toString();
}

int AccessPoints::strength() const
{
    if (m_json.isEmpty())
        return -1;

    return m_json.value("Strength").toInt();
}

bool AccessPoints::secured() const
{
    return m_json.value("Secured").toBool();
}

bool AccessPoints::securedInEap() const
{
    return m_json.value("SecuredInEap").toBool();
}

int AccessPoints::frequency() const
{
    return m_json.value("Frequency").toInt();
}

QString AccessPoints::path() const
{
    return m_json.value("Path").toString();
}

QString AccessPoints::devicePath() const
{
    return m_devicePath;
}

bool AccessPoints::connected() const
{
    return (m_status == ConnectionStatus::Activated);
}

ConnectionStatus AccessPoints::status() const
{
    return m_status;
}

bool AccessPoints::hidden() const
{
    if (m_json.contains("Hidden"))
        return m_json.value("Hidden").toBool();

    return false;
}

AccessPoints::WlanType AccessPoints::type() const
{
    // 根据需求，在当前Wlan未连接的情况下，才判断是否有同名Wlan中存在Wlan6，如果当前已连接，则直接判断
    if (!connected()) {
        // 如果是重名的Wlan，则判断同名的Wlan的Flags是否为Wlan6
        if (m_json.contains("extendFlags")) {
            int flag = m_json.value("extendFlags").toInt();
            if (flag & AP_FLAGS_HE)
                return WlanType::wlan6;
        }
    }

    if (m_json.contains("Flags")) {
        int flag = m_json.value("Flags").toInt();
        if (flag & AP_FLAGS_HE)
            return WlanType::wlan6;
    }

    return WlanType::wlan;
}

void AccessPoints::updateAccessPoints(const QJsonObject &json)
{
    int nOldStrength = strength();
    m_json = json;
    int nStrength = strength();
    if (nOldStrength != -1 && nStrength != nOldStrength)
        Q_EMIT strengthChanged(nStrength);
}

/**
 * @brief 无线连接信息
 */
AccessPoints *WirelessConnection::accessPoints() const
{
    return m_accessPoints;
}

bool WirelessConnection::connected()
{
    return m_accessPoints ? m_accessPoints->connected() : false;
}

WirelessConnection::WirelessConnection()
    : ControllItems()
    , m_accessPoints(Q_NULLPTR)
{
}

WirelessConnection::~WirelessConnection()
{
}

WirelessConnection *WirelessConnection::createConnection(AccessPoints *ap)
{
    WirelessConnection *wirelessConnection = new WirelessConnection;
    QJsonObject json;
    json.insert("Ssid", ap->ssid());
    wirelessConnection->setConnection(json);
    wirelessConnection->m_accessPoints = ap;
    return wirelessConnection;
}
