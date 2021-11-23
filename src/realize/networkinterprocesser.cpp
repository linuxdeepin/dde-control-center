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

#include "networkinterprocesser.h"
#include "ipconfilctchecker.h"

#include "dslcontroller.h"
#include "hotspotcontroller.h"
#include "networkdetails.h"
#include "networkdevicebase.h"
#include "proxycontroller.h"
#include "vpncontroller.h"
#include "wireddevice.h"
#include "wirelessdevice.h"
#include "utils.h"
#include "deviceinterrealize.h"

namespace dde {
namespace network {

enum InterfaceFlags {
    NM_DEVICE_INTERFACE_FLAG_NONE     = 0,          // an alias for numeric zero, no flags set.
    NM_DEVICE_INTERFACE_FLAG_UP       = 0x1,        // the interface is enabled from the administrative point of view. Corresponds to kernel IFF_UP.
    NM_DEVICE_INTERFACE_FLAG_LOWER_UP = 0x2,        // the physical link is up. Corresponds to kernel IFF_LOWER_UP.
    NM_DEVICE_INTERFACE_FLAG_CARRIER  = 0x10000,    // the interface has carrier. In most cases this is equal to the value of @NM_DEVICE_INTERFACE_FLAG_LOWER_UP
};

const static QString networkService = "com.deepin.daemon.Network";
const static QString networkPath = "/com/deepin/daemon/Network";

NetworkInterProcesser::NetworkInterProcesser(bool sync, bool ipCheck, QObject *parent)
    : NetworkProcesser(parent)
    , m_proxyController(Q_NULLPTR)
    , m_vpnController(Q_NULLPTR)
    , m_dslController(Q_NULLPTR)
    , m_hotspotController(Q_NULLPTR)
    , m_networkInter(new NetworkInter(networkService, networkPath, QDBusConnection::sessionBus(), this))
    , m_connectivity(Connectivity::Full)
    , m_sync(sync)
    , m_unManagerDevice(Q_NULLPTR)
    , m_newManagerDevice(Q_NULLPTR)
    , m_ipChecker(new IPConfilctChecker(this, ipCheck, m_networkInter, this))
{
    initConnection();
    initDeviceService();
}

NetworkInterProcesser::~NetworkInterProcesser()
{
}

void NetworkInterProcesser::initNetData(NetworkInter *networkInt)
{
    onDevicesChanged(networkInt->devices());
    onConnectionListChanged(networkInt->connections());
    onAccesspointChanged(networkInt->wirelessAccessPoints());
    onActiveConnectionsChanged(networkInt->activeConnections());
}

void NetworkInterProcesser::initDeviceService()
{
    m_networkInter->setSync(m_sync);
    if (m_sync) {
        // 同步的方式来获取设备列表和连接列表
        initNetData(m_networkInter);
    } else {
        // 异步的方式获取设备列表和连接列表
        QDBusReply<bool> req = QDBusConnection::sessionBus().interface()->isServiceRegistered(networkService);
        if (!req.value()) {
            QDBusServiceWatcher *serviceWatcher = new QDBusServiceWatcher(this);
            serviceWatcher->setConnection(QDBusConnection::sessionBus());
            serviceWatcher->addWatchedService(networkService);
            connect(serviceWatcher, &QDBusServiceWatcher::serviceRegistered, [ = ] {
                NetworkInter netInter(networkService, networkPath, QDBusConnection::sessionBus(), this);
                initNetData(&netInter);
                serviceWatcher->deleteLater();
            });
        } else {
            // 在异步状态下，这里必须调用一次相关的接口来获取数据，否则就不会触发相关的信号
            initNetData(m_networkInter);
        }
    }
}

void NetworkInterProcesser::initConnection()
{
    connect(m_networkInter, &NetworkInter::DevicesChanged, this, &NetworkInterProcesser::onDevicesChanged);                            // 设备状态发生变化
    connect(m_networkInter, &NetworkInter::ConnectionsChanged, this, &NetworkInterProcesser::onConnectionListChanged);                 // 连接发生变化
    connect(m_networkInter, &NetworkInter::ActiveConnectionsChanged, this, &NetworkInterProcesser::onActiveConnectionsChanged);        // 当前活动连接发生变化
    connect(m_networkInter, &NetworkInter::WirelessAccessPointsChanged, this, &NetworkInterProcesser::onAccesspointChanged);           // 热点发生变化
    connect(m_networkInter, &NetworkInter::DeviceEnabled, this, &NetworkInterProcesser::onDeviceEnableChanged);                        // 关闭设备或启用设备

    connect(m_networkInter, &NetworkInter::ConnectivityChanged, this, &NetworkInterProcesser::onConnectivityChanged);                  // 网络状态发生变化
    connect(m_ipChecker, &IPConfilctChecker::conflictStatusChanged, this, [ ] (NetworkDeviceBase *device, const bool &confilct) {
        Q_EMIT device->deviceStatusChanged(confilct ? DeviceStatus::IpConfilct : device->deviceStatus());
    });
}

NetworkDeviceBase *NetworkInterProcesser::findDevices(const QString &path) const
{
    // 查找指定路径的设备
    for (NetworkDeviceBase *device : m_devices) {
        if (device->path() == path)
            return device;
    }

    return Q_NULLPTR;
}

static DeviceType deviceType(const QString &type)
{
    if (type == "wireless")
        return DeviceType::Wireless;

    if (type == "wired")
        return DeviceType::Wired;

    return DeviceType::Unknown;
}

void NetworkInterProcesser::onDevicesChanged(const QString &value)
{
    if (value.isEmpty())
        return;

    PRINT_INFO_MESSAGE("device Changed");
    static bool isFirstEnter = false;
    const QJsonObject data = QJsonDocument::fromJson(value.toUtf8()).object();
    QStringList devPaths;
    QMap<NetworkDeviceBase *, QJsonObject> devInfoMap;
    QList<NetworkDeviceBase *> newDevices;
    QStringList keys = data.keys();
    for (int i = 0; i < keys.size(); i++) {
        QString key = keys[i];
        DeviceType type = deviceType(key);
        if (type == DeviceType::Unknown)
            continue;

        QJsonArray deviceList = data.value(key).toArray();
        for (const QJsonValue jsonValue : deviceList) {
            const QJsonObject deviceInfo = jsonValue.toObject();
            if (!deviceInfo.value("Managed").toBool()) {
                const QString path = deviceInfo.value("Path").toString();
                NetworkDeviceBase *device = findDevices(path);
                if (device && device->deviceType() == DeviceType::Wireless && device->supportHotspot()) {
                    // 上面判断的条件是当前设备为无线设备,并且当前设备支持热点,则需要先判断一下当前设备是不是
                    // 如果当前unManager的设备是无线设备，并且只有一个UnManager的设备(!m_unManagerDevice)，则认为当前的设备变化信号是通过关闭热点出发的
                    // 如果发现上一个无线设备存在(m_unManagerDevice不为空)，则认为当前设备不是通过关闭热点引起的
                    DeviceInterRealize *deviceInter = static_cast<DeviceInterRealize *>(device->deviceRealize());
                    deviceInter->m_data = deviceInfo;
                    if (!m_unManagerDevice) {
                        m_unManagerDevice = device;
                        isFirstEnter = true;
                    } else {
                        m_unManagerDevice = Q_NULLPTR;
                    }
                }
                PRINT_DEBUG_MESSAGE(QString("unManager device: %1").arg(deviceInfo.value("Interface").toString()));
                continue;
            }

            // 根据标志位InterfaceFlags判断网络连接是否有效
            if (type != DeviceType::Wireless) {
                if (!deviceInfo.value("InterfaceFlags").isUndefined()) {
                    if (!(deviceInfo.value("InterfaceFlags").toInt() & NM_DEVICE_INTERFACE_FLAG_UP)) {
                        PRINT_DEBUG_MESSAGE(QString("Interface_flag_up: %1").arg(deviceInfo.value("Interface").toString()));
                        continue;
                    }
                }
            }

            const QString path = deviceInfo.value("Path").toString();
            NetworkDeviceBase *device = findDevices(path);
            if (!device) {
                switch (type) {
                case DeviceType::Wireless: {
                    PRINT_DEBUG_MESSAGE(QString("new Wireless Device: %1").arg(deviceInfo.value("Interface").toString()));
                    DeviceInterRealize *wirelessRealize = new WirelessDeviceInterRealize(m_ipChecker, m_networkInter, nullptr);
                    device = new WirelessDevice(wirelessRealize, this);
                    if (!m_newManagerDevice)
                        m_newManagerDevice = device;
                    else
                        m_newManagerDevice = Q_NULLPTR;
                    break;
                }
                case DeviceType::Wired: {
                    PRINT_DEBUG_MESSAGE(QString("new Wired Device: %1").arg(deviceInfo.value("Interface").toString()));
                    DeviceInterRealize *wiredRealize = new WiredDeviceInterRealize(m_ipChecker, m_networkInter, nullptr);
                    device = new WiredDevice(wiredRealize, this);
                    break;
                }
                default:
                    break;
                }
                if (device) {
                    newDevices << device;
                    m_devices << device;
                }
            }
            if (device) {
                devInfoMap[device] = deviceInfo;
                // 将所有的设备路径添加到缓存中
                if (!devPaths.contains(device->path()))
                    devPaths << device->path();
            }
        }
    }
    // 对设备进行排序，有线网络始终排在无线网络前面
    qSort(m_devices.begin(), m_devices.end(), [ = ] (NetworkDeviceBase *dev1, NetworkDeviceBase *dev2) {
        if (dev1->deviceType() == DeviceType::Wired && dev2->deviceType() == DeviceType::Wireless)
            return true;

        if (dev1->deviceType() == DeviceType::Wireless && dev2->deviceType() == DeviceType::Wired)
            return false;

        return devPaths.indexOf(dev1->path()) < devPaths.indexOf(dev2->path());
    });
    // 更新设备的连接信息，因为读取设备状态信息的时候，需要这个连接信息
    updateDeviceConnectiveInfo();
    // 删除已经移除的设备
    QList<NetworkDeviceBase *> rmDevices;
    for (NetworkDeviceBase *device : m_devices) {
        if (!devPaths.contains(device->path()))
            rmDevices << device;
    }

    for (NetworkDeviceBase *device : rmDevices)
        m_devices.removeOne(device);

    for (NetworkDeviceBase *device : m_devices) {
        if (devInfoMap.contains(device)) {
            QJsonObject info = devInfoMap.value(device);
            DeviceInterRealize *deviceInter = static_cast<DeviceInterRealize *>(device->deviceRealize());
            deviceInter->updateDeviceInfo(info);
        }
    }

    auto sendDeviceChangeSignal = [ = ] {
        if (newDevices.size() > 0 || rmDevices.size() > 0) {
            // 更新设备名称
            updateDeviceName();

            // 告诉外面新增的设备列表
            if (newDevices.size() > 0) {
                // 初始化设备的数据，包括设备是否可用等信息
                bool wirelessExist = false;
                for (NetworkDeviceBase *device : newDevices) {
                    DeviceInterRealize *deviceInter = static_cast<DeviceInterRealize *>(device->deviceRealize());
                    deviceInter->initDeviceInfo();
                    if (device->deviceType() == DeviceType::Wireless)
                        wirelessExist = true;
                }

                // 更新设备的时候，同时需要更新连接信息，因为可能连接的信号发生在设备更新信息之前
                updateConnectionsInfo(newDevices);
                // 如果新增的设备中存在无线网卡，则同时需要更新wlan的信息，因为存在如下情况
                // 如果关闭热点的时候，会先移除设备，然后再新增设备，此时如果不更新wlan，这种情况下，新增的那个无线设备的wlan就会为空
                if (wirelessExist)
                    onAccesspointChanged(m_networkInter->wirelessAccessPoints());
            }
            // 设备列表发生变化的同时，需要同时更新网络连接状态
            onActiveConnectionsChanged(m_networkInter->activeConnections());

            // 设备列表发生变化的同时，需要同时更新DSL的相关的信息，因为DSL里面用到了设备的信息，需要获取设备路径等
            updateDSLData();

            // 设备列表发生变化后，同时也需要更新设备热点的信息
            updateDeviceHotpot();
            // 更新热点的活动连接信息
            updateDeviceActiveHotpot();

            // 发送删除的设备列表信号，此时这些设备对象还未析构，外面调用来处理响应的操作，统一在一个线程中处理
            if (rmDevices.size() > 0) {
                for (NetworkDeviceBase *device : rmDevices)
                    Q_EMIT device->removed();

                Q_EMIT deviceRemoved(rmDevices);
                // 在移除设备后，需要立刻将网络详情中对应的项移除
                updateNetworkDetails();
            }

            // 需要将新增设备的信号放到更新设备数据之后，因为外部接收到新增设备信号的时候，需要更新设备信息，如果放到更新设备数据前面，则里面的数据不是最新的数据
            if (newDevices.size() > 0)
                Q_EMIT deviceAdded(newDevices);

            // 一定要将删除设备放到最后，因为在发出信号后，外面可能还会用到
            for (NetworkDeviceBase *device : rmDevices)
                delete device;
        }
    };

    if (m_unManagerDevice) {
        // 当前认为是通过关闭热点的设备，并且新增设备为空，则等待下一次newDevice的出现
        // 此处这么做,是为了防止关闭热点的时候引起当前设备先unManager然后manager
        if (isFirstEnter) {
            QTimer::singleShot(1000, this, [ = ] {
                // 等待1秒过后,查看新设备是否存在s
                if (m_newManagerDevice) {
                    // 如果新设备存在,并且和上次unManager的设备的path相同,则认为他们是同一个设备,如果是同一个设备,则无需发送设备变化信号,
                    // 如果是不同的设备,或者1秒过后没有新增设备,则直接发送设备变化信号
                    if (m_unManagerDevice->path() != m_newManagerDevice->path()) {
                        // 如果上次移除的设备和本次新增的设备为同一个设备，则不做任何的操作，否则，需要向外发送设备变化信号
                        sendDeviceChangeSignal();
                    }
                } else {
                    sendDeviceChangeSignal();
                }
                m_unManagerDevice = Q_NULLPTR;
                m_newManagerDevice = Q_NULLPTR;
            });
            isFirstEnter = false;
        }
    } else {
        sendDeviceChangeSignal();
        m_newManagerDevice = Q_NULLPTR;
    }
}

void NetworkInterProcesser::onConnectionListChanged(const QString &connections)
{
    if (connections.isEmpty())
        return;

    PRINT_INFO_MESSAGE("start");

    m_connections = QJsonDocument::fromJson(connections.toUtf8()).object();
    updateConnectionsInfo(m_devices);

    // 更新VPN的数据
    if (m_vpnController && m_connections.contains("vpn"))
        m_vpnController->updateVPNItems(m_connections.value("vpn").toArray());

    // 更新DSL的数据
    updateDSLData();

    // 更新热点的数据
    updateDeviceHotpot();

    // 更新热点活动连接
    updateDeviceActiveHotpot();

    // 向外抛出信号告知连接发生了变化
    Q_EMIT connectionChanged();
}

void NetworkInterProcesser::onActiveConnectionsChanged(const QString &activeConnections)
{
    PRINT_INFO_MESSAGE("Active Connections Changed");
    m_connectivity = connectivityValue(m_networkInter->connectivity());
    activeInfoChanged(activeConnections);
    updateDeviceConnectiveInfo();
    asyncActiveConnectionInfo();
}

void NetworkInterProcesser::updateDeviceConnectiveInfo()
{
    for (NetworkDeviceBase *device : m_devices)
        device->deviceRealize()->m_connectivity = m_connectivity;
}

void NetworkInterProcesser::activeConnInfoChanged(const QString &conns)
{
    QJsonParseError error;
    m_activeConnectionInfo = QJsonDocument::fromJson(conns.toUtf8(), &error).array();
    if (error.error == QJsonParseError::NoError) {
        QMap<NetworkDeviceBase *, QJsonObject> deviceInfoMap;
        for (QJsonValue jsonValue : m_activeConnectionInfo) {
            QJsonObject connInfo = jsonValue.toObject();
            const QString devPath = connInfo.value("Device").toString();
            NetworkDeviceBase *device = findDevices(devPath);
            if (!device)
                continue;

            deviceInfoMap.insertMulti(device, connInfo);
        }

        for (auto it = deviceInfoMap.begin(); it != deviceInfoMap.end(); it++) {
            NetworkDeviceBase *device = it.key();
            QList<QJsonObject> json = deviceInfoMap.values(device);
            DeviceInterRealize *deviceInter = static_cast<DeviceInterRealize *>(device->deviceRealize());
            deviceInter->updateActiveConnectionInfo(json, !m_hotspotController);
        }
    } else {
        PRINT_DEBUG_MESSAGE(QString("receive error connection value: %1").arg(conns));
    }
    // 要更新活动热点信息，是因为如果启用或者禁用热点的话，这个活动连接信息会有变化，通过这里来触发热点启用或禁用的信号
    updateDeviceActiveHotpot();
    // 同时需要更新网络信息
    updateNetworkDetails();

    Q_EMIT activeConnectionChange();
}

void NetworkInterProcesser::onAccesspointChanged(const QString &accessPoints)
{
    if (accessPoints.isEmpty())
        return;

    const QJsonObject json = QJsonDocument::fromJson(accessPoints.toUtf8()).object();

    for (NetworkDeviceBase *device : m_devices) {
        // 只有无线网卡才会有wlan连接信息
        if (device->deviceType() == DeviceType::Wireless) {
            WirelessDevice *d = static_cast<WirelessDevice *>(device);
            if (json.contains(d->path())) {
                QJsonArray array = json.value(d->path()).toArray();

                WirelessDeviceInterRealize *deviceRealize = qobject_cast<WirelessDeviceInterRealize *>(d->deviceRealize());
                if (deviceRealize)
                    deviceRealize->updateAccesspoint(array);
            }
        }
    }
}

void NetworkInterProcesser::onDeviceEnableChanged(const QString &devicePath, bool enabled)
{
    PRINT_INFO_MESSAGE(QString("Device enabled:%1").arg(enabled));
    NetworkDeviceBase *device = findDevices(devicePath);
    if (device) {
        DeviceInterRealize *interDevice = static_cast<DeviceInterRealize *>(device->deviceRealize());
        interDevice->setDeviceEnabledStatus(enabled);
        // 如果改设备是无线设备，且支持热点，则更新热点的信息
        if (!m_hotspotController)
            return;
        // 如果当前设备是无线设备切当前设备支持热点，则更新当前设备的热点信息
        if (device->deviceType() == DeviceType::Wireless && device->supportHotspot())
            updateDeviceHotpot();
    }
}

void NetworkInterProcesser::onConnectivityChanged(uint conectivity)
{
    PRINT_INFO_MESSAGE(QString("conectivity:%1").arg(conectivity));
    Connectivity conn = static_cast<Connectivity>(conectivity);
    if (conn == m_connectivity)
        return;

    m_connectivity = conn;
    updateDeviceConnectiveInfo();
    Q_EMIT connectivityChanged(m_connectivity);
}

void NetworkInterProcesser::updateConnectionsInfo(const QList<NetworkDeviceBase *> &devices)
{
    if (devices.isEmpty() || m_connections.isEmpty())
        return;

    static QList<QPair<QString, DeviceType>> devConnInfo
        = {{ "wired", DeviceType::Wired }, { "wireless", DeviceType::Wireless }};

    for (const QPair<QString, DeviceType> &connInfo : devConnInfo) {
        if (m_connections.contains(connInfo.first)) {
            const QJsonArray &connlist = m_connections.value(connInfo.first).toArray();
            for (NetworkDeviceBase *device : devices) {
                if (device->deviceType() == connInfo.second) {
                    DeviceInterRealize *deviceInter = static_cast<DeviceInterRealize *>(device->deviceRealize());
                    deviceInter->updateConnection(connlist);
                }
            }
        }
    }
}

void NetworkInterProcesser::asyncActiveConnectionInfo()
{
    PRINT_INFO_MESSAGE("start");
    QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(m_networkInter->GetActiveConnectionInfo(), this);
    connect(w, &QDBusPendingCallWatcher::finished, w, &QDBusPendingCallWatcher::deleteLater);
    connect(w, &QDBusPendingCallWatcher::finished, [ = ](QDBusPendingCallWatcher * w) {
        QDBusPendingReply<QString> reply = *w;
        QString activeConnectionInfo = reply.value();
        PRINT_INFO_MESSAGE("receive value");
        activeConnInfoChanged(activeConnectionInfo);
    });
}

void NetworkInterProcesser::activeInfoChanged(const QString &conns)
{
    PRINT_INFO_MESSAGE("active info changed");
    // 当前活动连接发生变化
    m_activeConection = QJsonDocument::fromJson(conns.toUtf8()).object();
    QMap<QString, QList<QJsonObject>> devActiveConn;
    for (const QJsonValue info : m_activeConection) {
        const QJsonObject connInfo = info.toObject();
        const QJsonArray devArray = connInfo.value("Devices").toArray();
        for (const QJsonValue &value : devArray) {
            const QString devPath = value.toString();
            devActiveConn[devPath] << connInfo;
        }
    }

    // 更新DSL的活动连接信息
    if (m_dslController)
        m_dslController->updateActiveConnections(m_activeConection);

    // 更新设备的活动连接信息
    for (auto it = devActiveConn.begin(); it != devActiveConn.end(); it++) {
        const QString devPath = it.key();
        NetworkDeviceBase *device = findDevices(devPath);
        if (!device)
            continue;

        const QList<QJsonObject> devInfos = it.value();
        DeviceInterRealize *deviceInt = static_cast<DeviceInterRealize *>(device->deviceRealize());
        deviceInt->updateActiveInfo(devInfos);
    }

    // 更新热点的活动连接信息
    if (m_hotspotController)
        m_hotspotController->updateActiveConnection(m_activeConection);

    // 更新VPN的活动连接信息
    if (m_vpnController)
        m_vpnController->updateActiveConnection(m_activeConection);
}

Connectivity NetworkInterProcesser::connectivity()
{
    return m_connectivity;
}

QList<NetworkDeviceBase *> NetworkInterProcesser::devices()
{
    return m_devices;
}

QList<NetworkDetails *> NetworkInterProcesser::networkDetails()
{
    return m_networkDetails;
}

void NetworkInterProcesser::updateDeviceHotpot()
{
    PRINT_INFO_MESSAGE("start");
    if (!m_hotspotController)
        return;

    m_hotspotController->updateDevices(m_devices);

    if (!m_connections.contains("wireless-hotspot"))
        return;

    const QJsonArray hotspots = m_connections.value("wireless-hotspot").toArray();
    // 更新连接的信息
    m_hotspotController->updateConnections(hotspots);
}

void NetworkInterProcesser::updateDeviceActiveHotpot()
{
    if (!m_hotspotController)
        return;

    QList<QJsonObject> conns;
    for (const QJsonValue jsonValue : m_activeConnectionInfo) {
        const QJsonObject json = jsonValue.toObject();
        const QString connType = json.value("ConnectionType").toString();
        if (connType == "wireless-hotspot")
            conns << json;
    }

    // 更新活动连接信息
    PRINT_INFO_MESSAGE("update active hotspot");
    m_hotspotController->updateActiveConnectionInfo(conns);
}

void NetworkInterProcesser::updateNetworkDetails()
{
    PRINT_INFO_MESSAGE("start");
    QStringList devicePaths;
    for (NetworkDeviceBase *device : m_devices) {
        if (!device->isEnabled())
            continue;

        devicePaths << device->path();
    }

    // 删除不在设备列表中的项
    for (NetworkDetails *detail : m_networkDetails) {
        if (!devicePaths.contains(detail->devicePath()))
            m_networkDetails.removeOne(detail);
    }
    // 删除多余的网络详情的数据
    if (m_networkDetails.size() >= m_activeConnectionInfo.size()) {
        for (int i = m_networkDetails.size() - 1; i >= m_activeConnectionInfo.size(); i--) {
            NetworkDetails *detail = m_networkDetails[i];
            m_networkDetails.removeOne(detail);
            delete detail;
        }
    }

    // 遍历网络详情列表，更新内存中的记录
    for (int i = 0; i < m_activeConnectionInfo.size(); i++) {
        QJsonObject activeConnection = m_activeConnectionInfo.at(i).toObject();
        QString devicePath = activeConnection.value("Device").toString();
        if (!devicePaths.contains(devicePath))
            continue;

        NetworkDetails *detail = Q_NULLPTR;
        if (i < m_networkDetails.size()) {
            detail = m_networkDetails[i];
        } else {
            detail = new NetworkDetails(this);
            m_networkDetails << detail;
        }

        detail->updateData(activeConnection);
    }

    if (m_devices.size() > 0) {
        qSort(m_networkDetails.begin(), m_networkDetails.end(), [ & ] (NetworkDetails *detail1, NetworkDetails *detail2) {
            int index1 = -1;
            int index2 = -1;
            for (int i = 0; i < m_devices.size(); i++) {
                NetworkDeviceBase *device = m_devices[i];
                if (device->path() == detail1->devicePath())
                    index1 = i;
                else if (device->path() == detail2->devicePath())
                    index2 = i;
            }

            return index1 < index2;
        });
    }
}

void NetworkInterProcesser::updateDSLData()
{
    if (!m_dslController || !m_connections.contains("pppoe"))
        return;

    PRINT_INFO_MESSAGE("start update dsl info");
    m_dslController->updateDevice(m_devices);
    m_dslController->updateDSLItems(m_connections.value("pppoe").toArray());
    m_dslController->updateActiveConnections(m_activeConection);
}

ProxyController *NetworkInterProcesser::proxyController()
{
    // 因为有的地方不会调用这个接口，所以该对象延迟创建
    if (!m_proxyController)
        m_proxyController = new ProxyController(m_networkInter, this);

    return m_proxyController;
}

VPNController *NetworkInterProcesser::vpnController()
{
    // 延迟创建
    if (!m_vpnController) {
        m_vpnController = new VPNController(m_networkInter, this);
        if (m_connections.contains("vpn"))
            m_vpnController->updateVPNItems(m_connections.value("vpn").toArray());
        m_vpnController->updateActiveConnection(m_activeConection);
    }

    return  m_vpnController;
}

DSLController *NetworkInterProcesser::dslController()
{
    // 延迟创建
    if (!m_dslController) {
        m_dslController = new DSLController(m_networkInter, this);
        updateDSLData();
    }

    return m_dslController;
}

HotspotController *NetworkInterProcesser::hotspotController()
{
    // 延迟创建
    if (!m_hotspotController) {
        m_hotspotController = new HotspotController(m_networkInter, this);
        updateDeviceHotpot();
        updateDeviceActiveHotpot();
        m_hotspotController->updateActiveConnection(m_activeConection);
    }

    return m_hotspotController;
}

}
}
