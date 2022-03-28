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

#include "devicemanagerrealize.h"
#include "netutils.h"
#include "networkdevicebase.h"

#include <networkmanagerqt/wirelessdevice.h>
#include <networkmanagerqt/wireddevice.h>
#include <networkmanagerqt/manager.h>
#include <networkmanagerqt/wiredsetting.h>

#include <wireddevice.h>
#include <wirelessdevice.h>

#include <QMetaEnum>

using namespace dde::network;
using namespace NetworkManager;

DeviceManagerRealize::DeviceManagerRealize(IPConfilctChecker *ipChecker, QSharedPointer<Device> device, QObject *parent)
    : NetworkDeviceRealize(ipChecker, parent)
    , m_wDevice(device)
    , m_activeWiredConnection(Q_NULLPTR)
    , m_activeWirelessConnection(Q_NULLPTR)
{
    // 初始化有线连接
    onWiredConnectionChanged();
    // 初始化无线连接
    onWirelessConnectionChanged();
    // 初始化信号槽的连接
    initSigSlotConnection();
    // 改变状态
    changeStatus(m_wDevice->state());
    // 获取是否开启热点
    m_hotspotEnabled = getHotspotIsEnabled();
}

void DeviceManagerRealize::initSigSlotConnection()
{
    connect(m_wDevice.get(), &NetworkManager::Device::availableConnectionChanged, this, &DeviceManagerRealize::onWiredConnectionChanged);
    connect(m_wDevice.get(), &NetworkManager::Device::stateChanged, this, &DeviceManagerRealize::onStatusChanged);
    if (m_wDevice->type() == NetworkManager::Device::Wifi) {
        NetworkManager::WirelessDevice::Ptr wirelessDevice = m_wDevice.staticCast<NetworkManager::WirelessDevice>();
        if (!wirelessDevice.isNull()) {
            connect(wirelessDevice.get(), &NetworkManager::WirelessDevice::accessPointAppeared, this, &DeviceManagerRealize::onWirelessConnectionChanged);
            connect(wirelessDevice.get(), &NetworkManager::WirelessDevice::accessPointDisappeared, this, &DeviceManagerRealize::onWirelessConnectionChanged);
            connect(wirelessDevice.get(), &NetworkManager::WirelessDevice::modeChanged, this, [ this ] {
                bool oldEnabled = m_hotspotEnabled;
                m_hotspotEnabled = getHotspotIsEnabled();
                PRINT_DEBUG_MESSAGE(QString("wirelessModel changed, old hotspotEnabled: %1, new hotspotEnabled: %2").arg(oldEnabled).arg(m_hotspotEnabled));
                if (m_hotspotEnabled)
                    setDeviceStatus(DeviceStatus::Disconnected);
                if (oldEnabled != m_hotspotEnabled)
                    Q_EMIT hotspotEnableChanged(m_hotspotEnabled);
            });
        }
    }
}

DeviceManagerRealize::~DeviceManagerRealize()
{
}

bool DeviceManagerRealize::isEnabled() const
{
    QDBusInterface dbusInter("com.deepin.system.Network", "/com/deepin/system/Network", "com.deepin.system.Network", QDBusConnection::systemBus());
    QDBusPendingCall reply = dbusInter.asyncCall("IsDeviceEnabled", m_wDevice->uni());
    reply.waitForFinished();
    QDBusPendingReply<bool> replyResult = reply.reply();
    return replyResult.argumentAt(0).toBool();
}

QString DeviceManagerRealize::interface() const
{
    return m_wDevice->interfaceName();
}

QString DeviceManagerRealize::driver() const
{
    return m_wDevice->driver();
}

bool DeviceManagerRealize::managed() const
{
    return m_wDevice->managed();
}

QString DeviceManagerRealize::vendor() const
{
    return QString();
}

QString DeviceManagerRealize::uniqueUuid() const
{
    return m_wDevice->udi();
}

bool DeviceManagerRealize::usbDevice() const
{
    return false;
}

QString DeviceManagerRealize::path() const
{
    return m_wDevice->uni();
}

