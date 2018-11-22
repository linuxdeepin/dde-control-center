/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#include "wifilistmodel.h"

#include <QSize>
#include <QDebug>
#include <QTimer>

#include <networkdevice.h>
#include <networkmodel.h>
#include <wirelessdevice.h>

using dde::network::NetworkModel;
using dde::network::NetworkDevice;
using dde::network::WirelessDevice;

WifiListModel::WifiListModel(NetworkModel *model, QObject *parent)
    : QAbstractListModel(parent),

      m_networkModel(model),
      m_refreshTimer(new QTimer(this))
{
    m_refreshTimer->setSingleShot(false);
    m_refreshTimer->setInterval(1000 / 60);

    connect(m_refreshTimer, &QTimer::timeout, this, &WifiListModel::refershActivatingIndex);
    connect(m_networkModel, &NetworkModel::connectionListChanged, [this] { emit layoutChanged(); });
    connect(m_networkModel, &NetworkModel::deviceEnableChanged, [this] { emit layoutChanged(); });
    connect(m_networkModel, &NetworkModel::deviceListChanged, this, &WifiListModel::onDeviceListChanged);

    QTimer::singleShot(1, this, [=] { onDeviceListChanged(m_networkModel->devices()); });
}

int WifiListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    int count = 0;
    for (auto it(m_apInfoList.cbegin()); it != m_apInfoList.cend(); ++it)
    {
        if (!it.key()->enabled())
            continue;
        count += it.value().size() + 1;
    }

    // +1 for "connect to hidden network"
    return count + 1;
}

QVariant WifiListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const ItemInfo info = indexInfo(index.row());

    switch (role) {
    case Qt::DisplayRole:
    {
        if (rowCount(QModelIndex()) == 1)
            return tr("Click icon to enable WIFI");

        if (!info.info && info.device)
            return deviceName(info.device);
        else if (!info.device)
            return tr("Connect to hidden network");
        else
            return info.info->value("Ssid");
    }
    case Qt::SizeHintRole:
        if (rowCount(QModelIndex()) == 1)
            return QSize(0, 36);
        if (!info.info)
            return QSize(0, 24);
        else
            return QSize(0, 36);
    case ItemInfoRole:
        return info.info ? *info.info : QVariant();
    case ItemHoveredRole:
        return index == m_currentIndex;
    case ItemIsHeaderRole:
        return !info.info && info.device;
    case ItemIsActiveRole:
        return info.info && static_cast<const WirelessDevice *>(info.device)->activeConnName() == info.info->value("Ssid").toString();
    case ItemIsActivatingRole:
        return m_refreshTimer->isActive() && info.info && m_activatingSsid == info.info->value("Ssid").toString();
    case ItemDevicePathRole:
        return info.device->path();
    case ItemApPathRole:
        return info.info ? info.info->value("Path") : QVariant();
    case ItemUuidRole:
        return info.info ? m_networkModel->connectionUuidByApInfo(static_cast<const WirelessDevice *>(info.device), info.info->value("Ssid").toString()) : QVariant();
    case ItemIsHiddenTipsRole:
        return !info.info && !info.device;
    case ItemNextRole:
        return m_currentIndex.row() + 1 == index.row();
    case ItemIsPowerOffRole:
        return rowCount(QModelIndex()) == 1;
    case ItemCountRole:
        return m_apInfoList.keys().count();
    default:;
    }

    return QVariant();
}

void WifiListModel::setCurrentHovered(const QModelIndex &index)
{
    m_currentIndex = index;

    emit dataChanged(m_currentIndex, m_currentIndex);
}

void WifiListModel::setCurrentActivating(const QModelIndex &index)
{
    const QModelIndex oldIndex = m_activatingIndex;

    m_activatingIndex = index;
    const ItemInfo &info = indexInfo(index.row());
    if (info.info) {
        m_activatingSsid = info.info->value("Ssid").toString();
    } else {
        m_activatingSsid = QString();
    }

    emit dataChanged(oldIndex, oldIndex);
}

int WifiListModel::indexOf(dde::network::WirelessDevice * const dev) const
{
    int pos = 0;
    for (auto *d : m_networkModel->devices())
    {
        if (d->type() != NetworkDevice::Wireless || !d->enabled())
            continue;

        pos += m_apInfoList[static_cast<WirelessDevice *>(d)].size() + 1;

        if (d == dev)
            break;
    }

    return pos;
}

