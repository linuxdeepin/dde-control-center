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

#include "wirelessdevice.h"
#include "utils.h"

using namespace dde::network;

bool WirelessDevice::isConnected() const
{
    for (AccessPoints *ap : m_accessPoints) {
        if (ap->connectionStatus() == ConnectionStatus::Activated)
            return true;
    }

    return false;
}

DeviceType WirelessDevice::deviceType() const
{
    return DeviceType::Wireless;
}

QList<AccessPoints *> WirelessDevice::accessPointItems() const
{
    /* 理论上，如果网卡是禁用状态，这里应该直接返回空列表
     * 但是由于在禁用网卡或者打开网卡的时候，后台的反应比较慢，控制中心无法实时获取，所以这里暂时让其先返回所有的网络列表，
     * 等后台反应慢的问题改好后，再把注释打开
     * if (!isEnabled())
        return QList<UAccessPoints *>();*/

    return m_accessPoints;
}

void WirelessDevice::scanNetwork()
{
    networkInter()->RequestWirelessScan();
}

void WirelessDevice::connectNetwork(const AccessPoints *item)
{
    WirelessConnection *wirelessConn = findConnectionByAccessPoint(item);
    if (!wirelessConn)
        return;

    QString uuid = wirelessConn->connection()->uuid();
    QString apPath = item->path();
    QString devPath = path();
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

AccessPoints *WirelessDevice::activeAccessPoints() const
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

void WirelessDevice::connectNetwork(const QString &ssid)
{
    AccessPoints *apConnection = findAccessPoint(ssid);
    if (apConnection)
        connectNetwork(apConnection);
}

void WirelessDevice::disconnectNetwork()
{
    networkInter()->DisconnectDevice(QDBusObjectPath(path()));
}

QList<WirelessConnection *> WirelessDevice::items() const
{
    QList<WirelessConnection *> lstItems;
    for (WirelessConnection *item : m_connections) {
        if (item->accessPoints())
            lstItems << item;
    }

    return lstItems;
}

WirelessDevice::WirelessDevice(NetworkInter *networkInter, QObject *parent)
    : NetworkDeviceBase(networkInter, parent)
{
}

WirelessDevice::~WirelessDevice()
{
    clearListData(m_accessPoints);
    clearListData(m_connections);
}

WirelessConnection *WirelessDevice::findConnectionByPath(const QString &path)
{
    for (WirelessConnection *conn : m_connections) {
        if (conn->connection()->path() == path)
            return conn;
    }

    return Q_NULLPTR;
}

AccessPoints *WirelessDevice::findAccessPoint(const QString &ssid)
{
    for (AccessPoints *accessPoint : m_accessPoints) {
        if (accessPoint->ssid() == ssid)
            return accessPoint;
    }

    return Q_NULLPTR;
}

WirelessConnection *WirelessDevice::findConnectionByAccessPoint(const AccessPoints *accessPoint)
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
void WirelessDevice::syncConnectionAccessPoints()
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

void WirelessDevice::updateActiveInfo()
{
    if (m_activeAccessPoints.isEmpty())
        return;

    // 先将所有的连接变成普通状态
    for (AccessPoints *ap : m_accessPoints)
        ap->m_status = ConnectionStatus::Unknown;

    // 遍历活动连接列表，找到对应的wlan，改变其连接状态，State赋值即可
    AccessPoints *activeAp = Q_NULLPTR;
    for (const QJsonObject &aapInfo : m_activeAccessPoints) {
        int connectionStatus = aapInfo.value("State").toInt();
        QString ssid = aapInfo.value("Id").toString();
        AccessPoints *ap = findAccessPoint(ssid);
        if (!ap)
            continue;

        ap->m_status = convertConnectionStatus(connectionStatus);
        if (ap->m_status == ConnectionStatus::Activated)
            activeAp = ap;
    }

    // 如果发现其中一个连接成功，将这个连接成功的信号移到最上面，然后则向外发送连接成功的信号
    if (activeAp) {
        int pos = m_accessPoints.indexOf(activeAp);
        m_accessPoints.move(pos, 0);
        Q_EMIT connectionSuccess(activeAp);
    }

    // 调用基类的方法触发连接发生变化，同时向外抛出连接变化的信号
    NetworkDeviceBase::updateActiveInfo(m_activeAccessPoints);
}

void WirelessDevice::updateAccesspoint(const QJsonArray &json)
{
    // 先过滤相同的ssid，找出信号强度最大的那个
    QMap<QString, int> ssidMaxStrength;
    QMap<QString, QString> ssidPath;
    for (const QJsonValue &jsonValue : json) {
        QJsonObject obj = jsonValue.toObject();
        QString ssid = obj.value("Ssid").toString();
        int strength = obj.value("Strength").toInt();
        QString path = obj.value("Path").toString();
        if (ssidMaxStrength.contains(ssid)) {
            int nOldStrength = ssidMaxStrength.value(ssid);
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
        QJsonObject accessInfo = jsonValue.toObject();
        QString ssid = accessInfo.value("Ssid").toString();
        QString maxSsidPath = ssidPath.value(ssid);
        QString path = accessInfo.value("Path").toString();
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

    if (rmAccessPoints.size() > 0)
        Q_EMIT networkRemoved(rmAccessPoints);

    for (AccessPoints *ap : rmAccessPoints) {
        m_accessPoints.removeOne(ap);
        ap->deleteLater();
    }

    // 按照信号强度从强到弱进行排序
    qSort(m_accessPoints.begin(), m_accessPoints.end(), [ = ](AccessPoints * ap1, AccessPoints * ap2) {
        return ap1->strength() > ap2->strength();
    });

    syncConnectionAccessPoints();
}

void WirelessDevice::setDeviceEnabledStatus(const bool &enabled)
{
    if (!enabled) {
        // 禁用网卡的情况下，直接清空原来的连接信息
        m_activeAccessPoints.clear();
        // 向外抛出删除wlan连接的信号,这里暂时不清空AccessPoints列表，防止再打开网卡的时候重复创建
        Q_EMIT networkRemoved(m_accessPoints);
    }

    NetworkDeviceBase::setDeviceEnabledStatus(enabled);
}

void WirelessDevice::updateConnection(const QJsonArray &info)
{
    QStringList connPaths;
    for (const QJsonValue jsonValue : info) {
        const QJsonObject &jsonObj = jsonValue.toObject();
        QString hwAddress = jsonObj.value("HwAddress").toString();
        if (!hwAddress.isEmpty() && hwAddress != realHwAdr())
            continue;

        QString path = jsonObj.value("Path").toString();
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

    syncConnectionAccessPoints();
}

void WirelessDevice::updateActiveInfo(const QList<QJsonObject> &info)
{
    m_activeAccessPoints = info;
    updateActiveInfo();
}

QString WirelessDevice::deviceKey()
{
    return "wireless";
}
/**
 * @brief 无线网络连接
 */
AccessPoints::AccessPoints(const QJsonObject &json, QObject *parent)
    : QObject(parent)
    , m_json(json)
    , m_status(ConnectionStatus::Unknown)
{
}

AccessPoints::~AccessPoints()
{
}

QString AccessPoints::ssid() const
{
    return m_json.value("Ssid").toString();
}

int AccessPoints::strength() const
{
    if (m_json.isEmpty())
        return -1;

    return m_json.value("Strength").toInt();
}

bool AccessPoints::secured() const
{
    return m_json.value("Secured").toBool();
}

bool AccessPoints::securedInEap() const
{
    return m_json.value("SecuredInEap").toBool();
}

int AccessPoints::frequency() const
{
    return m_json.value("Frequency").toInt();
}

QString AccessPoints::path() const
{
    return m_json.value("Path").toString();
}

QString AccessPoints::devicePath() const
{
    return m_devicePath;
}

bool AccessPoints::connected() const
{
    return (m_status == ConnectionStatus::Activated);
}

ConnectionStatus AccessPoints::connectionStatus() const
{
    return m_status;
}

void AccessPoints::updateAccessPoints(const QJsonObject &json)
{
    int nOldStrength = strength();
    m_json = json;
    int nStrength = strength();
    if (nOldStrength != -1 && nStrength != nOldStrength)
        Q_EMIT strengthChanged(nStrength);
}

/**
 * @brief 无线连接信息
 */
AccessPoints *WirelessConnection::accessPoints() const
{
    return m_accessPoints;
}

bool WirelessConnection::connected()
{
    return m_accessPoints ? m_accessPoints->connected() : false;
}

WirelessConnection::WirelessConnection()
    : ControllItems()
    , m_accessPoints(Q_NULLPTR)
{
}

WirelessConnection::~WirelessConnection()
{
}

WirelessConnection *WirelessConnection::createConnection(AccessPoints *ap)
{
    WirelessConnection *wirelessConnection = new WirelessConnection;
    QJsonObject json;
    json.insert("Ssid", ap->ssid());
    wirelessConnection->setConnection(json);
    wirelessConnection->m_accessPoints = ap;
    return wirelessConnection;
}