QString DeviceManagerRealize::activeAp() const
{
    if (m_wDevice->type() == NetworkManager::Device::Wifi) {
        NetworkManager::WirelessDevice::Ptr wirelessDevice = m_wDevice.staticCast<NetworkManager::WirelessDevice>();
        AccessPoint::Ptr ap = wirelessDevice->activeAccessPoint();
        if (ap)
            return ap->ssid();
    }

    return QString();
}

bool DeviceManagerRealize::supportHotspot() const
{
    if (m_wDevice->type() == NetworkManager::Device::Wifi) {
        NetworkManager::WirelessDevice::Ptr wirelessDevice = m_wDevice.staticCast<NetworkManager::WirelessDevice>();
        return (wirelessDevice->wirelessCapabilities().testFlag(NetworkManager::WirelessDevice::ApCap));
    }

    return false;
}

QString DeviceManagerRealize::realHwAdr() const
{
    if (m_wDevice->type() == NetworkManager::Device::Ethernet) {
        QSharedPointer<NetworkManager::WiredDevice> device = m_wDevice.staticCast<NetworkManager::WiredDevice>();
        return device->hardwareAddress();
    }

    if (m_wDevice->type() == NetworkManager::Device::Wifi) {
        QSharedPointer<NetworkManager::WirelessDevice> device = m_wDevice.staticCast<NetworkManager::WirelessDevice>();
        return device->hardwareAddress();
    }

    return QString();
}

QString DeviceManagerRealize::usingHwAdr() const
{
    return QString();
}

const QStringList DeviceManagerRealize::ipv4()
{
    if (!device() || !device()->isConnected() || !isEnabled())
        return QStringList();

    NetworkManager::IpAddresses ipv4AddrList = m_wDevice->ipV4Config().addresses();
    QStringList ipv4s;
    for (const IpAddress &ipAddr : ipv4AddrList)
        ipv4s << ipAddr.ip().toString();

    return ipv4s;
}

const QStringList DeviceManagerRealize::ipv6()
{
    if (!device() || !device()->isConnected() || !isEnabled())
        return QStringList();

    NetworkManager::IpAddresses ipv6AddrList = m_wDevice->ipV6Config().addresses();
    QStringList ipv6s;
    for (const IpAddress &ipAddr : ipv6AddrList)
        ipv6s << ipAddr.ip().toString();

    return ipv6s;
}

QJsonObject DeviceManagerRealize::activeConnectionInfo() const
{
    return QJsonObject();
}

void DeviceManagerRealize::setEnabled(bool enabled)
{
    bool currentEnabled = isEnabled();
    if (currentEnabled != enabled) {
        qInfo() << "set Device " << m_wDevice->uni() << " enabled:" << (enabled ? "true" : "false");
        QDBusInterface dbusInter("com.deepin.system.Network", "/com/deepin/system/Network", "com.deepin.system.Network", QDBusConnection::systemBus());
        QDBusReply<QDBusObjectPath> reply = dbusInter.call("EnableDevice", m_wDevice->uni(), enabled);
        if (enabled) {
            // 如果是开启，则让其自动连接
            QString activeConnectionPath = reply.value().path();
            NetworkManager::activateConnection(activeConnectionPath, m_wDevice->uni(), QString());
            qInfo() << "connected:" << activeConnectionPath;
        }
    }
}

void DeviceManagerRealize::disconnectNetwork()
{
    NetworkManager::ActiveConnection::Ptr activeConnection = m_wDevice->activeConnection();
    if (!activeConnection.isNull())
        NetworkManager::deactivateConnection(activeConnection->path());
}

QList<AccessPoints *> DeviceManagerRealize::accessPointItems() const
{
    if (m_wDevice->type() == NetworkManager::Device::Wifi)
        return m_accessPoints;

    return QList<AccessPoints *>();
}

void DeviceManagerRealize::scanNetwork()
{
    if (m_wDevice->type() != NetworkManager::Device::Wifi)
        return;

    NetworkManager::WirelessDevice::Ptr device = m_wDevice.staticCast<NetworkManager::WirelessDevice>();
    device->requestScan();
}

