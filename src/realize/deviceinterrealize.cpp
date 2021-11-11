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

#include "deviceinterrealize.h"
#include "utils.h"
#include "wireddevice.h"
#include "wirelessdevice.h"

using namespace dde::network;

QString DeviceInterRealize::ipv4() const
{
    if (!isConnected() || !isEnabled() || !m_activeInfoData.contains("Ip4"))
        return QString();

    // 返回IPv4地址
    QJsonObject objIpv4 = m_activeInfoData["Ip4"].toObject();
    return objIpv4.value("Address").toString();
}

QString DeviceInterRealize::ipv6() const
{
    if (!isConnected() || !isEnabled() || !m_activeInfoData.contains("Ip6"))
        return QString();

    // 返回IPv6地址
    QJsonObject objIpv4 = m_activeInfoData["Ip6"].toObject();
    return objIpv4.value("Address").toString();
}

QJsonObject DeviceInterRealize::activeConnectionInfo() const
{
    return m_activeInfoData;
}

void DeviceInterRealize::setEnabled(bool enabled)
{
    m_networkInter->EnableDevice(QDBusObjectPath(path()), enabled);
}

Connectivity DeviceInterRealize::connectivity()
{
    return m_connectivity;
}

DeviceInterRealize::DeviceInterRealize(NetworkInter *networkInter, QObject *parent)
    : NetworkDeviceRealize(parent)
    , m_networkInter(networkInter)
    , m_enabled(true)
    , m_connectivity(Connectivity::Full)
{
}

DeviceInterRealize::~DeviceInterRealize()
{
}

NetworkInter *DeviceInterRealize::networkInter()
{
    return m_networkInter;
}

void DeviceInterRealize::updateDeviceInfo(const QJsonObject &info)
{
    m_data = info;
    DeviceStatus stat = convertDeviceStatus(info.value("State").toInt());

    setDeviceStatus(stat);
}

void DeviceInterRealize::initDeviceInfo()
{
    if (m_networkInter) {
        // 状态发生变化后，获取设备的实时信息
        QDBusPendingReply<bool> netEnabled = m_networkInter->IsDeviceEnabled(QDBusObjectPath(path()));
        m_enabled = netEnabled.value();
    }
}

void DeviceInterRealize::setDeviceEnabledStatus(const bool &enabled)
{
    m_enabled = enabled;
    Q_EMIT enableChanged(enabled);
}

void DeviceInterRealize::updateActiveInfo(const QList<QJsonObject> &info)
{
    // 更新活动连接信息，查找当前的设备的最新的状态
    // 这里无需向外发送connectionChanged()信号，因为连接发生变化后，紧接着会获取IP地址等信息，
    // 获取IP地址信息是一个异步过程，所以它将会最后发送
    for (const QJsonObject &activeInfo : info) {
        int activeStatus = activeInfo.value("State").toInt();
        if (activeStatus == static_cast<int>(ConnectionStatus::Activated)) {
            setDeviceStatus(DeviceStatus::Activated);
            break;
        }
    }
}

void DeviceInterRealize::updateActiveConnectionInfo(const QList<QJsonObject> &infos, bool emitHotspot)
{
    Q_UNUSED(emitHotspot);

    m_activeInfoData = QJsonObject();
    for (const QJsonObject &info : infos) {
        if (info.value("ConnectionType").toString() == deviceKey()) {
            // 如果找到了当前硬件地址的连接信息，则直接使用这个数据
            m_activeInfoData = info;
            break;
        }
    }

    // 获取到完整的IP地址后，向外发送连接改变的信号
    if (!m_activeInfoData.isEmpty())
        Q_EMIT connectionChanged();
}

/**
 * @brief 有线设备类的具体实现
 */

WiredDeviceInterRealize::WiredDeviceInterRealize(NetworkInter *networkInter, QObject *parent)
    : DeviceInterRealize(networkInter, parent)
{
}

WiredDeviceInterRealize::~WiredDeviceInterRealize()
{
}

bool WiredDeviceInterRealize::connectNetwork(WiredConnection *connection)
{
    if (!connection)
        return false;

    networkInter()->ActivateConnection(connection->connection()->uuid(), QDBusObjectPath(path()));
    return true;
}

void WiredDeviceInterRealize::disconnectNetwork()
{
    networkInter()->DisconnectDevice(QDBusObjectPath(path()));
}

