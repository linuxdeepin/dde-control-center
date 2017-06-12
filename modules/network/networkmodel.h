#ifndef NETWORKMODEL_H
#define NETWORKMODEL_H

#include "networkdevice.h"

#include <QObject>
#include <QMap>
#include <QTimer>
#include <QDBusObjectPath>

namespace dcc {

namespace network {

struct ProxyConfig
{
    QString url;
    QString port;
};

class NetworkDevice;
class NetworkWorker;
class NetworkModel : public QObject
{
    Q_OBJECT

    friend class NetworkWorker;

public:
    explicit NetworkModel(QObject *parent = 0);
    ~NetworkModel();

    bool vpnEnabled() const { return m_vpnEnabled; }
    const ProxyConfig proxy(const QString &type) const { return m_proxies[type]; }
    const QString autoProxy() const { return m_autoProxy; }
    const QString proxyMethod() const { return m_proxyMethod; }
    const QString ignoreHosts() const { return m_proxyIgnoreHosts; }
    const QList<NetworkDevice *> devices() const { return m_devices; }
    const QList<QJsonObject> vpns() const { return m_connections.value("vpn"); }
    const QList<QJsonObject> wireds() const { return m_connections.value("wired"); }
    const QList<QJsonObject> pppoes() const { return m_connections.value("pppoe"); }
    const QList<QJsonObject> activeConnInfos() const { return m_activeConnInfos; }
    const QSet<QString> activeConnections() const { return m_activeConnections; }
    const QString connectionUuidByPath(const QString &connPath) const;
    const QString connectionNameByPath(const QString &connPath) const;
    const QJsonObject connectionByPath(const QString &connPath) const;
    const QString connectionUuidByApInfo(const QString &hwAddr, const QString &ssid) const;

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
    QSet<QString> m_activeConnections;
};

}   // namespace network

}   // namespace dcc

#endif // NETWORKMODEL_H