void DeviceManagerRealize::connectNetwork(const AccessPoints *item)
{
    if (m_wDevice->type() != NetworkManager::Device::Wifi)
        return;

    WirelessConnection *conn = findConnectionByAccessPoint(item, m_wirelessConnections);
    if (conn)
        NetworkManager::activateConnection(conn->connection()->path(), m_wDevice->uni(), "");
}

QList<WirelessConnection *> DeviceManagerRealize::wirelessItems() const
{
    return m_wirelessConnections;
}

AccessPoints *DeviceManagerRealize::activeAccessPoints() const
{
    if (m_wDevice->type() != NetworkManager::Device::Wifi)
        return nullptr;

    QSharedPointer<NetworkManager::WirelessDevice> device = m_wDevice.staticCast<NetworkManager::WirelessDevice>();
    AccessPoint::Ptr currentActiveAp = device->activeAccessPoint();
    if (!currentActiveAp.isNull()) {
        for (AccessPoints *ap : m_accessPoints) {
            if (ap->ssid() == currentActiveAp->ssid())
                return ap;
        }
    }

    return nullptr;
}

bool DeviceManagerRealize::connectNetwork(WiredConnection *connection)
{
    if (m_wDevice->type() != NetworkManager::Device::Ethernet)
        return false;

    NetworkManager::activateConnection(connection->connection()->path(), m_wDevice->uni(), "");
    return true;
}

QList<WiredConnection *> DeviceManagerRealize::wiredItems() const
{
    return m_wiredConnections;
}

void DeviceManagerRealize::onWiredConnectionChanged()
{
    if (m_wDevice->type() != NetworkManager::Device::Ethernet)
        return;

    PRINT_INFO_MESSAGE("start.....");
    NetworkManager::Connection::List connections = m_wDevice->availableConnections();
    QList<WiredConnection *> newConnection;
    QList<WiredConnection *> allConnection;
    for (QSharedPointer<NetworkManager::Connection> conn : connections) {
        if (conn->settings()->connectionType() != ConnectionSettings::ConnectionType::Wired)
            continue;

        QJsonObject json = createConnectionJson(conn);
        WiredConnection *wiredConn = findWiredConnection(conn->path());
        if (!wiredConn) {
            wiredConn = new WiredConnection();
            newConnection << wiredConn;
        }
        wiredConn->setConnection(json);
        allConnection << wiredConn;
    }
    QList<WiredConnection *> rmConnection;
    for (WiredConnection *conn : m_wiredConnections) {
        if (!allConnection.contains(conn)) {
            m_wiredConnections.removeOne(conn);
            rmConnection << conn;
        }
    }

    PRINT_DEBUG_MESSAGE(QString("wiredConnectionSize:%1").arg(allConnection.size()));

    sortWiredItem(allConnection);

    m_wiredConnections = allConnection;
    if (newConnection.size() > 0 || rmConnection.size() > 0) {
        PRINT_DEBUG_MESSAGE(QString("new connection size: %1, remove connection size: %2").arg(newConnection.size()).arg(rmConnection.size()));
        if (newConnection.size() > 0)
            Q_EMIT connectionAdded(newConnection);

        if (rmConnection.size() > 0)
            Q_EMIT connectionRemoved(rmConnection);

        for (WiredConnection *conn : rmConnection) {
            if (conn == m_activeWiredConnection)
                m_activeWiredConnection = Q_NULLPTR;

            delete conn;
        }
    }
}