bool WiredDeviceInterRealize::isConnected() const
{
    for (WiredConnection *connection : m_connections) {
        if (connection->connected())
            return true;
    }

    return false;
}

QList<WiredConnection *> WiredDeviceInterRealize::wiredItems() const
{
    return m_connections;
}

WiredConnection *WiredDeviceInterRealize::findConnection(const QString &path)
{
    for (WiredConnection *conn : m_connections) {
        if (conn->connection()->path() == path)
            return conn;
    }

    return Q_NULLPTR;
}

void WiredDeviceInterRealize::setDeviceEnabledStatus(const bool &enabled)
{
    if (!enabled) {
        // 禁用网卡的情况下，先清空连接信息
        for (WiredConnection *connection : m_connections)
            connection->setConnectionStatus(ConnectionStatus::Deactivated);
    }

    DeviceInterRealize::setDeviceEnabledStatus(enabled);
}

void WiredDeviceInterRealize::updateConnection(const QJsonArray &info)
{
    QList<WiredConnection *> newWiredConnections;
    QList<WiredConnection *> changedWiredConnections;
    QStringList connPaths;
    for (const QJsonValue &jsonValue : info) {
        const QJsonObject &jsonObj = jsonValue.toObject();
        const QString IfcName = jsonObj.value("IfcName").toString();
        if (!IfcName.isEmpty() && IfcName != interface())
            continue;

        const QString path = jsonObj.value("Path").toString();
        WiredConnection *conn = findConnection(path);
        if (!conn) {
            conn = new WiredConnection;
            m_connections << conn;
            newWiredConnections << conn;
        } else {
            if (conn->connection()->id() != jsonObj.value("Id").toString()
                    || conn->connection()->ssid() != jsonObj.value("Ssid").toString())
                changedWiredConnections << conn;
        }

        conn->setConnection(jsonObj);
        if (!connPaths.contains(path))
            connPaths << path;
    }

    QList<WiredConnection *> rmConns;
    for (WiredConnection *connection : m_connections) {
        if (!connPaths.contains(connection->connection()->path()))
            rmConns << connection;
    }

    for (WiredConnection *connection : rmConns)
        m_connections.removeOne(connection);

    if (changedWiredConnections.size())
        Q_EMIT connectionPropertyChanged(changedWiredConnections);

    if (newWiredConnections.size() > 0)
        Q_EMIT connectionAdded(newWiredConnections);

    if (rmConns.size() > 0)
        Q_EMIT connectionRemoved(rmConns);

    // 提交改变信号后，删除不在的连接
    for (WiredConnection *connection : rmConns)
        delete connection;
}

WiredConnection *WiredDeviceInterRealize::findWiredConnectionByUuid(const QString &uuid)
{
    for (WiredConnection *connection : m_connections) {
        if (connection->connection()->uuid() == uuid)
            return connection;
    }

    return Q_NULLPTR;
}

static ConnectionStatus convertStatus(int status)
{
    if (status == 1)
        return ConnectionStatus::Activating;

    if (status == 2)
        return ConnectionStatus::Activated;

    if (status == 3)
        return ConnectionStatus::Deactivating;

    if (status == 4)
        return ConnectionStatus::Deactivated;

    return ConnectionStatus::Unknown;
}

void WiredDeviceInterRealize::updateActiveInfo(const QList<QJsonObject> &info)
{
    bool changeStatus = false;
    // 根据返回的UUID找到对应的连接，找到State=2的连接变成连接成功状态
    for (const QJsonObject &activeInfo : info) {
        const QString uuid = activeInfo.value("Uuid").toString();
        WiredConnection *connection = findWiredConnectionByUuid(uuid);
        if (!connection)
            continue;

        ConnectionStatus status = convertStatus(activeInfo.value("State").toInt());
        if (connection->status() != status) {
            connection->setConnectionStatus(status);
            changeStatus = true;
        }
    }

    // 调用基类的函数，更改设备的状态，同时向外发送信号
    DeviceInterRealize::updateActiveInfo(info);
    if (changeStatus)
        Q_EMIT activeConnectionChanged();
}

QString WiredDeviceInterRealize::deviceKey()
{
    return "wired";
}

/**
 * @brief WirelessDeviceInterRealize::WirelessDeviceInterRealize
 * @param networkInter
 * @param parent
 */

