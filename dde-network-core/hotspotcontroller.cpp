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

#include "hotspotcontroller.h"
#include "wirelessdevice.h"
#include "utils.h"

#include <networkmanagerqt/manager.h>

using namespace dde::network;
using namespace NetworkManager;

HotspotController::HotspotController(NetworkInter *networkInter, QObject *parent)
    : QObject(parent)
    , m_networkInter(networkInter)
    , m_activeConnectionChanged(false)
{
    Q_ASSERT(m_networkInter);
}

HotspotController::~HotspotController()
{
}

void HotspotController::setEnabled(WirelessDevice *device, const bool enable)
{
    QList<HotspotItem *> deviceHotsItem = items(device);
    if (enable) {
        if (deviceHotsItem.size() > 0) {
            // 在打开热点的时候,默认开启第一个热点
            HotspotItem *item = deviceHotsItem[0];
            m_networkInter->ActivateConnection(item->connection()->uuid(), QDBusObjectPath(device->path()));
        }
    } else {
        // 在关闭热点的时候,找到当前已经连接的热点,并断开它的连接
        for (HotspotItem *item : deviceHotsItem) {
            if (item->connectionStatus() == ConnectionStatus::Activated) {
                m_networkInter->DeactivateConnection(item->connection()->uuid());
                break;
            }
        }

        // 关闭热点后，需要设置DeviceManaged
        QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(m_networkInter->SetDeviceManaged(device->path(), false));
        connect(w, &QDBusPendingCallWatcher::finished, [ = ]() {
            m_networkInter->SetDeviceManaged(device->path(), true);
        });

        connect(w, &QDBusPendingCallWatcher::finished, w, &QDBusPendingCallWatcher::deleteLater);
    }
}

bool HotspotController::enabled(WirelessDevice *device)
{
    QString devicePath = device->path();
    return m_deviceEnableStatus.contains(devicePath) ? m_deviceEnableStatus.value(devicePath) : false;
}

bool HotspotController::supportHotspot()
{
    return m_devices.size() > 0;
}

void HotspotController::connectItem(HotspotItem *item)
{
    // 获取当前连接的UUID和设备的path
    m_networkInter->ActivateConnection(item->connection()->uuid(), QDBusObjectPath(item->devicePath()));
}

void HotspotController::connectItem(const QString &uuid)
{
    for (HotspotItem *item : m_hotspotItems) {
        if (item->connection()->uuid() == uuid) {
            connectItem(item);
            break;
        }
    }
}

void HotspotController::disconnectItem()
{
    if (!m_activePath.isEmpty()) {
        QDBusPendingReply<> reply = deactivateConnection(m_activePath);
        reply.waitForFinished();
    }
}

QList<HotspotItem *> HotspotController::items(WirelessDevice *device)
{
    // 获取当前设备对应的所有的热点
    QList<HotspotItem *> items;
    for (HotspotItem *item : m_hotspotItems) {
        if (item->device() == device)
            items << item;
    }

    return items;
}

QList<WirelessDevice *> HotspotController::devices()
{
    // 这里只支持已经启用的设备
    QList<WirelessDevice *> hotDevices;
    for (WirelessDevice *dev : m_devices) {
        if (dev->isEnabled())
            hotDevices << dev;
    }

    return hotDevices;
}

HotspotItem *HotspotController::findItem(WirelessDevice *device, const QJsonObject &json)
{
    // 从列表中查找指定UUID的连接
    for (HotspotItem *item : m_hotspotItems) {
        if (item->device() == device
                && item->connection()->uuid() == json.value("Uuid").toString())
            return item;
    }

    return Q_NULLPTR;
}

