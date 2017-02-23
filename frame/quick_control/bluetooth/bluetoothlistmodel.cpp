/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "bluetoothlistmodel.h"
#include "bluetooth/device.h"
#include <QSize>
#include <QTimer>

BluetoothListModel::BluetoothListModel(BluetoothModel *model, QObject *parent)
    : QAbstractListModel(parent),
      m_bluetoothModel(model)
{
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
        count += it.value().size() + 1;
    }

    return count;
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
        if (!info.device)
            return info.adapter->name();
        else
            return info.device->name();
    }
    case Qt::SizeHintRole:
    {
        if (!info.device)
            return QSize(0, 20);
        else
            return QSize(0, 30);
    }
    case ItemConnectedRole:
    {
        Q_ASSERT(info.device);
        return info.device->state() == Device::StateConnected;
    }
    case  ItemHoveredRole:
        return index == m_currentIndex;
    case ItemIsHeaderRole:
        return info.device == nullptr;
    case ItemAdapterRole:
        return info.device->id();
    case ItemDeviceRole:
        return QVariant::fromValue(info);
    default:;
    }

    return QVariant();
}

void BluetoothListModel::setCurrentHovered(const QModelIndex &index)
{
    m_currentIndex = index;
}

void BluetoothListModel::onAdapterAdded(const dcc::bluetooth::Adapter *adapter)
{
    if (m_adapterList.contains(adapter))
        return;

    m_adapterList.insert(adapter, QList<const Device *>());

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
    Adapter *adapter = static_cast<Adapter*>(sender());

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
    Adapter *adapter = static_cast<Adapter*>(sender());

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
}

void BluetoothListModel::onDevicePairedChanged()
{
    const Device *device = static_cast<Device *>(sender());
    if (!device->paired())
        onDeviceRemove(device->id());
    else
        onDeviceAdded(device);
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

    qDebug() << index;
    qDebug() << rowCount(QModelIndex());
    qDebug() << m_adapterList.count();

    Q_UNREACHABLE();

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