bool WirelessDeviceInterRealize::isConnected() const
{
    for (AccessPoints *ap : m_accessPoints) {
        if (ap->status() == ConnectionStatus::Activated)
            return true;
    }

    return false;
}

QList<AccessPoints *> WirelessDeviceInterRealize::accessPointItems() const
{
    /* 理论上，如果网卡是禁用状态，这里应该直接返回空列表
     * 但是由于在禁用网卡或者打开网卡的时候，后台的反应比较慢，控制中心无法实时获取，所以这里暂时让其先返回所有的网络列表，
     * 等后台反应慢的问题改好后，再把注释打开
     * if (!isEnabled())
        return QList<UAccessPoints *>();*/
    if (m_hotspotInfo.isEmpty())
        return m_accessPoints;

    return QList<AccessPoints *>();
}

void WirelessDeviceInterRealize::scanNetwork()
{
    networkInter()->RequestWirelessScan();
}

void WirelessDeviceInterRealize::connectNetwork(const AccessPoints *item)
{
    WirelessConnection *wirelessConn = findConnectionByAccessPoint(item);
    if (!wirelessConn)
        return;

    const QString uuid = wirelessConn->connection()->uuid();
    const QString apPath = item->path();
    const QString devPath = path();
    QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(networkInter()->ActivateAccessPoint(uuid, QDBusObjectPath(apPath), QDBusObjectPath(devPath)));

    connect(w, &QDBusPendingCallWatcher::finished, [ = ](QDBusPendingCallWatcher * wSelf) {
        QDBusPendingReply<QDBusObjectPath> reply = *wSelf;

        if (reply.value().path().isEmpty()) {
            // 连接失败
            Q_EMIT connectionFailed(item);
            Q_EMIT deviceStatusChanged(DeviceStatus::Failed);
        } else {
            Q_EMIT deviceStatusChanged(DeviceStatus::Activated);
        }
        w->deleteLater();
    });
}

AccessPoints *WirelessDeviceInterRealize::activeAccessPoints() const
{
    // 如果网卡是关闭的状态下，肯定是没有连接
    if (!isEnabled())
        return Q_NULLPTR;

    for (AccessPoints *ap : m_accessPoints) {
        if (ap->connected())
            return ap;
    }

    return Q_NULLPTR;
}

void WirelessDeviceInterRealize::disconnectNetwork()
{
    networkInter()->DisconnectDevice(QDBusObjectPath(path()));
}

QList<WirelessConnection *> WirelessDeviceInterRealize::items() const
{
    QList<WirelessConnection *> lstItems;
    for (WirelessConnection *item : m_connections) {
        if (item->accessPoints())
            lstItems << item;
    }

    return lstItems;
}

WirelessDeviceInterRealize::WirelessDeviceInterRealize(NetworkInter *networkInter, QObject *parent)
    : DeviceInterRealize(networkInter, parent)
{
}

WirelessDeviceInterRealize::~WirelessDeviceInterRealize()
{
    clearListData(m_accessPoints);
    clearListData(m_connections);
}

WirelessConnection *WirelessDeviceInterRealize::findConnectionByPath(const QString &path)
{
    for (WirelessConnection *conn : m_connections) {
        if (conn->connection()->path() == path)
            return conn;
    }

    return Q_NULLPTR;
}

AccessPoints *WirelessDeviceInterRealize::findAccessPoint(const QString &ssid)
{
    for (AccessPoints *accessPoint : m_accessPoints) {
        if (accessPoint->ssid() == ssid)
            return accessPoint;
    }

    return Q_NULLPTR;
}

WirelessConnection *WirelessDeviceInterRealize::findConnectionByAccessPoint(const AccessPoints *accessPoint)
{
    for (WirelessConnection *connection : m_connections) {
        if (connection->accessPoints() == accessPoint)
            return connection;

        if (connection->connection()->ssid() == accessPoint->ssid())
            return connection;
    }

    return Q_NULLPTR;
}

/**
 * @brief 同步热点和连接的信息
 */
