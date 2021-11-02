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

namespace dde {

namespace network {

class ProxyController;
class VPNController;
class DSLController;
class HotspotController;
class NetworkDetails;
class NetworkDeviceBase;
class NetworkProcesser;

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
    static void setServiceType(const ServiceLoadType serviceType);             // 是否系统级（用于在登录界面，此时尚未进入用户，无法访问用户接口）
    ProxyController *proxyController();                                          // 返回代理控制管理器
    VPNController *vpnController();                                              // 返回VPN控制器
    DSLController *dslController();                                              // DSL控制器
    HotspotController *hotspotController();                                      // 个人热点控制器
    QList<NetworkDetails *> networkDetails();                                    // 网络详情
    QList<NetworkDeviceBase *> devices() const;                                  // 当前网络设备列表
    Connectivity connectivity();                                                 // 获取网络连接状态

Q_SIGNALS:
    void deviceAdded(QList<NetworkDeviceBase *>);                                 // 新增设备发出的信号
    void deviceRemoved(QList<NetworkDeviceBase *>);                               // 移除设备发出的信号
    void connectivityChanged(const Connectivity &) const;                         // 网络连接状态发生变化的时候发出的信号
    void connectionChanged();                                                     // 连接信息发生变化的时候触发的信号
    void activeConnectionChange();                                                // 网络数据发生变化

protected:
    explicit NetworkController();
    ~NetworkController();

private:
    NetworkProcesser *m_processer;
    ProxyController *m_proxyController;
    VPNController *m_vpnController;
    DSLController *m_dslController;
    HotspotController *m_hotspotController;
    QList<NetworkDetails *> m_networkDetails;
    static bool m_sync;
    static ServiceLoadType m_serviceLoadType;
};

}

}

#endif // UNETWORKCONTROLLER_H