void HotspotController::updateActiveConnection(const QJsonObject &activeConnections)
{
    m_activeDevices.clear();
    // 先保存所有的连接状态
    QMap<QString, ConnectionStatus> allConnectionStatus;
    for (HotspotItem *item : m_hotspotItems) {
        allConnectionStatus[item->connection()->uuid()] = item->connectionStatus();
        item->setConnectionStatus(ConnectionStatus::Deactivated);
    }

    QString oldActivePath = m_activePath;
    m_activePath.clear();

    // 在这里记录一个标记，用来表示是否发送活动连接发生变化的信号，
    // 因为这个连接更新后，紧接着会调用获取活动连接信息的信号，响应这个信号会调用下面的updateActiveConnectionInfo函数
    m_activeConnectionChanged = false;

    QStringList keys = activeConnections.keys();
    for (int i = 0; i < keys.size(); i++) {
        QString path = keys[i];
        QJsonObject activeConnection = activeConnections.value(path).toObject();

        QString uuid = activeConnection.value("Uuid").toString();
        ConnectionStatus state = convertConnectionStatus(activeConnection.value("State").toInt());

        QJsonArray devicePaths = activeConnection.value("Devices").toArray();
        for (const QJsonValue jsonValue : devicePaths) {
            QString devicePath = jsonValue.toString();
            WirelessDevice *device = findDevice(devicePath);
            HotspotItem *hotspotItem = findItem(device, activeConnection);
            if (hotspotItem) {
                hotspotItem->setConnectionStatus(state);
                if (allConnectionStatus.contains(uuid)) {
                    ConnectionStatus oldConnectionStatus = allConnectionStatus[uuid];
                    if (!m_activeConnectionChanged && oldConnectionStatus != hotspotItem->connectionStatus()) {
                        m_activeConnectionChanged = true;
                        if (!m_activeDevices.contains(device))
                            m_activeDevices << device;
                    }
                }
            }
        }

        if (state == ConnectionStatus::Activated)
            m_activePath = path;
    }
}

void HotspotController::updateActiveConnectionInfo()
{
    // 设备路径
    QStringList devicePaths;
    // 更新当前的活动连接
    for (QJsonObject json : m_activeconnection)
        devicePaths << json.value("Device").toString();

    // 根据活动列表中是否有当前的活动连接来决定热点是否启用
    for (WirelessDevice *device : m_devices) {
        QString devicePath = device->path();
        bool oldEnabled = (m_deviceEnableStatus.contains(devicePath) ? m_deviceEnableStatus.value(devicePath) : false);
        bool newEnabled = (devicePaths.contains(device->path()));
        if (oldEnabled != newEnabled)
            Q_EMIT device->hotspotEnableChanged(newEnabled);

        m_deviceEnableStatus[devicePath] = newEnabled;
    }

    // 如果连接信息发生了变化，则需要像外发送连接改变的信号
    // 在信号发出去后，需要复位m_activeConnectionChanged标记，防止下次重新触发
    if (m_activeConnectionChanged) {
        Q_EMIT activeConnectionChanged(m_activeDevices);
        m_activeConnectionChanged = false;
        m_activeDevices.clear();
    }
}

WirelessDevice *HotspotController::findDevice(const QString &path)
{
    for (WirelessDevice *device : m_devices) {
        if (device->path() == path)
            return device;
    }

    return Q_NULLPTR;
}

void HotspotController::updateActiveConnectionInfo(const QList<QJsonObject> &conns)
{
    m_activeconnection = conns;
    updateActiveConnectionInfo();
}

