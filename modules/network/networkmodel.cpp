/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
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

#include "networkmodel.h"
#include "networkdevice.h"
#include "wirelessdevice.h"
#include "wireddevice.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

using namespace dcc::network;

NetworkDevice::DeviceType parseDeviceType(const QString &type)
{
    if (type == "wireless") {
        return NetworkDevice::Wireless;
    }
    if (type == "wired") {
        return NetworkDevice::Wired;
    }

    return NetworkDevice::None;
}

NetworkModel::NetworkModel(QObject *parent)
    : QObject(parent)
{
}

NetworkModel::~NetworkModel()
{
    qDeleteAll(m_devices);
}

const QString NetworkModel::connectionUuidByPath(const QString &connPath) const
{
    return connectionByPath(connPath).value("Uuid").toString();
}

const QString NetworkModel::connectionNameByPath(const QString &connPath) const
{
    return connectionByPath(connPath).value("Id").toString();
}

const QJsonObject NetworkModel::connectionByPath(const QString &connPath) const
{
    for (const auto &list : m_connections)
    {
        for (const auto &cfg : list)
        {
            if (cfg.value("Path").toString() == connPath)
                return cfg;
        }
    }

    return QJsonObject();
}

const QJsonObject NetworkModel::activeConnObjectByUuid(const QString &uuid) const
{
    for (const auto &info : m_activeConnObjects)
    {
        if (info.value("Uuid").toString() == uuid)
            return info;
    }

    return QJsonObject();
}

const QString NetworkModel::connectionUuidByApInfo(const QString &hwAddr, const QString &ssid) const
{
    for (const auto &list : m_connections)
    {
        for (const auto &cfg : list)
        {
            if (cfg.value("HwAddress").toString() == hwAddr && cfg.value("Ssid").toString() == ssid)
                return cfg.value("Uuid").toString();
        }
    }

    return QString();
}

const QJsonObject NetworkModel::connectionByUuid(const QString &uuid) const
{
    for (const auto &list : m_connections)
    {
        for (const auto &cfg : list)
        {
            if (cfg.value("Uuid").toString() == uuid)
                return cfg;
        }
    }

    return QJsonObject();
}

void NetworkModel::onVPNEnabledChanged(const bool enabled)
{
    if (m_vpnEnabled != enabled)
    {
        m_vpnEnabled = enabled;

        emit vpnEnabledChanged(m_vpnEnabled);
    }
}

void NetworkModel::onProxiesChanged(const QString &type, const QString &url, const QString &port)
{
    const ProxyConfig config = { type, url, port, "", "" };
    const ProxyConfig old = m_proxies[type];


    if (old.url != config.url || old.port != config.port)
    {
        m_proxies[type] = config;

        emit proxyChanged(type, config);
    }
}

void NetworkModel::onAutoProxyChanged(const QString &proxy)
{
    if (m_autoProxy != proxy)
    {
        m_autoProxy = proxy;

        emit autoProxyChanged(m_autoProxy);
    }
}

void NetworkModel::onProxyMethodChanged(const QString &proxyMethod)
{
    if (m_proxyMethod != proxyMethod)
    {
        m_proxyMethod = proxyMethod;

        emit proxyMethodChanged(m_proxyMethod);
    }
}

void NetworkModel::onProxyIgnoreHostsChanged(const QString &hosts)
{
    if (hosts != m_proxyIgnoreHosts)
    {
        m_proxyIgnoreHosts = hosts;

        emit proxyIgnoreHostsChanged(m_proxyIgnoreHosts);
    }
}

