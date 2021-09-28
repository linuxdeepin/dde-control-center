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

#include "vpncontroller.h"
#include "networkconst.h"
#include "utils.h"

#include "networkmanagerqt/manager.h"

using namespace dde::network;
using namespace NetworkManager;

void VPNController::connectItem(VPNItem *item)
{
    connectItem(item->connection()->uuid());
}

void VPNController::connectItem(const QString &uuid)
{
    m_networkInter->ActivateConnection(uuid, QDBusObjectPath("/"));
}

void VPNController::disconnectItem()
{
    if (!m_activePath.isEmpty()) {
        QDBusPendingReply<> reply = deactivateConnection(m_activePath);
        reply.waitForFinished();
    }
}

VPNController::VPNController(NetworkInter *networkInter, QObject *parent)
    : QObject(parent)
    , m_networkInter(networkInter)
    , m_enabled(false)
{
    Q_ASSERT(m_networkInter);
    connect(m_networkInter, &NetworkInter::VpnEnabledChanged, this, &VPNController::onEnableChanged);
    onEnableChanged(m_networkInter->vpnEnabled());
}

VPNController::~VPNController()
{
}

void VPNController::updateVPNItems(const QJsonArray &vpnArrays)
{
    // 更新列表中的VPN状态
    QList<VPNItem *> newVpns;
    QList<VPNItem *> changeVpns;
    QStringList paths;
    for (const QJsonValue jsonValue : vpnArrays) {
        QJsonObject vpn = jsonValue.toObject();
        QString path = vpn.value("Path").toString();
        VPNItem *item = findItem(path);
        if (!item) {
            item = new VPNItem;
            item->setConnection(vpn);
            m_vpnItems << item;
            newVpns << item;
        } else {
            if (item->connection()->id() != vpn.value("Id").toString())
                changeVpns << item;

            item->setConnection(vpn);
        }

        paths << path;
    }

    if (changeVpns.size())
        Q_EMIT itemChanged(changeVpns);

    if (newVpns.size())
        Q_EMIT itemAdded(newVpns);

    // 将列表中不存在的VPN删除
    QList<VPNItem *> rmVpns;
    for (VPNItem *item : m_vpnItems) {
        if (!paths.contains(item->connection()->path()))
            rmVpns << item;
    }

    for (VPNItem *item : rmVpns)
        m_vpnItems.removeOne(item);

    if (rmVpns.size() > 0)
        Q_EMIT itemRemoved(rmVpns);

    // 删除移除的对象
    for (VPNItem *item : rmVpns)
        delete item;
}

void VPNController::updateActiveConnection(const QJsonObject &activeConection)
{
    // 先将所有的连接都置为未连接状态
    QMap<QString, ConnectionStatus> allConnectionStatus;
    for (VPNItem *item : m_vpnItems) {
        allConnectionStatus[item->connection()->uuid()] = item->status();
        item->setConnectionStatus(ConnectionStatus::Deactivated);
    }

    // 获取当前所有的活动连接，并记录活动连接是否发生过变化
    m_activePath.clear();
    bool statusChanged = false;
    QStringList keys = activeConection.keys();
    for (QString key : keys) {
        QJsonObject json = activeConection.value(key).toObject();
        VPNItem *vpnItem = findItemByUuid(json.value("Uuid").toString());
        if (vpnItem) {
            ConnectionStatus state = convertConnectionStatus(json.value("State").toInt());
            vpnItem->setConnectionStatus(state);
            if (state == ConnectionStatus::Activated)
                m_activePath = key;

            QString uuid = vpnItem->connection()->uuid();
            if (allConnectionStatus.contains(uuid)) {
                ConnectionStatus oldStatus = allConnectionStatus[uuid];
                if (!statusChanged && oldStatus != state)
                    statusChanged = true;
            }
        }
    }

    if (statusChanged)
        Q_EMIT activeConnectionChanged();
}

VPNItem *VPNController::findItem(const QString &path)
{
    for (VPNItem *item : m_vpnItems) {
        if (item->connection()->path() == path)
            return item;
    }

    return Q_NULLPTR;
}

VPNItem *VPNController::findItemByUuid(const QString &uuid)
{
    for (VPNItem *item : m_vpnItems) {
        if (item->connection()->uuid() == uuid)
            return item;
    }

    return Q_NULLPTR;
}

void VPNController::onEnableChanged(const bool enabled)
{
    m_enabled = enabled;
    Q_EMIT enableChanged(enabled);
}

/**
 * @brief UVPNItem详细项
 */

VPNItem::VPNItem()
    : ControllItems()
    , m_connectionStatus(ConnectionStatus::Deactivated)
{
}

VPNItem::~VPNItem()
{
}

void VPNItem::setConnectionStatus(ConnectionStatus connectionStatus)
{
    m_connectionStatus = connectionStatus;
}

ConnectionStatus VPNItem::status() const
{
    return m_connectionStatus;
}
