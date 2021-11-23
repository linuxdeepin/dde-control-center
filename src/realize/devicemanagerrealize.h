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

#ifndef DEVICEMANAGERREALIZE_H
#define DEVICEMANAGERREALIZE_H

#include "netinterface.h"

#include <networkmanagerqt/device.h>

namespace NetworkManager {
class WirelessNetwork;
}

using namespace NetworkManager;

namespace dde {
namespace network {

class DeviceManagerRealize : public NetworkDeviceRealize
{
    Q_OBJECT

public:
    explicit DeviceManagerRealize(IPConfilctChecker *ipChecker, QSharedPointer<Device> device, QObject *parent = nullptr);
    ~DeviceManagerRealize() override;
    void resetDeviceStatus();

protected:
    bool isEnabled() const override;                                                      // 当前的网卡是否启用
    QString interface() const override;                                                   // 返回设备上的Interface
    QString driver() const override;                                                      // 驱动，对应于备上返回值的Driver
    bool managed() const override;                                                        // 对应于设备上返回值的Managed
    QString vendor() const override;                                                      // 对应于设备上返回值的Vendor
    QString uniqueUuid() const override;                                                  // 网络设备的唯一的UUID，对应于设备上返回值的UniqueUuid
    bool usbDevice() const override;                                                      // 是否是USB设备，对应于设备上返回值的UsbDevice
    QString path() const override;                                                        // 设备路径，对应于设备上返回值的Path
    QString activeAp() const override;                                                    // 对应于设备上返回值的ActiveAp
    bool supportHotspot() const override;                                                 // 是否支持热点,对应于设备上返回值的SupportHotspot
    QString realHwAdr() const override;                                                   // mac地址
    QString usingHwAdr() const override;                                                  // 正在使用的mac地址
    QString ipv4() const override;                                                        // IPV4地址
    QString ipv6() const override;                                                        // IPV6地址
    QJsonObject activeConnectionInfo() const override;                                    // 获取当前活动连接的信息
    void setEnabled(bool enabled) override;                                               // 开启或禁用网卡
    void disconnectNetwork() override;                                                    // 断开网络连接，该方法是一个虚方法，具体在子类

    // 无线网络接口
    QList<AccessPoints *> accessPointItems() const override;                              // 当前网卡上所有的网络列表
    void scanNetwork() override;                                                        // 重新加载所有的无线网络列表
    void connectNetwork(const AccessPoints *item) override;                               // 连接网络，连接成功抛出deviceStatusChanged信号
    QList<WirelessConnection *> wirelessItems() const override;                           // 无线网络连接列表
    AccessPoints *activeAccessPoints() const override;                                    // 当前活动的无线连接

    // 有线网络接口
    bool connectNetwork(WiredConnection *connection) override;                            // 连接网络，连接成功抛出deviceStatusChanged信号
    QList<WiredConnection *> wiredItems() const override;                                 // 有线网络连接列表

private:
    template<class T>
    void clearListData(QList<T *> &dataList) {
        for (T *data : dataList)
            delete data;

        dataList.clear();
    }

private:
    void initSigSlotConnection();
    void createWlans(QList<WirelessConnection *> &allConnections);
    void syncWlanAndConnections(QList<WirelessConnection *> &allConnections);

    AccessPoints *findAccessPoints(const QString &ssid);
    QJsonObject createWlanJson(QSharedPointer<NetworkManager::WirelessNetwork> network);
    QJsonObject createConnectionJson(QSharedPointer<NetworkManager::Connection> networkConnection);
    WirelessConnection *findConnectionByAccessPoint(const AccessPoints *accessPoint, QList<WirelessConnection *> &allConnections);

    WiredConnection *findWiredConnection(const QString &path);
    WiredConnection *findWiredConnectionByUuid(const QString &uuid);
    WirelessConnection *findWirelessConnectionBySsid(const QString &ssid);
    WirelessConnection *findWirelessConnection(const QString &path);
    WirelessConnection *findWirelessConnectionByUuid(const QString &uuid);
    ConnectionStatus convertStatus(Device::State state);
    DeviceStatus convertDeviceStatus(Device::State state);

    void changeWiredStatus(Device::State newstate);
    void changeWirelessStatus(Device::State newstate);
    void changeStatus(Device::State newstate);

    bool getHotspotIsEnabled();
    bool hotspotEnabled() override;

private slots:
    void onWiredConnectionChanged();
    void onWirelessConnectionChanged();
    void onStatusChanged(NetworkManager::Device::State newstate, NetworkManager::Device::State oldstate, NetworkManager::Device::StateChangeReason reason);

private:
    QSharedPointer<Device> m_wDevice;
    QList<AccessPoints *> m_accessPoints;
    QList<WiredConnection *> m_wiredConnections;
    QList<WirelessConnection *> m_wirelessConnections;
    WiredConnection *m_activeWiredConnection;
    WirelessConnection *m_activeWirelessConnection;
    bool m_hotspotEnabled;
    QQueue<DeviceStatus> m_statusQueue;
};

}
}
#endif // DEVICEMANAGERREALIZE_H
