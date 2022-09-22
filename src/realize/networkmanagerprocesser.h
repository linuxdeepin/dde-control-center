// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef NETWORKMANAGERPROCESSER_H
#define NETWORKMANAGERPROCESSER_H

#include "netinterface.h"
#include "networkconst.h"
#include "netutils.h"

#include <networkmanagerqt/manager.h>

namespace dde {
namespace network {

class NetworkDeviceBase;
class IPConfilctChecker;

class NetworkManagerProcesser : public NetworkProcesser
{
    Q_OBJECT

public:
    explicit NetworkManagerProcesser(QObject *parent = Q_NULLPTR);
    ~NetworkManagerProcesser() override;

protected:
    QList<NetworkDeviceBase *> devices() override;
    dde::network::Connectivity connectivity() override;
    QList<NetworkDetails *> networkDetails() override;
    ProxyController *proxyController() override;                                          // 返回代理控制管理器
    VPNController *vpnController() override;                                              // 返回VPN控制器
    DSLController *dslController() override;                                              // DSL控制器
    HotspotController *hotspotController() override;                                      // 个人热点控制器

private:
    void initConnections();
    NetworkDeviceBase *findDevice(const QString devicePath);
    NetworkInter *networkInter();
    void sortDevice();

private slots:
    void onDeviceAdded(const QString &uni);
    void onDeviceRemove(const QString &uni);
    void onConnectivityChanged(NetworkManager::Connectivity conntity);
    void onDeviceEnabledChanged(QDBusObjectPath path, bool enabled);
    void onDevicesChanged(const QList<QDBusObjectPath> &devices);
    void checkConnectivityFinished(quint32 conntity);
    void onDeviceAdded(const QDBusObjectPath &device);
    void onDeviceRemoved(const QDBusObjectPath &device);
    void onPropertiesChanged(const QString &interfaceName, const QVariantMap &changedProperties, const QStringList &invalidatedProperties);

private:
    QList<NetworkDeviceBase *> m_devices;
    QList<NetworkDetails *> m_details;
    ProxyController *m_proxyController;
    VPNController *m_vpnController;
    DSLController *m_dslController;
    HotspotController *m_hotspotController;
    NetworkInter *m_networkInter;
    dde::network::Connectivity m_connectivity;
    IPConfilctChecker *m_ipChecker;
};

}
}

#endif // NETWORKMANAGERPROCESSER_H