void DeviceManagerRealize::changeWiredStatus(Device::State newstate)
{
    if (m_wDevice->type() != NetworkManager::Device::Ethernet)
        return;

    PRINT_DEBUG_MESSAGE(QString("Device:%1, new Status: %2").arg(m_wDevice->interfaceName()).arg(QMetaEnum::fromType<Device::State>().valueToKey(newstate)));
    // 查看当前活动连接是否存在与连接列表中，如果不存在，就将当前活动连接置空
    if (m_activeWiredConnection && !m_wiredConnections.contains(m_activeWiredConnection))
        m_activeWiredConnection = Q_NULLPTR;

    ConnectionStatus oldStatus = ConnectionStatus::Unknown;
    if (m_activeWiredConnection)
        oldStatus = m_activeWiredConnection->status();

    NetworkManager::ActiveConnection::Ptr activeConn = m_wDevice->activeConnection();
    if (activeConn.isNull()) {
        // 断开了连接以后，如果以前存在连接，则向外抛出断开的信号
        if (m_activeWiredConnection) {
            m_activeWiredConnection->setConnectionStatus(ConnectionStatus::Deactivated);
            m_activeWiredConnection = nullptr;
            Q_EMIT activeConnectionChanged();
        }
        PRINT_INFO_MESSAGE("active connection is empty");
        return;
    }

    WiredConnection *currentConnection = findWiredConnectionByUuid(activeConn->uuid());
    if (!currentConnection) {
        PRINT_INFO_MESSAGE(QString("cannot find connection uuid: %1").arg(activeConn->uuid()));
        return;
    }

    ConnectionStatus newStatus = convertStatus(newstate);
    if (m_activeWiredConnection && currentConnection != m_activeWiredConnection) {
        m_activeWiredConnection->setConnectionStatus(ConnectionStatus::Deactivated);
        oldStatus = ConnectionStatus::Unknown;
    }
    if(oldStatus != newStatus){
        m_activeWiredConnection = currentConnection;
        currentConnection->setConnectionStatus(newStatus);
        Q_EMIT activeConnectionChanged();
    }
}

void DeviceManagerRealize::changeWirelessStatus(Device::State newstate)
{
    if (m_wDevice->type() != NetworkManager::Device::Wifi)
        return;

    PRINT_DEBUG_MESSAGE(QString("Device:%1, new Status: %2").arg(m_wDevice->interfaceName()).arg(QMetaEnum::fromType<Device::State>().valueToKey(newstate)));
    if (m_activeWirelessConnection && !m_wirelessConnections.contains(m_activeWirelessConnection))
        m_activeWirelessConnection = Q_NULLPTR;

    ConnectionStatus oldStatus = ConnectionStatus::Unknown;
    if (m_activeWirelessConnection) {
        AccessPoints *activePoint = findAccessPoints(m_activeWirelessConnection->connection()->ssid());
        if (activePoint)
            oldStatus = activePoint->status();
    }

    NetworkManager::ActiveConnection::Ptr activeConn = m_wDevice->activeConnection();
    if (activeConn.isNull()) {
        if (m_activeWirelessConnection) {
            AccessPoints *ap = findAccessPoints(m_activeWirelessConnection->connection()->ssid());
            if (ap)
                ap->m_status = ConnectionStatus::Deactivated;

            m_activeWirelessConnection = nullptr;
            Q_EMIT activeConnectionChanged();
        }
        PRINT_INFO_MESSAGE("active connection is empty");
        return;
    }

    WirelessConnection *currentConnection = findWirelessConnectionBySsid(activeConn->id());
    if (!currentConnection) {
        PRINT_DEBUG_MESSAGE(QString("cannot find connection id: %1").arg(activeConn->id()));
        Q_EMIT activeConnectionChanged();
        return;
    }

    if (m_activeWirelessConnection && m_activeWirelessConnection != currentConnection) {
        AccessPoints *ap = findAccessPoints(m_activeWirelessConnection->connection()->ssid());
        if (ap)
            ap->m_status = ConnectionStatus::Deactivated;
        oldStatus = ConnectionStatus::Unknown;
    }

    ConnectionStatus newStatus = convertStatus(newstate);
    if (newStatus != oldStatus) {
        m_activeWirelessConnection = currentConnection;
        AccessPoints *ap = findAccessPoints(currentConnection->connection()->ssid());
        if (ap) {
            ap->m_status = newStatus;
            Q_EMIT activeConnectionChanged();
        }
    }
}

WirelessConnection *DeviceManagerRealize::findWirelessConnectionByUuid(const QString &uuid)
{
    for (WirelessConnection *conn : m_wirelessConnections) {
        if (conn->connection()->uuid() == uuid)
            return conn;
    }

    return Q_NULLPTR;
}

