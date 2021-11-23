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

#ifndef NETWORKMANAGERPROCESSER_H
#define NETWORKMANAGERPROCESSER_H

#include "netinterface.h"
#include "networkconst.h"

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

private slots:
    void onDeviceChanged();
    void onConnectivityChanged(NetworkManager::Connectivity conntity);
    void onDeviceEnabledChanged(QDBusObjectPath path, bool enabled);

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
