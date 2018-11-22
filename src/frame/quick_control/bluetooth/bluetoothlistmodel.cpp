/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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

#include "bluetoothlistmodel.h"
#include "bluetooth/device.h"
#include <QSize>
#include <QTimer>

BluetoothListModel::BluetoothListModel(BluetoothModel *model, QObject *parent)
    : QAbstractListModel(parent),
      m_bluetoothModel(model),
      m_connectTimer(new QTimer),
      m_refreshTimer(new QTimer)
{
    m_refreshTimer->setSingleShot(false);
    connect(m_refreshTimer, &QTimer::timeout, this, &BluetoothListModel::refershConnectAnimation);

    connect(m_connectTimer, &QTimer::timeout, this, &BluetoothListModel::refershConnectAnimation);
    connect(m_bluetoothModel, &BluetoothModel::adapterAdded, this, &BluetoothListModel::onAdapterAdded);
    connect(m_bluetoothModel, &BluetoothModel::adapterRemoved,this, &BluetoothListModel::onAdapterRemove);

    QTimer::singleShot(1, this, [=] {
        for (const Adapter *adapter : m_bluetoothModel->adapters()) {
            onAdapterAdded(adapter);
        }
    });

    qRegisterMetaType<ItemInfo>("ItemInfo");
}

int BluetoothListModel::rowCount(const QModelIndex &parent) const
{

    Q_UNUSED(parent)

    int count = 0;
    for (auto it(m_adapterList.cbegin()); it != m_adapterList.cend(); ++it)
    {
        if (!it.key()->powered())
            continue;

        count += it.value().size() + 1;
    }

    return count + 1;
}

QVariant BluetoothListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (rowCount(QModelIndex()) <= index.row())
        return QVariant();

    const ItemInfo info = indexInfo(index.row());

    switch (role) {
    case Qt::DisplayRole:
    {
        if (rowCount(QModelIndex()) == 1)
            return tr("Click icon to enable bluetooth");

        if (!info.device && info.adapter)
            return info.adapter->name();
        else if (!info.adapter)
            return tr("Connect to other devices");
        else
            return info.device->name();
    }
    case Qt::SizeHintRole:
    {
        if (rowCount(QModelIndex()) == 1)
            return QSize(0, 36);
        if (!info.device)
            return QSize(0, 24);
        else
            return QSize(0, 36);
    }
    case ItemConnectedRole:
        Q_ASSERT(info.device);
        return info.device->state() == Device::StateConnected;
    case ItemConnectingRole:
        Q_ASSERT(info.device);
        return info.device->state() == Device::StateAvailable;
    case  ItemHoveredRole:
        return index == m_currentIndex;
    case ItemIsHeaderRole:
        return info.device == nullptr && info.adapter != nullptr;
    case ItemAdapterRole:
        return info.adapter->id();
    case ItemDeviceRole:
        return QVariant::fromValue(info);
    case ItemIsSettingRole:
        return !info.adapter && !info.device;
    case ItemNextRole:
        return m_currentIndex.row() + 1 == index.row();
    case ItemIsPowerOffRole:
        return rowCount(QModelIndex()) == 1;
    case ItemLastRole:
        return rowCount(QModelIndex()) - 2 == index.row();
    case ItemRefreshRole:
        return m_refreshTimer->isActive() && info.device == nullptr && info.adapter != nullptr;
    case ItemCountRole:
        return m_adapterList.keys().count();
    default:;
    }

    return QVariant();
}

void BluetoothListModel::setCurrentHovered(const QModelIndex &index)
{
    m_currentIndex = index;

    emit dataChanged(m_currentIndex, m_currentIndex);
}

void BluetoothListModel::onAdapterAdded(const dcc::bluetooth::Adapter *adapter)
{
    if (m_adapterList.contains(adapter))
        return;

    connect(adapter, &Adapter::poweredChanged, this, &BluetoothListModel::onAdapterPowerChanged, Qt::UniqueConnection);

    if (!adapter->powered())
        return;

    int pos = rowCount(QModelIndex());

    beginInsertRows(QModelIndex(), pos, pos);
    m_adapterList.insert(adapter, QList<const Device *>());
    endInsertRows();

    connect(adapter, &Adapter::deviceAdded, this, &BluetoothListModel::onDeviceAdded);
    connect(adapter, &Adapter::nameChanged, this, &BluetoothListModel::onAdapterChanged);
    connect(adapter, &Adapter::deviceRemoved, this, &BluetoothListModel::onDeviceRemove);
    for (const Device *device : adapter->devices()) {
        onDeviceAdded(device);
    }
}

