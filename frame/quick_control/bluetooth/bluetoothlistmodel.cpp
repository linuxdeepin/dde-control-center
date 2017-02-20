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
    connect(m_bluetoothModel, &BluetoothModel::adapterRemoved,this,[=](const Adapter *adapter){
        m_adapterList.remove(adapter);
        emit layoutChanged();
    });
    connect(m_bluetoothModel, &BluetoothModel::adpaterListChanged, this,[=]{
        emit layoutChanged();
    });

    QTimer::singleShot(1, this, [=] {
        for (const Adapter *adapter : m_bluetoothModel->adapters()) {
            onAdapterAdded(adapter);
        }
    });
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
        if (!info.info)
            return info.device->name();
        else
            return info.info->value("Alias");
    }
    case Qt::SizeHintRole:
    {
        if (!info.info)
            return QSize(0, 20);
        else
            return QSize(0, 30);
    }
    case ItemInfoRole:
    {
        Q_ASSERT(info.info);
        return *info.info;
    }
    case  ItemHoveredRole:
        return index == m_currentIndex;
    case ItemIsHeaderRole:
        return info.info == nullptr;
    default:;
    }

    return QVariant();
}

QMap<const dcc::bluetooth::Adapter *, QList<QJsonObject> > BluetoothListModel::adapterList()
{
    return m_adapterList;
}

void BluetoothListModel::setCurrentHovered(const QModelIndex &index)
{
    const QModelIndex oldIndex = m_currentIndex;

    m_currentIndex = index;

    emit dataChanged(oldIndex, oldIndex);
    emit dataChanged(m_currentIndex, m_currentIndex);
}

void BluetoothListModel::onAdapterAdded(const dcc::bluetooth::Adapter *adapter)
{
    if (m_adapterList.contains(adapter))
        return;

    m_adapterList.insert(adapter, QList<QJsonObject>());

    connect(adapter, &Adapter::deviceAdded, this, &BluetoothListModel::onDeviceApAdded);
    connect(adapter, &Adapter::poweredChanged, this,[=]{emit layoutChanged();});
    connect(adapter, &Adapter::nameChanged, this,[=]{emit layoutChanged();});
    connect(adapter, &Adapter::deviceRemoved, this,[=]{emit layoutChanged();});

    for (const Device *device : adapter->devices()) {
        onDeviceApAdded(device);
    }
}

void BluetoothListModel::onDeviceApAdded(const dcc::bluetooth::Device *device)
{
    Adapter *adapter = static_cast<Adapter *>(sender());

    Q_ASSERT(adapter);
    Q_ASSERT(m_adapterList.contains(adapter));

    connect(device, &Device::stateChanged,this,[=](const Device::State state){
        if (state == Device::StateConnected) {
            onDeviceAdded(adapter, device);
        } else {
            onDeviceRemove(adapter, device);
        }
    });

    if (!device->paired())
        return;

    onDeviceAdded(adapter, device);
}

void BluetoothListModel::onDeviceAdded(const dcc::bluetooth::Adapter *adapter, const dcc::bluetooth::Device *device)
{
    QJsonObject json;
    json.insert("Path", device->id());
    json.insert("Alias", device->name());
    json.insert("Paired", device->paired());
    json.insert("State", device->state());

    int pos = 0;
    for (const Adapter *d : m_bluetoothModel->adapters())
    {
        if (d == adapter)
            break;

        pos += 1;
    }

    beginInsertRows(QModelIndex(), pos, pos);
    m_adapterList[adapter].append(json);
    endInsertRows();

    connect(device, &Device::nameChanged,this,[=]{
        emit layoutChanged();
    });

    connect(device, &Device::pairedChanged,this,[=]{
        emit layoutChanged();
    });

    connect(device, &Device::trustedChanged,this,[=]{
        emit layoutChanged();
    });

    connect(device, &Device::connectingChanged,this,[=]{
        emit layoutChanged();
    });

    emit layoutChanged();
}

void BluetoothListModel::onDeviceRemove(const dcc::bluetooth::Adapter *adapter, const dcc::bluetooth::Device *device)
{
    for (const QJsonObject json : m_adapterList[adapter]) {
        if (json["Path"].toString() == device->id()) {
            m_adapterList[adapter].removeOne(json);
            emit layoutChanged();
            return;
        }
    }
}

const ItemInfo BluetoothListModel::indexInfo(const int index) const
{
    ItemInfo info;
    int r = index;

    for (const Adapter *adapter : m_bluetoothModel->adapters())
    {
        if (!m_adapterList.contains(adapter))
            continue;

        info.device = adapter;


        if (!r)
            return info;
        else
            --r;

        const int s = m_adapterList[adapter].size();
        if (r < s)
        {
            info.info = &m_adapterList[adapter][r];
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