const ItemInfo WifiListModel::indexInfo(const int index) const
{
    ItemInfo info;
    int r = index;
    for (auto *dev : m_networkModel->devices())
    {
        if (dev->type() != NetworkDevice::Wireless || !dev->enabled())
            continue;

        WirelessDevice *d = static_cast<WirelessDevice *>(dev);
        if (!m_apInfoList.contains(d))
            continue;

        info.device = dev;

        if (!r)
            return info;
        else
            --r;

        const int s = m_apInfoList[d].size();
        if (r < s)
        {
            info.info = &m_apInfoList[d][r];
            return info;
        } else {
            r -= s;
        }
    }

    info.device = nullptr;

    return info;
}

const QString WifiListModel::deviceName(const NetworkDevice *wirelessDevice) const
{
    int index = 1;
    for (const auto *dev : m_networkModel->devices())
    {
        if (!dev->enabled())
            continue;

        if (dev == wirelessDevice)
            break;

        if (dev->type() == NetworkDevice::Wireless)
            ++index;
    }

    return tr("Wireless Card %1").arg(index);
}

void WifiListModel::onDeviceListChanged(const QList<NetworkDevice *> &devices)
{
    for (auto *dev : devices)
    {
        if (dev->type() != NetworkDevice::Wireless || !dev->enabled())
            continue;

        WirelessDevice *d = static_cast<WirelessDevice *>(dev);

        if (m_apInfoList.contains(d))
            continue;

        connect(d, &WirelessDevice::enableChanged, this, &WifiListModel::onDeviceEnableChanged, Qt::UniqueConnection);
        connect(d, &WirelessDevice::activeConnectionChanged, this, &WifiListModel::refershActivatingIndex, Qt::UniqueConnection);
        connect(d, &WirelessDevice::apAdded, this, &WifiListModel::onDeviceApAdded, Qt::UniqueConnection);
        connect(d, &WirelessDevice::apInfoChanged, this, &WifiListModel::onDeviceApInfoChanged, Qt::UniqueConnection);
        connect(d, &WirelessDevice::activeConnectionChanged, this, &WifiListModel::onDeviceActiveApChanged, Qt::UniqueConnection);
        connect(d, static_cast<void (WirelessDevice::*)(const NetworkDevice::DeviceStatus) const>(&WirelessDevice::statusChanged), this, &WifiListModel::onDeviceStateChanged, Qt::UniqueConnection);
        connect(d, &WirelessDevice::apRemoved, d, [=](const QJsonObject &apInfo) { onDeviceApRemoved(d, apInfo); }, Qt::UniqueConnection);

        emit requestDeviceApList(d->path());
    }

    // removed devices
    QList<WirelessDevice *> removedDeviceList;
    for (auto it(m_apInfoList.cbegin()); it != m_apInfoList.cend(); ++it)
        if (!devices.contains(it.key()))
            removedDeviceList.append(it.key());
    for (auto *d : removedDeviceList)
        m_apInfoList.remove(d);

    sortApList();

    emit layoutChanged();
}

void WifiListModel::onDeviceApAdded(const QJsonObject &info)
{
    WirelessDevice *dev = static_cast<WirelessDevice *>(sender());
    Q_ASSERT(dev);

    if (!dev->enabled())
        return;

    const auto &path = info.value("Path").toString();
    const auto &ssid = info.value("Ssid").toString();
    const auto &strength = info.value("Strength").toInt();

    // test if ap already exist.
    int row = indexOf(dev);
    auto &list = m_apInfoList[dev];
    for (int i(0); i != list.size(); ++i) {
        const auto &mAp = list.at(i);
        // same Path means same ap, return
        if (mAp.value("Path").toString() == path)
            return;
        // different Path but use the same Ssid, then compare Theirs Strength
        else if (mAp.value("Ssid").toString() == ssid) {
            if (mAp.value("Strength").toInt() < strength) {
                row += i + 1;
                list.replace(i, info);
                auto changedIndex = index(row);
                Q_EMIT dataChanged(changedIndex, changedIndex);
            }
            return;
        }
    }

    // reach here means it is a new ap need to add
    beginInsertRows(QModelIndex(), row, row);
    if (info.value("Ssid").toString() == dev->activeConnName()) {
        m_activeConnNameMap.insert(dev, dev->activeConnName());
        m_apInfoList[dev].insert(0, info);
    } else {
        m_apInfoList[dev].append(info);
    }
    sortApList();
    endInsertRows();
}

