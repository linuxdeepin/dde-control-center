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

#ifndef WIREDDEVICE_H
#define WIREDDEVICE_H

#include "networkdevicebase.h"

namespace dde {

namespace network {

class WiredConnection;

class WiredDevice : public NetworkDeviceBase
{
    Q_OBJECT

    friend class NetworkController;
    friend class NetworkInterProcesser;
    friend class NetworkManagerProcesser;

private:
    WiredDevice(NetworkDeviceRealize *devcieRealize, QObject *parent);
    ~WiredDevice() override;

Q_SIGNALS:
    void connectionAdded(const QList<WiredConnection *>);                       // 新增连接
    void connectionRemoved(const QList<WiredConnection *>);                     // 删除连接
    void connectionPropertyChanged(const QList<WiredConnection *> &);           // 连接属性发生变化

public:
    bool connectNetwork(WiredConnection *connection);                           // 连接网络，连接成功抛出deviceStatusChanged信号
    bool connectNetwork(const QString &path);                                   // 连接网络重载函数，参数为配置路径
    bool isConnected() const override;                                          // 是否连接网络，重写基类的虚函数
    DeviceType deviceType() const override;                                     // 返回设备类型，适应基类统一的接口
    QList<WiredConnection *> items() const;                                     // 有线网络连接列表
};

class WiredConnection : public ControllItems
{
    friend class WiredDevice;
    friend class WiredDeviceInterRealize;
    friend class DeviceManagerRealize;

public:
    bool connected();                                                   //网络是否连接成功
    ConnectionStatus status() const;                             // 当前连接的连接状态

protected:
    WiredConnection();
    ~WiredConnection();
    void setConnectionStatus(const ConnectionStatus &status);

private:
    ConnectionStatus m_status;
};

}

}

#endif // UWIREDDEVICE_H
