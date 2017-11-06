/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
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

#ifndef NETWORKMODEL_H
#define NETWORKMODEL_H

#include "networkmodule.h"
#include "networkdevice.h"

#include <QObject>
#include <QMap>
#include <QTimer>
#include <QDBusObjectPath>

namespace dcc {

namespace network {

struct ProxyConfig
{
    QString type;
    QString url;
    QString port;
    QString username;
    QString password;
};

class NetworkDevice;
class NetworkWorker;
class NetworkModel : public QObject
{
    Q_OBJECT

    friend class NetworkWorker;
    friend class NetworkModule;

public:
    explicit NetworkModel(QObject *parent = 0);
    ~NetworkModel();
    ProxyConfig getChainsProxy() { return m_chainsProxy;}

    bool vpnEnabled() const { return m_vpnEnabled; }
    const ProxyConfig proxy(const QString &type) const { return m_proxies[type]; }
    const QString autoProxy() const { return m_autoProxy; }
    const QString proxyMethod() const { return m_proxyMethod; }
    const QString ignoreHosts() const { return m_proxyIgnoreHosts; }
    const QList<NetworkDevice *> devices() const { return m_devices; }
    const QList<QJsonObject> vpns() const { return m_connections.value("vpn"); }
    const QList<QJsonObject> wireds() const { return m_connections.value("wired"); }
    const QList<QJsonObject> pppoes() const { return m_connections.value("pppoe"); }
    const QList<QJsonObject> hotspots() const { return m_connections.value("wireless-hotspot"); }
    const QList<QJsonObject> activeConnInfos() const { return m_activeConnInfos; }
    const QSet<QString> activeConnections() const { return m_activeConnections; }
    const QString connectionUuidByPath(const QString &connPath) const;
    const QString connectionNameByPath(const QString &connPath) const;
    const QString connectionUuidByApInfo(const QString &hwAddr, const QString &ssid) const;
    const QJsonObject connectionByUuid(const QString &uuid) const;
    const QJsonObject connectionByPath(const QString &connPath) const;
    const QJsonObject activeConnObjectByUuid(const QString &uuid) const;

signals:
    void connectionListChanged() const;
    void deviceEnableChanged(const QString &device, const bool enabled) const;
    void autoProxyChanged(const QString &proxy) const;
    void proxyChanged(const QString &type, const ProxyConfig &config) const;
    void proxyMethodChanged(const QString &proxyMethod) const;
    void proxyIgnoreHostsChanged(const QString &hosts) const;
    void requestDeviceStatus(const QString &devPath) const;
    void activeConnectionsChanged(const QSet<QString> &conns) const;
    void activeConnInfoChanged(const QList<QJsonObject> &infos) const;
    void vpnEnabledChanged(const bool enabled) const;
    void deviceListChanged(const QList<NetworkDevice *> devices) const;
    void unhandledConnectionSessionCreated(const QString &device, const QString &sessionPath) const;
    void chainsTypeChanged(const QString &type) const;
    void chainsAddrChanged(const QString &addr) const;
    void chainsPortChanged(const QString& port) const;
    void chainsUsernameChanged(const QString &username) const;
    void chainsPasswdChanged(const QString &passwd) const;

private slots:
    void onVPNEnabledChanged(const bool enabled);
    void onProxiesChanged(const QString &type, const QString &url, const QString &port);
    void onAutoProxyChanged(const QString &proxy);
    void onProxyMethodChanged(const QString &proxyMethod);
    void onProxyIgnoreHostsChanged(const QString &hosts);
    void onDeviceListChanged(const QString &devices);
    void onConnectionListChanged(const QString &conns);
    void onActiveConnInfoChanged(const QString &conns);
    void onActiveConnectionsChanged(const QString &conns);
    void onConnectionSessionCreated(const QString &device, const QString &sessionPath);
    void onDeviceAPListChanged(const QString &device, const QString &apList);
    void onDeviceAPInfoChanged(const QString &device, const QString &apInfo);
    void onDeviceAPRemoved(const QString &device, const QString &apInfo);
    void onDeviceEnableChanged(const QString &device, const bool enabled);
    void onDeviceConnectionsChanged(const QString &devPath, const QList<QDBusObjectPath> &connections);
    void onChainsTypeChanged(const QString &type);
    void onChainsAddrChanged(const QString &addr);
    void onChainsPortChanged(const QString &port);
    void onChainsUserChanged(const QString &user);
    void onChainsPasswdChanged(const QString &passwd);

private:
    bool containsDevice(const QString &devPath) const;
    NetworkDevice *device(const QString &devPath) const;
    void updateWiredConnInfo();

private:
    bool m_vpnEnabled;
    QString m_proxyMethod;
    QString m_proxyIgnoreHosts;
    QString m_autoProxy;
    QMap<QString, ProxyConfig> m_proxies;
    QList<NetworkDevice *> m_devices;
    QMap<QString, QList<QJsonObject>> m_connections;
    QList<QJsonObject> m_activeConnInfos;
    QList<QJsonObject> m_activeHotspotInfos;
    QList<QJsonObject> m_activeConnObjects;
    QSet<QString> m_activeConnections;
    ProxyConfig m_chainsProxy;
};

}   // namespace network

}   // namespace dcc

#endif // NETWORKMODEL_H
