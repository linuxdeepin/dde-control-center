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

#include "wireddevice.h"
#include "networkconst.h"

using namespace dde::network;

WiredDevice::WiredDevice(NetworkInter *networkInter, QObject *parent)
    : NetworkDeviceBase(networkInter, parent)
{
}

WiredDevice::~WiredDevice()
{
}

void WiredDevice::connectNetwork(WiredConnection *connection)
{
    if (!connection)
        return;

    networkInter()->ActivateConnection(connection->connection()->uuid(), QDBusObjectPath(path()));
}

void WiredDevice::connectNetwork(const QString &path)
{
    for (WiredConnection *connection : m_connections) {
        if (connection->connection()->path() == path) {
            connectNetwork(connection);
            break;
        }
    }
}

void WiredDevice::disconnectNetwork()
{
    networkInter()->DisconnectDevice(QDBusObjectPath(path()));
}

bool WiredDevice::isConnected() const
{
    for (WiredConnection *connection : m_connections) {
        if (connection->connected())
            return true;
    }

    return false;
}

DeviceType WiredDevice::deviceType() const
{
    return DeviceType::Wired;
}

WiredConnection *WiredDevice::findConnection(const QString &path)
{
    for (WiredConnection *conn : m_connections) {
        if (conn->connection()->path() == path)
            return conn;
    }

    return Q_NULLPTR;
}

void WiredDevice::setDeviceEnabledStatus(const bool &enabled)
{
    if (!enabled) {
        // 禁用网卡的情况下，先清空连接信息
        for (WiredConnection *connection : m_connections)
            connection->m_connected = false;
    }

    NetworkDeviceBase::setDeviceEnabledStatus(enabled);
}

void WiredDevice::updateConnection(const QJsonArray &info)
{
    QList<WiredConnection *> newWiredConnections;
    QStringList connPaths;
    for (const QJsonValue jsonValue : info) {
        const QJsonObject &jsonObj = jsonValue.toObject();
        QString hwAddress = jsonObj.value("HwAddress").toString();
        if (!hwAddress.isEmpty() && hwAddress != realHwAdr())
            continue;

        QString path = jsonObj.value("Path").toString();
        WiredConnection *conn = findConnection(path);
        if (!conn) {
            conn = new WiredConnection;
            m_connections << conn;
            newWiredConnections << conn;
        }

        conn->setConnection(jsonObj);
        if (!connPaths.contains(path))
            connPaths << path;
    }

    QList<WiredConnection *> rmConns;
    for (WiredConnection *connection : m_connections) {
        if (!connPaths.contains(connection->connection()->path())) {
            rmConns << connection;
            m_connections.removeOne(connection);
        }
    }

    if (newWiredConnections.size() > 0)
        Q_EMIT connectionAdded(newWiredConnections);

    if (rmConns.size() > 0)
        Q_EMIT connectionRemoved(rmConns);

    // 提交改变信号后，删除不在的连接
    for (WiredConnection *connection : rmConns)
        delete connection;
}

WiredConnection *WiredDevice::findWiredConnectionByUuid(const QString &uuid)
{
    for (WiredConnection *connection : m_connections) {
        if (connection->connection()->uuid() == uuid)
            return connection;
    }

    return Q_NULLPTR;
}

void WiredDevice::updateActiveInfo(const QList<QJsonObject> &info)
{
    // 先将所有的连接信息置空
    for (WiredConnection *connection : m_connections)
        connection->m_connected = false;

    // 根据返回的UUID找到对应的连接，找到State=2的连接变成连接成功状态
    for (const QJsonObject &activeInfo : info) {
        int activeStatus = activeInfo.value("State").toInt();
        QString uuid = activeInfo.value("Uuid").toString();
        WiredConnection *connection = findWiredConnectionByUuid(uuid);
        if (connection)
            connection->m_connected = (activeStatus == static_cast<int>(ConnectionStatus::Activated));
    }

    // 调用基类的函数，更改设备的状态，同时向外发送信号
    NetworkDeviceBase::updateActiveInfo(info);
}

QString WiredDevice::deviceKey()
{
    return "wired";
}

/**
 * @brief 无线网络连接信息
 */
bool WiredConnection::connected()
{
    return m_connected;
}

WiredConnection::WiredConnection()
    : ControllItems()
    , m_connected(false)
{
}

WiredConnection::~WiredConnection()
{
}