void HotspotController::updateConnections(const QJsonArray &jsons, const QList<NetworkDeviceBase *> &devices)
{
    // 筛选出通用的(HwAddress为空)热点和指定HwAddress的热点
    QList<QJsonObject> commonConnections;
    QMap<QString, QList<QJsonObject>> deviceConnections;
    for (QJsonValue jsonValue : jsons) {
        QJsonObject json = jsonValue.toObject();
        QString hwAddress = json.value("HwAddress").toString();
        if (hwAddress.isEmpty())
            commonConnections << json;
        else
            deviceConnections[hwAddress] << json;
    }

    // 将所有热点的UUID缓存，用来对比不存在的热点，删除不存在的热点
    QMap<WirelessDevice *, QList<HotspotItem *>> newItems;
    QStringList allHotsItem;
    // HwAddress为空的热点适用于所有的设备，HwAddress不为空的热点只适用于指定的设备
    QList<WirelessDevice *> tmpDevices = m_devices;
    m_devices.clear();
    for (NetworkDeviceBase *dev : devices) {
        if (dev->deviceType() != DeviceType::Wireless)
            continue;

        WirelessDevice *device = static_cast<WirelessDevice *>(dev);
        // 如果当前设备不支持热点，则无需继续
        if (!device->supportHotspot())
            continue;

        QList<QJsonObject> hotspotJsons = commonConnections;
        if (deviceConnections.contains(device->realHwAdr()))
            hotspotJsons << deviceConnections[device->realHwAdr()];

        for (QJsonValue value : hotspotJsons) {
            QJsonObject json = value.toObject();
            HotspotItem *item = findItem(device, json);
            if (!item) {
                item = new HotspotItem(device);
                m_hotspotItems << item;
                newItems[device] << item;
            }

            item->setConnection(json);
            QString pathName = QString("%1-%2").arg(device->path()).arg(json.value("Path").toString());
            allHotsItem << pathName;
        }

        m_devices << device;
    }

    bool hotspotEnabled = (tmpDevices.size() > 0);
    if ((m_devices.size() > 0) != hotspotEnabled)
        Q_EMIT enabledChanged(hotspotEnabled);

    // 查找移除的设备
    QList<WirelessDevice *> rmDevices;
    for (WirelessDevice *device : tmpDevices) {
        if (!m_devices.contains(device))
            rmDevices << device;
    }

    // 查找新增的设备
    QList<WirelessDevice *> newDevices;
    for (WirelessDevice *device : m_devices) {
        if (!tmpDevices.contains(device))
            newDevices << device;
    }

    // 告诉外面有新增的热点设备
    if (newDevices.size() > 0)
        Q_EMIT deviceAdded(newDevices);

    // 告诉外面有移除的热点设备
    if (rmDevices.size() > 0)
        Q_EMIT deviceRemove(rmDevices);

    // 如果有新增的连接，则发送新增连接的信号
    if (newItems.size() > 0)
        Q_EMIT itemAdded(newItems);

    QMap<WirelessDevice *, QList<HotspotItem *>> rmItems;
    // 删除列表中不存在的设备
    for (HotspotItem *item : m_hotspotItems) {
        QString pathKey = QString("%1-%2").arg(item->device()->path()).arg(item->connection()->path());
        if (!allHotsItem.contains(pathKey)) {
            m_hotspotItems.removeOne(item);
            rmItems[item->device()] << item;
        }
    }

    // 如果有删除的连接，向外发送删除的信号
    if (rmItems.size() > 0)
        Q_EMIT itemRemoved(rmItems);

    // 清空已经删除的对象
    QList<QList<HotspotItem *>> allRmItems = rmItems.values();
    for (QList<HotspotItem *> allItem : allRmItems) {
        for (HotspotItem *item : allItem)
            delete item;
    }
}

/**
 * @brief UHotspotItem::UHotspotItem
 */

HotspotItem::HotspotItem(WirelessDevice *device)
    : ControllItems()
    , m_device(device)
    , m_devicePath(m_device->path())
    , m_connectionStatus(ConnectionStatus::Unknown)
{
}

HotspotItem::~HotspotItem()
{
}

void HotspotItem::setConnectionStatus(const ConnectionStatus &status)
{
    m_connectionStatus = status;
}

QString HotspotItem::name() const
{
    return connection()->id();
}

WirelessDevice *HotspotItem::device() const
{
    return m_device;
}

QString HotspotItem::devicePath() const
{
    return m_devicePath;
}

ConnectionStatus HotspotItem::connectionStatus() const
{
    return m_connectionStatus;
}
