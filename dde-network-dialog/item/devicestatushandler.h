// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DEVICESTATUSHANDLER_H
#define DEVICESTATUSHANDLER_H

#include <QObject>

namespace dde {
  namespace network {
    class WiredDevice;
    class WirelessDevice;
  }
}

using namespace dde::network;

enum class NetDeviceStatus {
    Unknown = 0,
    Enabled,
    Disabled,
    Connected,
    Disconnected,
    Connecting,
    Authenticating,
    ObtainingIP,
    ObtainIpFailed,
    ConnectNoInternet,
    Nocable,
    ConnectFailed
};

enum class PluginState
{
    Unknow = 0,
    Disabled,
    Connected,
    Disconnected,
    Connecting,
    Failed,
    ConnectNoInternet,
    WirelessDisabled,
    WiredDisabled,
    WirelessConnected,
    WiredConnected,
    WirelessDisconnected,
    WiredDisconnected,
    WirelessConnecting,
    WiredConnecting,
    WirelessConnectNoInternet,
    WiredConnectNoInternet,
    WirelessFailed,
    WiredFailed,
    Nocable
};

#define DECLARE_STATIC_CHECKSTATUS(method) static bool method(const NetDeviceStatus &, const NetDeviceStatus &);

class DeviceStatusHandler : public QObject
{
    Q_OBJECT

public:
    // 获取当前所有的设备列表的状态
    static PluginState pluginState();

private:
    explicit DeviceStatusHandler(QObject *parent = Q_NULLPTR);
    ~DeviceStatusHandler();

    static NetDeviceStatus wiredStatus(WiredDevice * device);
    static NetDeviceStatus wiredStatus(const QList<WiredDevice *> &devices);
    static NetDeviceStatus wirelessStatus(WirelessDevice *device);
    static NetDeviceStatus wirelessStatus(const QList<WirelessDevice *> &devices);
    static PluginState plugState(const NetDeviceStatus &wiredStatus, const NetDeviceStatus &wirelessStatus);

private:
    DECLARE_STATIC_CHECKSTATUS(isUnknow)
    DECLARE_STATIC_CHECKSTATUS(isDisabled)
    DECLARE_STATIC_CHECKSTATUS(isWiredDisconnected)
    DECLARE_STATIC_CHECKSTATUS(isWiredDisabled)
    DECLARE_STATIC_CHECKSTATUS(isWiredConnected)
    DECLARE_STATIC_CHECKSTATUS(isWiredConnecting)
    DECLARE_STATIC_CHECKSTATUS(isWiredConnectNoInternet)
    DECLARE_STATIC_CHECKSTATUS(isNocable)
    DECLARE_STATIC_CHECKSTATUS(isWiredFailed)
    DECLARE_STATIC_CHECKSTATUS(isWirelessDisconnected)
    DECLARE_STATIC_CHECKSTATUS(isWirelessDisabled)
    DECLARE_STATIC_CHECKSTATUS(isWirelessConnected)
    DECLARE_STATIC_CHECKSTATUS(isWirelessConnecting)
    DECLARE_STATIC_CHECKSTATUS(isWirelessConnectNoInternet)
    DECLARE_STATIC_CHECKSTATUS(isWirelessFailed)
    DECLARE_STATIC_CHECKSTATUS(isDisconnected)
    DECLARE_STATIC_CHECKSTATUS(isConnected)
    DECLARE_STATIC_CHECKSTATUS(isConnecting)
    DECLARE_STATIC_CHECKSTATUS(isConnectNoInternet)
};

#endif // DEVICESTATUSHANDLER_H
