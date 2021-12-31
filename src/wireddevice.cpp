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
#include "realize/netinterface.h"

using namespace dde::network;

WiredDevice::WiredDevice(NetworkDeviceRealize *devcieRealize, QObject *parent)
    : NetworkDeviceBase(devcieRealize, parent)
{
    connect(devcieRealize, &NetworkDeviceRealize::connectionAdded, this, &WiredDevice::connectionAdded);
    connect(devcieRealize, &NetworkDeviceRealize::connectionRemoved, this, &WiredDevice::connectionRemoved);
    connect(devcieRealize, &NetworkDeviceRealize::connectionPropertyChanged, this, &WiredDevice::connectionPropertyChanged);
    connect(devcieRealize, &NetworkDeviceRealize::activeConnectionChanged, this, &WiredDevice::activeConnectionChanged);
}

WiredDevice::~WiredDevice()
{
}

bool WiredDevice::connectNetwork(WiredConnection *connection)
{
    NetworkDeviceRealize *deviceInterface = deviceRealize();
    return deviceInterface->connectNetwork(connection);
}

bool WiredDevice::connectNetwork(const QString &path)
{
    QList<WiredConnection *> wiredItems = items();
    for (WiredConnection *connection : wiredItems) {
        if (connection->connection()->path() == path)
            return connectNetwork(connection);
    }

    return false;
}

bool WiredDevice::isConnected() const
{
    QList<WiredConnection *> wiredItems = items();
    for (WiredConnection *connection : wiredItems) {
        if (connection->connected())
            return true;
    }

    return false;
}

DeviceType WiredDevice::deviceType() const
{
    return DeviceType::Wired;
}

QList<WiredConnection *> WiredDevice::items() const
{
    return deviceRealize()->wiredItems();
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
    , m_status(ConnectionStatus::Unknown)
{
}

WiredConnection::~WiredConnection()
{
}

void WiredConnection::setConnectionStatus(const ConnectionStatus &status)
{
    m_status = status;
}