void DeviceManagerRealize::changeStatus(Device::State newstate)
{
    // 转换当前获取的状态为设备的状态
    setDeviceStatus(convertDeviceStatus(newstate));
    switch (m_wDevice->type()) {
    case NetworkManager::Device::Ethernet:
        changeWiredStatus(newstate);
        break;
    case NetworkManager::Device::Wifi:
        changeWirelessStatus(newstate);
        break;
    default: break;
    }
}

bool DeviceManagerRealize::getHotspotIsEnabled()
{
    NetworkManager::WirelessDevice::Ptr wirelessDevice = m_wDevice.staticCast<NetworkManager::WirelessDevice>();
    NetworkManager::WirelessDevice::OperationMode mode = wirelessDevice->mode();
    return (mode == NetworkManager::WirelessDevice::OperationMode::ApMode);
}

bool DeviceManagerRealize::hotspotEnabled()
{
    return m_hotspotEnabled;
}

void DeviceManagerRealize::onStatusChanged(Device::State newstate, Device::State oldstate, Device::StateChangeReason reason)
{
    Q_UNUSED(oldstate);
    Q_UNUSED(reason);

    changeStatus(newstate);
}

WiredConnection *DeviceManagerRealize::findWiredConnectionByUuid(const QString &uuid)
{
    for (WiredConnection *conn : m_wiredConnections) {
        if (conn->connection()->uuid() == uuid)
            return conn;
    }

    return Q_NULLPTR;
}

void DeviceManagerRealize::onWirelessConnectionChanged()
{
    if (m_wDevice->type() != NetworkManager::Device::Wifi)
        return;

    PRINT_DEBUG_MESSAGE(QString("Device:%1").arg(m_wDevice->interfaceName()));
    NetworkManager::Connection::List connections = m_wDevice->availableConnections();
    QList<WirelessConnection *> newConnection;
    QList<WirelessConnection *> allConnection;
    for (QSharedPointer<NetworkManager::Connection> conn : connections) {
        if (conn->settings()->connectionType() != ConnectionSettings::ConnectionType::Wireless)
            continue;

        const QJsonObject json = createConnectionJson(conn);
        WirelessConnection *wirelessConn = findWirelessConnection(conn->path());
        if (!wirelessConn) {
            wirelessConn = new WirelessConnection();
            newConnection << wirelessConn;
        }

        wirelessConn->setConnection(json);
        allConnection << wirelessConn;
    }

    createWlans(allConnection);
}

void DeviceManagerRealize::createWlans(QList<WirelessConnection *> &allConnections)
{
    PRINT_DEBUG_MESSAGE(QString("allConnections size:%1").arg(allConnections.size()));
    QList<AccessPoints *> newAps, allAccessPoints;
    NetworkManager::WirelessDevice::Ptr wirelessDevice = m_wDevice.staticCast<NetworkManager::WirelessDevice>();
    NetworkManager::WirelessNetwork::List networks = wirelessDevice->networks();
    for (QSharedPointer<NetworkManager::WirelessNetwork> network : networks) {
        QSharedPointer<AccessPoint> ap = network->referenceAccessPoint();
        const QJsonObject json = createWlanJson(network);
        AccessPoints *accessPoint = findAccessPoints(ap->ssid());
        if (!accessPoint) {
            accessPoint = new AccessPoints(json);
            newAps << accessPoint;
        } else {
            accessPoint->updateAccessPoints(json);
        }

        allAccessPoints << accessPoint;
    }
    QList<AccessPoints *> rmAps;
    for (AccessPoints *ap : m_accessPoints) {
        if (!allAccessPoints.contains(ap)) {
            m_accessPoints.removeOne(ap);
            rmAps << ap;
        }
    }
    // 在信号发送之前同步accessPoints数据，因为在抛出信号后，外面可能通过获取内部所有的WLAN数据来更新内容，此时需要保证WLAN列表是最新的
    m_accessPoints = allAccessPoints;
    syncWlanAndConnections(allConnections);

    // 删除不存在的Connections，也放在信号发送之前，原因和accessPoints一样
    QList<WirelessConnection *> rmConnections;
    for (WirelessConnection *conn : m_wirelessConnections) {
        if (!allConnections.contains(conn) && !rmConnections.contains(conn))
            rmConnections << conn;
    }

    if (rmConnections.contains(m_activeWirelessConnection))
        m_activeWirelessConnection = Q_NULLPTR;

    m_wirelessConnections = allConnections;

    PRINT_DEBUG_MESSAGE(QString("new AccessPoint size:%1, remove AccessPoint Size: %2").arg(newAps.size()).arg(rmAps.size()));
    if (rmAps.size() > 0 || newAps.size() > 0) {
        if (newAps.size() > 0) {
            Q_EMIT networkAdded(newAps);
            // 新增无线网络之前最好先刷新网络列表状态，因为考虑到连接隐藏网络的时候，没有触发网络状态变化信号，因此此处需要手动调用
            changeStatus(m_wDevice->state());
        }

        if (rmAps.size() > 0)
            Q_EMIT networkRemoved(rmAps);

        for (AccessPoints *ap : rmAps)
            delete ap;
    }

    // 删除无用的数据
    for (WirelessConnection *rmConn : rmConnections)
        delete rmConn;
}