void BluetoothListModel::onAdapterRemove(const dcc::bluetooth::Adapter *adapter)
{
    const int pos = indexof(adapter);

    beginRemoveRows(QModelIndex(), pos, pos + m_adapterList[adapter].length());
    m_adapterList.remove(adapter);
    endRemoveRows();
}

void BluetoothListModel::onDeviceAdded(const dcc::bluetooth::Device *device)
{
    const Adapter *adapter = adapterById(device->id());

    connect(device, &Device::stateChanged, this, [=]{onDeviceChanged(adapter, device);}, Qt::UniqueConnection);
    connect(device, &Device::pairedChanged, this, &BluetoothListModel::onDevicePairedChanged, Qt::UniqueConnection);
    connect(device, &Device::nameChanged, this, [=]{onDeviceChanged(adapter, device);}, Qt::UniqueConnection);

    if (!device->paired())
        return;

    const int pos = indexof(adapter) + m_adapterList[adapter].count();

    beginInsertRows(QModelIndex(), pos, pos);
    m_adapterList[adapter].append(device);
    endInsertRows();
}

void BluetoothListModel::onDeviceRemove(const QString &deviceId)
{
    const Adapter *adapter = findAdapter(deviceId);

    if (!adapter)
        return;

    const int pos = indexof(adapter) + 1;
    int i;
    for (i = 0; i != m_adapterList[adapter].length(); ++i) {
        if (m_adapterList[adapter][i]->id() == deviceId)
            break;
    }
    beginRemoveRows(QModelIndex(), pos + i, pos + i);
    m_adapterList[adapter].removeAt(i);
    endRemoveRows();
}

void BluetoothListModel::onDeviceChanged(const dcc::bluetooth::Adapter * const adapter, const dcc::bluetooth::Device * const device)
{
    if (!m_adapterList[adapter].contains(device))
        return;

    const int pos = indexof(adapter) + m_adapterList[adapter].indexOf(device) + 1;
    const QModelIndex i = index(pos);
    emit dataChanged(i, i);
    m_activeIndex = i;

    if (device->state() == Device::StateAvailable)
        m_connectTimer->start();
    else
        m_connectTimer->stop();
}

void BluetoothListModel::onDevicePairedChanged()
{
    const Device *device = static_cast<Device *>(sender());
    if (!device->paired())
        onDeviceRemove(device->id());
    else
        onDeviceAdded(device);
}

void BluetoothListModel::refershConnectAnimation()
{
    emit dataChanged(m_activeIndex, m_activeIndex);
}

void BluetoothListModel::onAdapterPowerChanged(const bool powered)
{
    Adapter *adapter = static_cast<Adapter *>(sender());
    if (powered) {
        onAdapterAdded(adapter);
    } else {
        onAdapterRemove(adapter);
    }
}

void BluetoothListModel::refreshData()
{
    m_refreshTimer->start();

    QTimer::singleShot(5000, m_refreshTimer, &QTimer::stop);
}

void BluetoothListModel::onAdapterChanged()
{
    Adapter *adapter = static_cast<Adapter *>(sender());
    const int pos = indexof(adapter);
    const QModelIndex i = index(pos);

    emit dataChanged(i, i);
}


const ItemInfo BluetoothListModel::indexInfo(const int index) const
{
    ItemInfo info;
    int r = index;

    for (const Adapter *adapter : m_bluetoothModel->adapters())
    {
        if (!adapter->powered())
            continue;

        if (!m_adapterList.contains(adapter))
            continue;

        info.adapter = adapter;


        if (!r)
            return info;
        else
            --r;

        const int s = m_adapterList[adapter].size();
        if (r < s)
        {
            info.device = m_adapterList[adapter][r];
            return info;
        } else {
            r -= s;
        }

    }

    info.adapter = nullptr;

    return info;
}

int BluetoothListModel::indexof(const dcc::bluetooth::Adapter * const adapter) const
{
    int pos = 0;
    for (const Adapter *ad : m_bluetoothModel->adapters()) {
        if (ad == adapter)
            break;
        pos += m_adapterList[adapter].count() + 1;
    }
    return pos;
}

const dcc::bluetooth::Adapter *BluetoothListModel::adapterById(const QString &id)
{
    for (const Adapter *adapter : m_bluetoothModel->adapters()) {
        for (const Device *device : adapter->devices())
            if (device->id() == id)
                return adapter;
    }

    Q_UNREACHABLE();
    return nullptr;
}

const dcc::bluetooth::Adapter *BluetoothListModel::findAdapter(const QString &id)
{
    for (const Adapter * adapter : m_adapterList.keys())
        for (const Device *device : m_adapterList[adapter])
            if (device->id() == id)
                return adapter;

    return nullptr;
}