void WirelessDeviceInterRealize::syncConnectionAccessPoints()
{
    if (m_accessPoints.isEmpty()) {
        clearListData(m_connections);
        return;
    }

    QList<WirelessConnection *> connections;
    // 找到每个热点对应的Connection，并将其赋值
    for (AccessPoints *accessPoint : m_accessPoints) {
        WirelessConnection *connection = findConnectionByAccessPoint(accessPoint);
        if (!connection) {
            connection = WirelessConnection::createConnection(accessPoint);
            m_connections << connection;
        }

        connection->m_accessPoints = accessPoint;
        connections << connection;
    }
    updateActiveInfo();
    // 删除列表中没有AccessPoints的Connection，让两边保持数据一致
    QList<WirelessConnection *> rmConns;
    for (WirelessConnection *connection : m_connections) {
        if (!connections.contains(connection))
            rmConns << connection;
    }

    for (WirelessConnection *rmConnection : rmConns) {
        m_connections.removeOne(rmConnection);
        delete rmConnection;
    }
}

void WirelessDeviceInterRealize::updateActiveInfo()
{
    if (m_activeAccessPoints.isEmpty())
        return;

    // 先将所有的连接变成普通状态
    for (AccessPoints *ap : m_accessPoints)
        ap->m_status = ConnectionStatus::Unknown;

    // 遍历活动连接列表，找到对应的wlan，改变其连接状态，State赋值即可
    bool changed = false;
    AccessPoints *activeAp = Q_NULLPTR;
    for (const QJsonObject &aapInfo : m_activeAccessPoints) {
        int connectionStatus = aapInfo.value("State").toInt();
        QString ssid = aapInfo.value("Id").toString();
        AccessPoints *ap = findAccessPoint(ssid);
        if (!ap)
            continue;

        ConnectionStatus status = convertConnectionStatus(connectionStatus);
        if (ap->status() == status)
            continue;

        ap->m_status = status;
        changed = true;
        if (ap->m_status == ConnectionStatus::Activated)
            activeAp = ap;
    }

    if (changed)
        Q_EMIT activeConnectionChanged();

    // 如果发现其中一个连接成功，将这个连接成功的信号移到最上面，然后则向外发送连接成功的信号
    if (activeAp) {
        int pos = m_accessPoints.indexOf(activeAp);
        m_accessPoints.move(pos, 0);
        Q_EMIT connectionSuccess(activeAp);
    }

    // 调用基类的方法触发连接发生变化，同时向外抛出连接变化的信号
    DeviceInterRealize::updateActiveInfo(m_activeAccessPoints);
}

QList<WirelessConnection *> WirelessDeviceInterRealize::wirelessItems() const
{
    return m_connections;
}

void WirelessDeviceInterRealize::updateActiveConnectionInfo(const QList<QJsonObject> &infos, bool emitHotspot)
{
    bool enabledHotspotOld = hotspotIsEnabled();

    m_hotspotInfo = QJsonObject();
    for (const QJsonObject &info : infos) {
        const QString devicePath = info.value("Device").toString();
        const QString connectionType = info.value("ConnectionType").toString();
        if (devicePath == this->path() && connectionType == "wireless-hotspot") {
            m_hotspotInfo = info;
            setDeviceStatus(DeviceStatus::Disconnected);
            break;
        }
    }

    if (emitHotspot) {
        bool enabledHotspot = hotspotIsEnabled();
        if (enabledHotspotOld != enabledHotspot)
            Q_EMIT hotspotEnableChanged(enabledHotspot);
    }

    DeviceInterRealize::updateActiveConnectionInfo(infos, emitHotspot);
}

bool dde::network::WirelessDeviceInterRealize::hotspotIsEnabled()
{
    return !m_hotspotInfo.isEmpty();
}