void WifiListModel::onDeviceApInfoChanged(const QJsonObject &info)
{
    WirelessDevice *dev = static_cast<WirelessDevice *>(sender());
    Q_ASSERT(dev);

    if (!dev->enabled())
        return;

    int row = indexOf(dev);
    auto &list = m_apInfoList[dev];
    for (int i(0); i != list.size(); ++i)
    {
        if (list[i].value("Path").toString() == info.value("Path").toString())
        {
            row += i + 1;
            list.replace(i, info);
            auto changedIndex = index(row);
            Q_EMIT dataChanged(changedIndex, changedIndex);
            return;
        }
    }

    // reach here means it is a new ap need to add
    onDeviceApAdded(info);
}

void WifiListModel::onDeviceApRemoved(dde::network::WirelessDevice *dev, const QJsonObject &apInfo)
{
    int row = indexOf(dev);

    const auto list = m_apInfoList[dev];
    for (int i(0); i != list.size(); ++i)
    {
        if (list[i].value("Path").toString() == apInfo.value("Path").toString())
        {
            row += i + 1;
            beginRemoveRows(QModelIndex(), row, row);
            m_apInfoList[dev].removeAt(i);
            endRemoveRows();
            return;
        }
    }
}

void WifiListModel::onDeviceStateChanged(const NetworkDevice::DeviceStatus &stat)
{
//    WirelessDevice *dev = static_cast<WirelessDevice *>(sender());
    if (stat >= NetworkDevice::Prepare && stat < NetworkDevice::Activated)
        m_refreshTimer->start();
    else
        m_refreshTimer->stop();

    Q_EMIT layoutChanged();
}

void WifiListModel::onDeviceActiveApChanged(const QJsonObject &oldApInfo, const QJsonObject &newApInfo)
{
    Q_UNUSED(oldApInfo);

    WirelessDevice *dev = static_cast<WirelessDevice *>(sender());
    Q_ASSERT(dev);

    const QString &activeConnName = newApInfo["ConnectionName"].toString();
    const auto list = m_apInfoList[dev];
    for (int i(0); i != list.size(); ++i)
    {
        if (list[i].value("Ssid").toString() == activeConnName)
        {
            m_activeConnNameMap.insert(dev, activeConnName);

            // pass if already first line
            if (i)
            {
                const auto info = m_apInfoList[dev][i];
//                beginMoveRows(QModelIndex(), i, i, QModelIndex(), 0);
                m_apInfoList[dev].removeOne(info);
                m_apInfoList[dev].insert(0, info);
//                endMoveRows();
            }

            sortApList();
            emit layoutChanged();
            return;
        }
    }
}

void WifiListModel::refershActivatingIndex()
{
    emit dataChanged(m_activatingIndex, m_activatingIndex);
}

void WifiListModel::sortApList()
{
    QString *activeSsid = new QString();

    auto cmpFunc = [=](const QJsonObject &a, const QJsonObject &b) {
        // make sure active ap is the first one of ap list
        bool aIsActive = (a.value("Ssid").toString() == *activeSsid);
        if (aIsActive || b.value("Ssid").toString() == *activeSsid) {
            return aIsActive;
        }
        return a.value("Strength").toInt() > b.value("Strength").toInt();
    };

    for (auto dev : m_apInfoList.keys()) {
        *activeSsid = m_activeConnNameMap.value(dev);
        QList<QJsonObject> list = m_apInfoList.value(dev);
        std::sort(list.begin(), list.end(), cmpFunc);
        m_apInfoList.insert(dev, list);
    }

    delete activeSsid;
}

void WifiListModel::onDeviceEnableChanged(const bool enable)
{
    WirelessDevice *d = static_cast<WirelessDevice*>(sender());
    Q_ASSERT(d);

    if (enable) {
        const int pos = rowCount(QModelIndex());
        beginInsertRows(QModelIndex(), pos, pos);
        m_apInfoList.insert(d, QList<QJsonObject>());
        endInsertRows();

        emit requestDeviceApList(d->path());
    } else {
        const int pos = indexOf(d);
        beginRemoveRows(QModelIndex(), pos, pos + m_apInfoList[d].length());
        m_apInfoList.remove(d);
        endRemoveRows();
    }
}
