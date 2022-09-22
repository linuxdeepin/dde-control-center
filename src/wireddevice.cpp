// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