void NetworkModel::onDeviceListChanged(const QString &devices)
{
    const QJsonObject data = QJsonDocument::fromJson(devices.toUtf8()).object();

    QSet<QString> devSet;

    for (auto it(data.constBegin()); it != data.constEnd(); ++it) {
        const auto type = parseDeviceType(it.key());
        const auto list = it.value().toArray();

        if (type == NetworkDevice::None)
            continue;

        for (auto const &l : list)
        {
            const auto info = l.toObject();
            const QString path = info.value("Path").toString();

            if (devSet.contains(path))
                continue;
            else
                devSet << path;

            NetworkDevice *d = device(path);
            if (!d)
            {
                switch (type)
                {
                case NetworkDevice::Wireless:   d = new WirelessDevice(info, this);      break;
                case NetworkDevice::Wired:      d = new WiredDevice(info, this);         break;
                default:;
                }
                m_devices.append(d);

                // init device enabled status
                emit requestDeviceStatus(d->path());
            } else {
                d->updateDeviceInfo(info);
            }
        }
    }

    // remove unexist device
    QList<NetworkDevice *> removeList;
    for (auto const d : m_devices)
    {
        if (!devSet.contains(d->path()))
            removeList << d;
    }

    for (auto const r : removeList)
        m_devices.removeOne(r);
    qDeleteAll(removeList);

    emit deviceListChanged(m_devices);
}

void NetworkModel::onConnectionListChanged(const QString &conns)
{
    QMap<QString, QList<QString>> devicesConnectionsList;

    const QJsonObject connsObject = QJsonDocument::fromJson(conns.toUtf8()).object();
    for (auto it(connsObject.constBegin()); it != connsObject.constEnd(); ++it)
    {
        const auto &connList = it.value().toArray();
        const auto &connType = it.key();
        m_connections[connType].clear();

        for (const auto &connObject : connList)
        {
            const QJsonObject &connection = connObject.toObject();

            m_connections[connType].append(connection);

            const auto &hwAddr = connection.value("HwAddress").toString();
            if (!hwAddr.isEmpty())
                devicesConnectionsList[hwAddr] << connection.value("Path").toString();
        }
    }

    for (NetworkDevice *dev : m_devices)
    {
        if (dev->type() != NetworkDevice::Wired)
            continue;

        WiredDevice *wDev = static_cast<WiredDevice *>(dev);
        const QString &hwAdr = wDev->realHwAdr();

        if (devicesConnectionsList.contains(hwAdr))
            wDev->setConnections(devicesConnectionsList[hwAdr]);
    }

    emit connectionListChanged();
}

void NetworkModel::onActiveConnInfoChanged(const QString &conns)
{
    m_activeConnInfos.clear();

    QMap<QString, QJsonObject> activeConnInfo;
    QMap<QString, QJsonObject> activeHotspotInfo;

    QJsonArray activeConns = QJsonDocument::fromJson(conns.toUtf8()).array();
    for (const auto &info : activeConns)
    {
        const auto &connInfo = info.toObject();
        const auto &type = connInfo.value("ConnectionType").toString();
        const auto &macAddr = connInfo.value("HwAddress").toString();

        activeConnInfo[macAddr] = connInfo;
        m_activeConnInfos << connInfo;

        if (type == "wireless-hotspot")
            activeHotspotInfo[macAddr] = connInfo;
    }

    // update device active connection info
    for (auto *dev : m_devices)
    {
        const auto &hwAdr = dev->usingHwAdr();

        switch (dev->type())
        {
        case NetworkDevice::Wired:
        {
            WiredDevice *d = static_cast<WiredDevice *>(dev);
            d->onActiveConnectionChanged(activeConnInfo[hwAdr]);
            break;
        }
        case NetworkDevice::Wireless:
        {
            WirelessDevice *d = static_cast<WirelessDevice *>(dev);
            d->setActiveApName(activeConnInfo[hwAdr].value("ConnectionName").toString());
            d->setHotspotInfo(activeHotspotInfo.value(hwAdr));
            break;
        }
        default:;
        }
    }

    emit activeConnInfoChanged(m_activeConnInfos);
}