void WirelessDeviceInterRealize::updateAccesspoint(const QJsonArray &json)
{
    // 先过滤相同的ssid，找出信号强度最大的那个
    QMap<QString, int> ssidMaxStrength;
    QMap<QString, QString> ssidPath;
    for (const QJsonValue &jsonValue : json) {
        const QJsonObject obj = jsonValue.toObject();
        const QString ssid = obj.value("Ssid").toString();
        const int strength = obj.value("Strength").toInt();
        const QString path = obj.value("Path").toString();
        if (ssidMaxStrength.contains(ssid)) {
            const int nOldStrength = ssidMaxStrength.value(ssid);
            if (nOldStrength < strength) {
                // 找到了对应的热点，更新热点的信号强度
                ssidMaxStrength[ssid] = strength;
                ssidPath[ssid] = path;
            }
        } else {
            // 第一次直接插入SSID和信号强度和路径
            ssidMaxStrength[ssid] = strength;
            ssidPath[ssid] = path;
        }
    }

    QList<AccessPoints *> newAp;
    QList<AccessPoints *> changedAp;
    QStringList ssids;
    for (const QJsonValue &jsonValue : json) {
        const QJsonObject accessInfo = jsonValue.toObject();
        const QString ssid = accessInfo.value("Ssid").toString();
        const QString maxSsidPath = ssidPath.value(ssid);
        const QString path = accessInfo.value("Path").toString();
        if (path != maxSsidPath)
            continue;

        // 从网络列表中查找现有的网络
        AccessPoints *accessPoint = findAccessPoint(ssid);
        if (!accessPoint) {
            // 如果没有找到这个网络，就新建一个网络，添加到网络列表
            accessPoint = new AccessPoints(accessInfo, this);
            accessPoint->m_devicePath = this->path();
            m_accessPoints << accessPoint;
            newAp << accessPoint;
        } else {
            int strength = accessInfo.value("Strength").toInt();
            if (accessPoint->strength() != strength)
                changedAp << accessPoint;

            accessPoint->updateAccessPoints(accessInfo);
        }

        if (!ssids.contains(ssid))
            ssids << ssid;
    }

    if (changedAp.size())
        Q_EMIT accessPointInfoChanged(changedAp);

    if (newAp.size() > 0)
        Q_EMIT networkAdded(newAp);

    // 更新网络和连接的关系
    QList<AccessPoints *> rmAccessPoints;
    for (AccessPoints *ap : m_accessPoints) {
        if (!ssids.contains(ap->ssid()))
            rmAccessPoints << ap;
    }

    if (rmAccessPoints.size() > 0) {
        for (AccessPoints *ap : rmAccessPoints)
            m_accessPoints.removeOne(ap);

        Q_EMIT networkRemoved(rmAccessPoints);
    }

    for (AccessPoints *ap : rmAccessPoints)
        ap->deleteLater();

    // 按照信号强度从强到弱进行排序
    qSort(m_accessPoints.begin(), m_accessPoints.end(), [ = ] (AccessPoints * ap1, AccessPoints * ap2) {
        return ap1->strength() > ap2->strength();
    });

    createConnection(m_connectionJson);
    syncConnectionAccessPoints();
}

void WirelessDeviceInterRealize::setDeviceEnabledStatus(const bool &enabled)
{
    if (!enabled) {
        // 禁用网卡的情况下，直接清空原来的连接信息
        m_activeAccessPoints.clear();
        // 向外抛出删除wlan连接的信号,这里暂时不清空AccessPoints列表，防止再打开网卡的时候重复创建
        Q_EMIT networkRemoved(m_accessPoints);
    }

    DeviceInterRealize::setDeviceEnabledStatus(enabled);
}

void WirelessDeviceInterRealize::updateConnection(const QJsonArray &info)
{
    m_connectionJson = info;

    createConnection(info);

    syncConnectionAccessPoints();
}

void WirelessDeviceInterRealize::createConnection(const QJsonArray &info)
{
    QStringList connPaths;
    for (const QJsonValue &jsonValue : info) {
        const QJsonObject &jsonObj = jsonValue.toObject();
        const QString hwAddress = jsonObj.value("HwAddress").toString();
        if (!hwAddress.isEmpty() && hwAddress != realHwAdr())
            continue;

        const QString path = jsonObj.value("Path").toString();
        WirelessConnection *connection = findConnectionByPath(path);
        if (!connection) {
            connection = new WirelessConnection;
            m_connections << connection;
        }

        connection->setConnection(jsonObj);
        if (!connPaths.contains(path))
            connPaths << path;
    }

    QList<WirelessConnection *> rmConns;
    for (WirelessConnection *conn : m_connections) {
        if (!connPaths.contains(conn->connection()->path()))
            rmConns << conn;
    }

    // 提交改变信号后，删除不在的连接
    for (WirelessConnection *conn : rmConns) {
        m_connections.removeOne(conn);
        delete conn;
    }
}

void WirelessDeviceInterRealize::updateActiveInfo(const QList<QJsonObject> &info)
{
    m_activeAccessPoints = info;
    updateActiveInfo();
}

QString WirelessDeviceInterRealize::deviceKey()
{
    return "wireless";
}