void DeviceManagerRealize::syncWlanAndConnections(QList<WirelessConnection *> &allConnections)
{
    PRINT_DEBUG_MESSAGE(QString("allConnections size:%1").arg(allConnections.size()));
    if (m_accessPoints.isEmpty()) {
        clearListData(allConnections);
        PRINT_INFO_MESSAGE("m_accessPoints is Empty");
        return;
    }

    QList<WirelessConnection *> connections;
    // 找到每个热点对应的Connection，并将其赋值
    for (AccessPoints *accessPoint : m_accessPoints) {
        WirelessConnection *connection = findConnectionByAccessPoint(accessPoint, allConnections);
        if (!connection) {
            connection = WirelessConnection::createConnection(accessPoint);
            m_wirelessConnections << connection;
        }

        connection->m_accessPoints = accessPoint;
        connections << connection;
    }
    // 删除列表中没有AccessPoints的Connection，让两边保持数据一致
    QList<WirelessConnection *> rmConns;
    for (WirelessConnection *connection : allConnections) {
        if (!connections.contains(connection))
            rmConns << connection;
    }

    for (WirelessConnection *rmConnection : rmConns) {
        allConnections.removeOne(rmConnection);

        // 如果当前所有的无线连接列表中有不存在的连接，那么需要将其从无线连接中移除，然后再delete
        // 因为后面会将m_wirelessConnections中不存在于allConnections的连接delete，如果
        // 此处不从m_wirelessConnections移除掉这个connection，那么后面就会出现删除两次引起报错
        if (m_wirelessConnections.contains(rmConnection))
            m_wirelessConnections.removeOne(rmConnection);

        delete rmConnection;
    }

    allConnections = connections;
}

AccessPoints *DeviceManagerRealize::findAccessPoints(const QString &ssid)
{
    for (AccessPoints *ap : m_accessPoints) {
        if (ap->ssid() == ssid)
            return ap;
    }

    return Q_NULLPTR;
}

QJsonObject DeviceManagerRealize::createWlanJson(QSharedPointer<NetworkManager::WirelessNetwork> network)
{
    QSharedPointer<AccessPoint> ap = network->referenceAccessPoint();
    QJsonObject jsonObject;
    jsonObject.insert("Ssid", ap->ssid());
    jsonObject.insert("Strength", network->signalStrength());
    bool secured = ap->capabilities() == AccessPoint::Capability::Privacy || ap->wpaFlags() != 0 || ap->rsnFlags() != 0;
    jsonObject.insert("Secured", secured);
    jsonObject.insert("SecuredInEap", secured);
    jsonObject.insert("Flags", static_cast<int>(ap->capabilities()));
    int frequency = static_cast<int>(ap->frequency());
    jsonObject.insert("Frequency", frequency);
    jsonObject.insert("Path", ap->uni());
    return jsonObject;
}