void NetworkModel::onActiveConnectionsChanged(const QString &conns)
{
    m_activeConnections.clear();
    m_activeConnObjects.clear();

    const QJsonObject activeConns = QJsonDocument::fromJson(conns.toUtf8()).object();
    for (auto it(activeConns.constBegin()); it != activeConns.constEnd(); ++it)
    {
        const QJsonObject &info = it.value().toObject();
        const auto &uuid = info.value("Uuid").toString();
        if (uuid.isEmpty())
            continue;

        m_activeConnections << uuid;
        m_activeConnObjects << info;
    }

    emit activeConnectionsChanged(m_activeConnections);
}

void NetworkModel::onConnectionSessionCreated(const QString &device, const QString &sessionPath)
{
    for (const auto dev : m_devices)
    {
        if (dev->path() != device)
            continue;
        emit dev->sessionCreated(sessionPath);
        return;
    }

    emit unhandledConnectionSessionCreated(device, sessionPath);
}

void NetworkModel::onDeviceAPListChanged(const QString &device, const QString &apList)
{
    for (auto const dev : m_devices)
    {
        if (dev->type() != NetworkDevice::Wireless || dev->path() != device)
            continue;
        return static_cast<WirelessDevice *>(dev)->setAPList(apList);
    }
}

void NetworkModel::onDeviceAPInfoChanged(const QString &device, const QString &apInfo)
{
    for (auto const dev : m_devices)
    {
        if (dev->type() != NetworkDevice::Wireless || dev->path() != device)
            continue;
        return static_cast<WirelessDevice *>(dev)->updateAPInfo(apInfo);
    }
}

void NetworkModel::onDeviceAPRemoved(const QString &device, const QString &apInfo)
{
    for (auto const dev : m_devices)
    {
        if (dev->type() != NetworkDevice::Wireless || dev->path() != device)
            continue;
        return static_cast<WirelessDevice *>(dev)->deleteAP(apInfo);
    }
}


void NetworkModel::onDeviceEnableChanged(const QString &device, const bool enabled)
{
    NetworkDevice *dev = nullptr;
    for (auto const d : m_devices)
    {
        if (d->path() == device)
        {
            dev = d;
            break;
        }
    }

    if (!dev)
        return;

    dev->setEnabled(enabled);

    emit deviceEnableChanged(device, enabled);
}

void NetworkModel::onDeviceConnectionsChanged(const QString &devPath, const QList<QString> &connections)
{
    WiredDevice *dev = static_cast<WiredDevice *>(device(devPath));
    Q_ASSERT(dev);

    dev->setConnections(connections);
}

void NetworkModel::onChainsTypeChanged(const QString &type)
{
    if (type != m_chainsProxy.type) {
        m_chainsProxy.type = type;
        emit chainsTypeChanged(type);
    }
}

void NetworkModel::onChainsAddrChanged(const QString &addr)
{
    if (addr != m_chainsProxy.url) {
        m_chainsProxy.url = addr;
        emit chainsAddrChanged(addr);
    }
}

void NetworkModel::onChainsPortChanged(const QString &port)
{
    if (port != m_chainsProxy.port) {
        m_chainsProxy.port = port;
        emit chainsPortChanged(port);
    }
}

void NetworkModel::onChainsUserChanged(const QString &user)
{
    if (user != m_chainsProxy.username) {
        m_chainsProxy.username = user;
        emit chainsUsernameChanged(user);
    }
}

void NetworkModel::onChainsPasswdChanged(const QString &passwd)
{
    if (passwd != m_chainsProxy.password) {
        m_chainsProxy.password = passwd;
        emit chainsPasswdChanged(passwd);
    }
}

bool NetworkModel::containsDevice(const QString &devPath) const
{
    return device(devPath) != nullptr;
}

NetworkDevice *NetworkModel::device(const QString &devPath) const
{
    for (auto const d : m_devices)
        if (d->path() == devPath)
            return d;

    return nullptr;
}
