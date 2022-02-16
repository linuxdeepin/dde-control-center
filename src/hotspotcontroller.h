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

#ifndef HOTSPOTCONTROLLER_H
#define HOTSPOTCONTROLLER_H

#include "networkconst.h"
#include "netutils.h"

#include <QMap>
#include <QObject>

namespace dde {

namespace network {

class Connection;
class HotspotItem;
class WirelessDevice;
class NetworkDeviceBase;

class HotspotController : public QObject
{
    Q_OBJECT

    friend class NetworkInterProcesser;
    friend class NetworkManagerProcesser;

public:
    void setEnabled(WirelessDevice *device, const bool enable);              // 开启还是关闭个人热点
    bool enabled(WirelessDevice *device);                                    // 设备的热点是否可用
    bool supportHotspot();                                                   // 是否支持个人热点
    void connectItem(HotspotItem *item);                                     // 连接到个人热点
    void connectItem(WirelessDevice *device, const QString &uuid);           // 连接到个人热点
    void disconnectItem(WirelessDevice *device);                             // 断开连接
    QList<HotspotItem *> items(WirelessDevice *device);                      // 返回列表
    QList<WirelessDevice *> devices();                                       // 获取支持热点的设备列表

Q_SIGNALS:
    void enabledChanged(const bool &);                                       // 热点是否可用发生了变化
    void itemAdded(const QMap<WirelessDevice *, QList<HotspotItem *>> &);    // 新增连接的信号
    void itemRemoved(const QMap<WirelessDevice *, QList<HotspotItem *>> &);  // 删除连接的信号
    void itemChanged(const QMap<WirelessDevice *, QList<HotspotItem *>> &);  // 连接信息改变的信号
    void activeConnectionChanged(const QList<WirelessDevice *> &);           // 活动连接发生变化
    void deviceAdded(const QList<WirelessDevice *> &);                       // 新增热点设备
    void deviceRemove(const QList<WirelessDevice *> &);                      // 删除热点设备

protected:
    explicit HotspotController(NetworkInter *networkInter, QObject *parent = Q_NULLPTR);
    ~HotspotController();

    void updateDevices(const QList<NetworkDeviceBase *> &devices);
    void updateConnections(const QJsonArray &jsons);
    HotspotItem *findItem(WirelessDevice *device, const QJsonObject &json);

    void updateActiveConnection(const QJsonObject &activeConnections);

    WirelessDevice *findDevice(const QString &path);
    bool isHotspotConnection(const QString &uuid);

private:
    QList<WirelessDevice *> m_devices;
    QList<HotspotItem *> m_hotspotItems;
    NetworkInter *m_networkInter;
};

class HotspotItem : public ControllItems
{
    friend class HotspotController;

public:
    QString name() const;                                                    // 个人热点名称
    WirelessDevice *device() const;                                          // 当前热点对应的无线设备
    ConnectionStatus status() const;                               // 当前连接的连接状态

protected:
    HotspotItem(WirelessDevice *device);
    ~HotspotItem();

    QString devicePath() const;                                              // 返回设备的路径
    void setConnectionStatus(const ConnectionStatus &status);

private:
    WirelessDevice *m_device;
    QString m_devicePath;
    ConnectionStatus m_connectionStatus;
};

}

}

#endif // UHOTSPOTCONTROLLER_H