QJsonObject DeviceManagerRealize::createConnectionJson(QSharedPointer<NetworkManager::Connection> networkConnection)
{
    QJsonObject json;
    json.insert("Path", networkConnection->path());
    json.insert("Uuid", networkConnection->uuid());
    json.insert("Id", networkConnection->settings()->id());
    json.insert("HwAddress", realHwAdr());
    json.insert("ClonedAddress", realHwAdr());
    json.insert("Ssid", networkConnection->name());
    return json;
}

WirelessConnection *DeviceManagerRealize::findConnectionByAccessPoint(const AccessPoints *accessPoint, QList<WirelessConnection *> &allConnections)
{
    for (WirelessConnection *connection : allConnections) {
        if (connection->accessPoints() == accessPoint)
            return connection;

        if (connection->connection()->ssid() == accessPoint->ssid())
            return connection;
    }

    return Q_NULLPTR;
}

WiredConnection *DeviceManagerRealize::findWiredConnection(const QString &path)
{
    for (WiredConnection *connection : m_wiredConnections) {
        if (connection->connection()->path() == path)
            return connection;
    }

    return Q_NULLPTR;
}

WirelessConnection *DeviceManagerRealize::findWirelessConnectionBySsid(const QString &ssid)
{
    for (WirelessConnection *connection : m_wirelessConnections) {
        if (connection->connection()->ssid() == ssid)
            return connection;
    }

    return Q_NULLPTR;
}

WirelessConnection *DeviceManagerRealize::findWirelessConnection(const QString &path)
{
    for (WirelessConnection *connection : m_wirelessConnections) {
        if (connection->connection()->path() == path)
            return connection;
    }

    return Q_NULLPTR;
}

ConnectionStatus DeviceManagerRealize::convertStatus(Device::State state)
{
    switch (state) {
    case Device::State::Failed:
    case Device::State::NeedAuth:
    case Device::State::Unmanaged:
    case Device::State::Unavailable:
    case Device::State::Disconnected:
        return ConnectionStatus::Deactivated;
    case Device::State::Preparing:
    case Device::State::CheckingIp:
    case Device::State::ConfiguringIp:
    case Device::State::ConfiguringHardware:
    case Device::State::WaitingForSecondaries:
        return ConnectionStatus::Activating;
    case Device::State::Activated:
        return ConnectionStatus::Activated;
    case Device::State::Deactivating:
        return ConnectionStatus::Deactivating;
    default: break;
    }

    return ConnectionStatus::Unknown;
}

DeviceStatus DeviceManagerRealize::convertDeviceStatus(Device::State state)
{
    if (m_wDevice->type() == NetworkManager::Device::Wifi) {
        // 如果当前网卡开启了热点，则认为它是连接断开的状态
        NetworkManager::WirelessDevice::Ptr wirelessDevice = m_wDevice.staticCast<NetworkManager::WirelessDevice>();
        if (wirelessDevice->mode() == NetworkManager::WirelessDevice::OperationMode::ApMode)
            return DeviceStatus::Disconnected;
    }
    switch (state) {
    case Device::State::Failed:
        return DeviceStatus::Failed;
    case Device::State::NeedAuth:
        return DeviceStatus::Needauth;
    case Device::State::Activated:
        return DeviceStatus::Activated;
    case Device::State::Preparing:
        return DeviceStatus::Prepare;
    case Device::State::Unmanaged:
        return DeviceStatus::Unmanaged;
    case Device::State::CheckingIp:
        return DeviceStatus::IpCheck;
    case Device::State::Unavailable:
        return DeviceStatus::Unavailable;
    case Device::State::Deactivating:
        return DeviceStatus::Deactivation;
    case Device::State::Disconnected:
        return DeviceStatus::Disconnected;
    case Device::State::ConfiguringIp:
    case Device::State::ConfiguringHardware:
        return DeviceStatus::Config;
    case Device::State::WaitingForSecondaries:
        return DeviceStatus::Secondaries;
    default: break;
    }

    return DeviceStatus::Unknown;
}
