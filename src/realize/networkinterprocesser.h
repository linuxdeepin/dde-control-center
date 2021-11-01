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

#ifndef NETWORKPROCESSER_H
#define NETWORKPROCESSER_H

#include "netinterface.h"

namespace dde {
namespace network {

class NetworkDeviceBase;
class ProxyController;
class VPNController;
class DSLController;
class HotspotController;
class NetworkDetails;

class NetworkInterProcesser : public NetworkProcesser
{
    Q_OBJECT

    friend class NetworkController;

protected:
    explicit NetworkInterProcesser(bool sync, QObject *parent);
    ~NetworkInterProcesser() override;

    void initDeviceService();
    void initConnection();
    void initNetData(NetworkInter *networkInt);                                    // 填充基本数据

    NetworkDeviceBase *findDevices(const QString &path) const;                     // 根据设备path查找设备
    void updateConnectionsInfo(const QList<NetworkDeviceBase *> &devices);         // 更新设备连接信息
    void asyncActiveConnectionInfo();                                              // 同步IP地址等信息
    void activeInfoChanged(const QString &conns);                                  // 更新活动连接信息
    void updateDeviceConnectiveInfo();                                             // 更新设备信号连接强度信息
    void activeConnInfoChanged(const QString &conns);                              // 活动连接信息发生变化
    void updateDeviceHotpot();                                                     // 更新热点设备数据
    void updateDeviceActiveHotpot();                                               // 更换当前活跃热点信息

    void updateNetworkDetails();                                                   // 更新网络连接信息

    void updateDSLData();                                                          // 更新DSL的数据
    void updateVPNActiveConnection();                                              // 更新VPN的活动连接信息

protected:
    ProxyController *proxyController() override;                                            // 返回代理控制管理器
    VPNController *vpnController() override;                                                // 返回VPN控制器
    DSLController *dslController() override;                                                // DSL控制器
    HotspotController *hotspotController() override;                                        // 个人热点控制器
    Connectivity connectivity() override;                                                   // 获取当前的信号
    QList<NetworkDeviceBase *> devices() override;                                          // 获取当前的所有的设备
    QList<NetworkDetails *> networkDetails() override;                                      // 获取设备的网络详情

protected Q_SLOTS:
    void onDevicesChanged(const QString &value);
    void onConnectionListChanged(const QString &connections);
    void onActiveConnectionsChanged(const QString &activeConnections);

    void onAccesspointChanged(const QString &accessPoints);
    void onDeviceEnableChanged(const QString &devicePath, bool enabled);
    void onConnectivityChanged(int conectivity);

private:
    ProxyController *m_proxyController;
    VPNController *m_vpnController;
    DSLController *m_dslController;
    HotspotController *m_hotspotController;
    QList<NetworkDeviceBase *> m_devices;
    NetworkInter *m_networkInter;
    QJsonObject m_connections;
    Connectivity m_connectivity;
    QJsonArray m_activeConnectionInfo;
    QJsonObject m_activeConection;
    QList<NetworkDetails *> m_networkDetails;
    bool m_sync;
};

}
}

#endif // NETWORKPROCESSER_H
