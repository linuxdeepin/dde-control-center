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

#ifndef NETINTERFACE_H
#define NETINTERFACE_H

#include "networkconst.h"

#include <QObject>
#include <QQueue>

namespace dde {
namespace network {

class NetworkDeviceBase;
class ProxyController;
class VPNController;
class DSLController;
class HotspotController;
class NetworkDetails;
class DSLItem;
class VPNItem;
class HotspotItem;
class WirelessDevice;
class AccessPoints;
class WiredDevice;
class WiredConnection;
class WirelessConnection;
class IPConfilctChecker;

class NetworkProcesser : public QObject
{
    Q_OBJECT

public:
    virtual QList<NetworkDeviceBase *> devices() = 0;
    virtual Connectivity connectivity() = 0;
    virtual QList<NetworkDetails *> networkDetails();
    virtual ProxyController *proxyController();                                          // 返回代理控制管理器
    virtual VPNController *vpnController();                                              // 返回VPN控制器
    virtual DSLController *dslController();                                              // DSL控制器
    virtual HotspotController *hotspotController();                                      // 个人热点控制器
    virtual void retranslate();                                                          // 更新翻译

protected:
    explicit NetworkProcesser(QObject *parent);
    virtual ~NetworkProcesser();

Q_SIGNALS:
    void deviceAdded(QList<NetworkDeviceBase *>);                                        // 新增设备发出的信号
    void deviceRemoved(QList<NetworkDeviceBase *>);                                      // 移除设备发出的信号
    void connectivityChanged(const Connectivity &) const;                                // 网络连接状态发生变化的时候发出的信号
    void connectionChanged();                                                            // 连接信息发生变化的时候触发的信号
    void activeConnectionChange();                                                       // 网络数据发生变化

protected:
    void updateDeviceName();                                                             // 更新设备名称
};

class NetworkDeviceRealize : public QObject
{
    Q_OBJECT

    friend class NetworkDeviceBase;
    friend class NetworkInterProcesser;
    friend class NetworkManagerProcesser;

Q_SIGNALS:
    void deviceStatusChanged(const DeviceStatus &) const;                                // 状态发生变化的时候触发的信号
    void enableChanged(const bool) const;                                                // 开启设备或禁用设备的时候发出的信号(参数值True表示开启设备，False表示禁用设备)
    void connectionChanged();                                                            // 连接发生变化的时候触发的信号
    void nameChanged(const QString &);                                                   // 网卡名称发生变化的时候触发的信号
    void removed();                                                                      // 设备移除
    // 无线网络信号
    void networkAdded(QList<AccessPoints *>);                                            // wlan新增网络
    void networkRemoved(QList<AccessPoints *>);                                          // wlan列表减少网络
    void connectionFailed(const AccessPoints *);                                         // 连接无线wlan失败，第一个参数为失败的热点，第二个参数为对应的connection的Uuid
    void connectionSuccess(const AccessPoints *);                                        // 连接无线网络wlan成功，参数为对应的wlan
    void hotspotEnableChanged(const bool &);                                             // 热点是否可用发生变化
    void accessPointInfoChanged(const QList<AccessPoints *> &);                          // wlan信号强度发生变化的网络
    void activeConnectionChanged();                                                      // 活动连接发生变化的时候发出的信号
    // 有线设备的信号
    void connectionAdded(const QList<WiredConnection *>);                                // 新增连接
    void connectionRemoved(const QList<WiredConnection *>);                              // 删除连接
    void connectionPropertyChanged(const QList<WiredConnection *> &);                    // 连接属性发生变化
    void ipV4Changed();                                                                  // IPv4地址发生变化

public:
    void setDevice(NetworkDeviceBase *device);                                           // 设置当前的设备
    virtual bool isEnabled() const;                                                      // 当前的网卡是否启用
    virtual bool IPValid();                                                              // IP是否合法
    virtual QString interface() const;                                                   // 返回设备上的Interface
    virtual QString driver() const;                                                      // 驱动，对应于备上返回值的Driver
    virtual bool managed() const;                                                        // 对应于设备上返回值的Managed
    virtual QString vendor() const;                                                      // 对应于设备上返回值的Vendor
    virtual QString uniqueUuid() const;                                                  // 网络设备的唯一的UUID，对应于设备上返回值的UniqueUuid
    virtual bool usbDevice() const;                                                      // 是否是USB设备，对应于设备上返回值的UsbDevice
    virtual QString path() const;                                                        // 设备路径，对应于设备上返回值的Path
    virtual QString activeAp() const;                                                    // 对应于设备上返回值的ActiveAp
    virtual bool supportHotspot() const;                                                 // 是否支持热点,对应于设备上返回值的SupportHotspot
    virtual QString realHwAdr() const;                                                   // mac地址
    virtual QString usingHwAdr() const;                                                  // 正在使用的mac地址
    virtual QStringList ipv4() const;                                                        // IPV4地址
    virtual QStringList ipv6() const;                                                        // IPV6地址
    virtual QJsonObject activeConnectionInfo() const;                                    // 获取当前活动连接的信息
    virtual void setEnabled(bool enabled);                                               // 开启或禁用网卡
    virtual void disconnectNetwork();                                                    // 断开网络连接，该方法是一个虚方法，具体在子类
    Connectivity connectivity();
    virtual DeviceStatus deviceStatus() const;

    // 无线网络接口
    virtual QList<AccessPoints *> accessPointItems() const;                              // 当前网卡上所有的网络列表
    virtual void scanNetwork() {}                                                        // 重新加载所有的无线网络列表
    virtual void connectNetwork(const AccessPoints *item);                               // 连接网络，连接成功抛出deviceStatusChanged信号
    virtual QList<WirelessConnection *> wirelessItems() const;                           // 无线网络连接列表
    virtual AccessPoints *activeAccessPoints() const;                                    // 当前活动的无线连接
    virtual bool hotspotEnabled() { return false; }                                      // 无线网络是否开启热点

    // 有线网络接口
    virtual bool connectNetwork(WiredConnection *connection);                            // 连接网络，连接成功抛出deviceStatusChanged信号
    virtual QList<WiredConnection *> wiredItems() const;                                 // 有线网络连接列表

protected:
    NetworkDeviceRealize(IPConfilctChecker *ipConflictChecker, QObject *parent);
    virtual ~NetworkDeviceRealize();

    NetworkDeviceBase *device() const;

    virtual QString statusStringDetail();
    virtual QString getStatusName();
    void enqueueStatus(const DeviceStatus &status);
    void setDeviceStatus(const DeviceStatus &status);
    void sortWiredItem(QList<WiredConnection *> &items);

private Q_SLOTS:
    void onConflictStatusChanged(NetworkDeviceBase *device, const bool confilcted);

private:
    NetworkDeviceBase *m_device;
    Connectivity m_connectivity;
    QQueue<DeviceStatus> m_statusQueue;
    DeviceStatus m_deviceStatus;
    IPConfilctChecker *m_ipConflictChecker;
    bool m_ipConflicted;
};

}
}

#endif // INTERFACE_H
