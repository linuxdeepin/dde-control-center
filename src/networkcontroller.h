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

#ifndef NETWORKCONTROLLER_H
#define NETWORKCONTROLLER_H

#include "networkconst.h"

#include <QObject>

class QThread;

namespace dde {

namespace network {

class ProxyController;
class VPNController;
class DSLController;
class HotspotController;
class NetworkDetails;
class NetworkDeviceBase;

enum InterfaceFlags {
    NM_DEVICE_INTERFACE_FLAG_NONE     = 0,          // an alias for numeric zero, no flags set.
    NM_DEVICE_INTERFACE_FLAG_UP       = 0x1,        // the interface is enabled from the administrative point of view. Corresponds to kernel IFF_UP.
    NM_DEVICE_INTERFACE_FLAG_LOWER_UP = 0x2,        // the physical link is up. Corresponds to kernel IFF_LOWER_UP.
    NM_DEVICE_INTERFACE_FLAG_CARRIER  = 0x10000,    // the interface has carrier. In most cases this is equal to the value of @NM_DEVICE_INTERFACE_FLAG_LOWER_UP
};

class NetworkController : public QObject
{
    Q_OBJECT

public:
    static NetworkController *instance();                                        // 对外返回的单例
    static void setActiveSync(const bool sync = false);                          // 设置QDbus服务通讯的方式 ,true表示同步获取，false表示异步获取
    ProxyController *proxyController();                                          // 返回代理控制管理器
    VPNController *vpnController();                                              // 返回VPN控制器
    DSLController *dslController();                                              // DSL控制器
    HotspotController *hotspotController();                                      // 个人热点控制器
    inline QList<NetworkDetails *> networkDetails() { return m_networkDetails; } // 网络详情
    inline QList<NetworkDeviceBase *> devices() const { return m_devices; }      // 当前网络设备列表
    inline Connectivity connectivity() { return m_connectivity; }                // 获取网络连接状态

Q_SIGNALS:
    void deviceAdded(QList<NetworkDeviceBase *>);                                 // 新增设备发出的信号
    void deviceRemoved(QList<NetworkDeviceBase *>);                               // 移除设备发出的信号
    void connectivityChanged(const Connectivity &) const;                        // 网络连接状态发生变化的时候发出的信号
    void connectionChanged();                                                    // 连接信息发生变化的时候触发的信号
    void activeConnectionChange();                                               // 网络数据发生变化

protected:
    explicit NetworkController();
    ~NetworkController();

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

    void updateDeviceName();                                                       // 更改设备的名称

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
    static bool m_sync;
};

}

}

#endif // UNETWORKCONTROLLER_H
