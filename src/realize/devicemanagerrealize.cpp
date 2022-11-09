// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
            connect(wirelessDevice.get(), &NetworkManager::WirelessDevice::activeAccessPointChanged, this, &DeviceManagerRealize::onWirelessConnectionChanged);
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
    if (m_wDevice->type() == NetworkManager::Device::Wifi) {
        QList<AccessPoints *> accessPoints;
        for (QSharedPointer<AccessPoints> conn : m_accessPoints)
            accessPoints << conn.data();

        return accessPoints;
    }

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

    QSharedPointer<WirelessConnection> conn = findConnectionByAccessPoint(item, m_wirelessConnections);
    if (!conn.isNull())
        NetworkManager::activateConnection(conn->connection()->path(), m_wDevice->uni(), "");
}

QList<WirelessConnection *> DeviceManagerRealize::wirelessItems() const
{
    QList<WirelessConnection *> items;
    for (QSharedPointer<WirelessConnection> item : m_wirelessConnections)
        items << item.data();

    return items;
}

AccessPoints *DeviceManagerRealize::activeAccessPoints() const
{
    if (m_wDevice->type() != NetworkManager::Device::Wifi)
        return nullptr;

    QSharedPointer<NetworkManager::WirelessDevice> device = m_wDevice.staticCast<NetworkManager::WirelessDevice>();
    AccessPoint::Ptr currentActiveAp = device->activeAccessPoint();
    if (!currentActiveAp.isNull()) {
        for (QSharedPointer<AccessPoints> ap : m_accessPoints) {
            if (ap->ssid() == currentActiveAp->ssid())
                return ap.data();
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
    if (m_activeWirelessConnection && !wirelessItems().contains(m_activeWirelessConnection))
        m_activeWirelessConnection = Q_NULLPTR;

    ConnectionStatus oldStatus = ConnectionStatus::Unknown;
    if (m_activeWirelessConnection) {
        QSharedPointer<AccessPoints> activePoint = findAccessPoints(m_activeWirelessConnection->connection()->ssid());
        if (activePoint)
            oldStatus = activePoint->status();
    }

    NetworkManager::ActiveConnection::Ptr activeConn = m_wDevice->activeConnection();
    if (activeConn.isNull()) {
        if (m_activeWirelessConnection) {
            QSharedPointer<AccessPoints> ap = findAccessPoints(m_activeWirelessConnection->connection()->ssid());
            if (ap)
                ap->updateConnectionStatus(ConnectionStatus::Deactivated);

            m_activeWirelessConnection = nullptr;
            Q_EMIT activeConnectionChanged();
        }
        PRINT_INFO_MESSAGE("active connection is empty");
        return;
    }

    QSharedPointer<WirelessConnection> currentConnection = findWirelessConnectionBySsid(activeConn->id());
    if (currentConnection.isNull()) {
        PRINT_DEBUG_MESSAGE(QString("cannot find connection id: %1").arg(activeConn->id()));
        Q_EMIT activeConnectionChanged();
        return;
    }

    if (m_activeWirelessConnection && m_activeWirelessConnection != currentConnection) {
        QSharedPointer<AccessPoints> ap = findAccessPoints(m_activeWirelessConnection->connection()->ssid());
        if (!ap.isNull())
            ap->updateConnectionStatus(ConnectionStatus::Deactivated);
        oldStatus = ConnectionStatus::Unknown;
    }

    ConnectionStatus newStatus = convertStatus(newstate);
    if (newStatus != oldStatus) {
        m_activeWirelessConnection = currentConnection.data();
        QSharedPointer<AccessPoints> ap = findAccessPoints(currentConnection->connection()->ssid());
        if (!ap.isNull()) {
            ap->updateConnectionStatus(newStatus);
            Q_EMIT activeConnectionChanged();
        }
    }
}

WirelessConnection *DeviceManagerRealize::findWirelessConnectionByUuid(const QString &uuid)
{
    for (QSharedPointer<WirelessConnection> conn : m_wirelessConnections) {
        if (conn->connection()->uuid() == uuid)
            return conn.data();
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
    QList<QSharedPointer<WirelessConnection>> allConnection;
    for (QSharedPointer<NetworkManager::Connection> conn : connections) {
        if (conn->settings()->connectionType() != ConnectionSettings::ConnectionType::Wireless)
            continue;

        const QJsonObject json = createConnectionJson(conn);
        QSharedPointer<WirelessConnection> wirelessConn = findWirelessConnection(conn->path());
        if (wirelessConn.isNull())
            wirelessConn.reset(new WirelessConnection());

        wirelessConn->setConnection(json);
        QSharedPointer<WirelessConnection> a = wirelessConn;

        allConnection.append(wirelessConn);
    }

    createWlans(allConnection);
}

void DeviceManagerRealize::createWlans(QList<QSharedPointer<WirelessConnection>> &allConnections)
{
    PRINT_DEBUG_MESSAGE(QString("allConnections size:%1").arg(allConnections.size()));
    QList<QSharedPointer<AccessPoints >> allAccessPoints;
    QList<AccessPoints *> newAps;
    NetworkManager::WirelessDevice::Ptr wirelessDevice = m_wDevice.staticCast<NetworkManager::WirelessDevice>();
    NetworkManager::WirelessNetwork::List networks = wirelessDevice->networks();
    for (QSharedPointer<NetworkManager::WirelessNetwork> network : networks) {
        QSharedPointer<AccessPoint> ap = network->referenceAccessPoint();
        const QJsonObject json = createWlanJson(ap);
        QSharedPointer<AccessPoints> accessPoint = findAccessPoints(ap->ssid());
        if (!accessPoint) {
            accessPoint.reset(new AccessPoints(json));
            accessPoint->m_devicePath = path();
            newAps << accessPoint.data();
        } else {
            accessPoint->updateAccessPoints(json);
        }

        allAccessPoints << accessPoint;
    }
    QList<QSharedPointer<AccessPoints>> rmAps;
    for (QSharedPointer<AccessPoints> ap : m_accessPoints) {
        if (!allAccessPoints.contains(ap)) {
            rmAps << ap;
            m_accessPoints.removeOne(ap);
        }
    }
    // 在信号发送之前同步accessPoints数据，因为在抛出信号后，外面可能通过获取内部所有的WLAN数据来更新内容，此时需要保证WLAN列表是最新的
    m_accessPoints = allAccessPoints;
    // 确保激活的在列表中
    AccessPoint::Ptr activeAp = wirelessDevice->activeAccessPoint();
    if (!activeAp.isNull()) {
        const QJsonObject json = createWlanJson(activeAp);
        QSharedPointer<AccessPoints> accessPoint = findAccessPoints(activeAp->ssid());
        if (!accessPoint) {
            accessPoint.reset(new AccessPoints(json));
            newAps << accessPoint.data();
            allAccessPoints << accessPoint;
            m_accessPoints << accessPoint;
        } else {
            accessPoint->updateAccessPoints(json);
        }
    }
    syncWlanAndConnections(allConnections);

    m_wirelessConnections = allConnections;
    // 如果当前的连接中不包含当前活动连击，认为当前活动连接为空
    if (!wirelessItems().contains(m_activeWirelessConnection))
        m_activeWirelessConnection = Q_NULLPTR;

    PRINT_DEBUG_MESSAGE(QString("new AccessPoint size:%1, remove AccessPoint Size: %2").arg(newAps.size()).arg(rmAps.size()));
    if (rmAps.size() > 0 || newAps.size() > 0) {
        if (newAps.size() > 0) {
            Q_EMIT networkAdded(newAps);
            // 新增无线网络之前最好先刷新网络列表状态，因为考虑到连接隐藏网络的时候，没有触发网络状态变化信号，因此此处需要手动调用
            changeStatus(m_wDevice->state());
        }

        if (rmAps.size() > 0) {
            QList<AccessPoints *> removeAccessPoints;
            for (QSharedPointer<AccessPoints> ap : rmAps)
                removeAccessPoints << ap.data();

            Q_EMIT networkRemoved(removeAccessPoints);
        }
    }
}

void DeviceManagerRealize::syncWlanAndConnections(QList<QSharedPointer<WirelessConnection>> &allConnections)
{
    PRINT_DEBUG_MESSAGE(QString("allConnections size:%1").arg(allConnections.size()));
    if (m_accessPoints.isEmpty()) {
        allConnections.clear();
        PRINT_INFO_MESSAGE("m_accessPoints is Empty");
        return;
    }

    QList<QSharedPointer<WirelessConnection>> connections;
    // 找到每个热点对应的Connection，并将其赋值
    for (QSharedPointer<AccessPoints> accessPoint : m_accessPoints) {
        QSharedPointer<WirelessConnection> connection = findConnectionByAccessPoint(accessPoint.data(), allConnections);
        if (connection.isNull()) {
            connection.reset(WirelessConnection::createConnection(accessPoint.data()));
            m_wirelessConnections.append(connection);
        }

        connection->m_accessPoints = accessPoint.data();
        connections << connection;
    }

    allConnections = connections;
}

QSharedPointer<AccessPoints> DeviceManagerRealize::findAccessPoints(const QString &ssid)
{
    for (QSharedPointer<AccessPoints> ap : m_accessPoints) {
        if (ap->ssid() == ssid)
            return ap;
    }

    return Q_NULLPTR;
}

QJsonObject DeviceManagerRealize::createWlanJson(QSharedPointer<NetworkManager::AccessPoint> ap)
{
    QJsonObject jsonObject;
    jsonObject.insert("Ssid", ap->ssid());
    jsonObject.insert("Strength", ap->signalStrength());
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

QSharedPointer<WirelessConnection> DeviceManagerRealize::findConnectionByAccessPoint(const AccessPoints *accessPoint, QList<QSharedPointer<WirelessConnection>> &allConnections)
{
    for (QSharedPointer<WirelessConnection> connection : allConnections) {
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

QSharedPointer<WirelessConnection> DeviceManagerRealize::findWirelessConnectionBySsid(const QString &ssid)
{
    for (QSharedPointer<WirelessConnection> connection : m_wirelessConnections) {
        if (connection->connection()->ssid() == ssid)
            return connection;
    }

    return Q_NULLPTR;
}

QSharedPointer<WirelessConnection> DeviceManagerRealize::findWirelessConnection(const QString &path)
{
    for (QSharedPointer<WirelessConnection> connection : m_wirelessConnections) {
        if (connection->connection()->path() == path)
            return connection;
    }

    return Q_NULLPTR;
}

ConnectionStatus DeviceManagerRealize::convertStatus(Device::State state)
{
    switch (state) {
    case Device::State::Failed:
    case Device::State::Unmanaged:
    case Device::State::Unavailable:
    case Device::State::Disconnected:
        return ConnectionStatus::Deactivated;
    case Device::State::Preparing:
    case Device::State::NeedAuth:
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
