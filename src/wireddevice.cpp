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

bool WiredDevice::connectNetwork(WiredConnection *connection)
{
    if (!connection)
        return false;

    networkInter()->ActivateConnection(connection->connection()->uuid(), QDBusObjectPath(path()));
    return true;
}

bool WiredDevice::connectNetwork(const QString &path)
{
    for (WiredConnection *connection : m_connections) {
        if (connection->connection()->path() == path)
            return connectNetwork(connection);
    }

    return false;
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
            connection->setConnectionStatus(ConnectionStatus::Deactivated);
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
        if (!connPaths.contains(connection->connection()->path()))
            rmConns << connection;
    }

    for (WiredConnection *connection : rmConns)
        m_connections.removeOne(connection);

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

void WiredDevice::updateActiveInfo(const QList<QJsonObject> &info)
{
    bool changeStatus = false;
    // 根据返回的UUID找到对应的连接，找到State=2的连接变成连接成功状态
    for (const QJsonObject &activeInfo : info) {
        QString uuid = activeInfo.value("Uuid").toString();
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
    NetworkDeviceBase::updateActiveInfo(info);
    if (changeStatus)
        Q_EMIT activeConnectionChanged();
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
    return (m_status == ConnectionStatus::Activated);
}

ConnectionStatus WiredConnection::status() const
{
    return m_status;
}

WiredConnection::WiredConnection()
    : ControllItems()
{
}

WiredConnection::~WiredConnection()
{
}

void WiredConnection::setConnectionStatus(const ConnectionStatus &status)
{
    m_status = status;
}
