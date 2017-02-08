/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "bluetoothlistmodel.h"
#include <QSize>

BluetoothListModel::BluetoothListModel(BluetoothModel *model, QObject *parent)
    : QAbstractListModel(parent),
      m_bluetoothModel(model)
{
    connect(m_bluetoothModel, &BluetoothModel::adapterAdded, this, &BluetoothListModel::onAdapterAdded);
}

int BluetoothListModel::rowCount(const QModelIndex &parent) const
{

    Q_UNUSED(parent)

    int count = 0;
    for (const auto &list : m_adapterList)
        count += list.size() + 1;

    return count;
}

QVariant BluetoothListModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
    {
        const ItemInfo info = indexInfo(index.row());

        if (!info.info)
            return info.device->id();
        else
            return info.info->value("Alias");
    }
    case Qt::SizeHintRole:
        return QSize(0, 30);
    default:;
    }

    return QVariant();
}

void BluetoothListModel::onAdapterAdded(const dcc::bluetooth::Adapter *adapter)
{
    if (m_adapterList.contains(adapter))
        return;

    m_adapterList.insert(adapter, QList<QJsonObject>());

    connect(adapter, &Adapter::deviceAdded, this, &BluetoothListModel::onDeviceApAdded);
}

void BluetoothListModel::onDeviceApAdded(const dcc::bluetooth::Device *device)
{
    Adapter *adapter = static_cast<Adapter *>(sender());

    Q_ASSERT(adapter);
    Q_ASSERT(m_adapterList.contains(adapter));

    if (!device->paired())
        return;

    QJsonObject json;
    json.insert("Path", device->id());
    json.insert("Alias", device->name());
    json.insert("Paired", device->paired());

    m_adapterList[adapter].append(json);
    emit layoutChanged();
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

    Q_UNREACHABLE();

    return info;
}
