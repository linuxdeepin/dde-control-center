/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef BLUETOOTHLISTMODEL_H
#define BLUETOOTHLISTMODEL_H

#include "bluetooth/bluetoothmodel.h"
#include "bluetooth/adapter.h"
#include "bluetooth/device.h"

#include <QAbstractListModel>
#include <QJsonObject>
#include <QMap>
#include <QList>

using dcc::bluetooth::BluetoothModel;
using dcc::bluetooth::Adapter;
using dcc::bluetooth::Device;

struct ItemInfo
{
    const dcc::bluetooth::Adapter *device = nullptr;
    const QJsonObject *info = nullptr;
};

class BluetoothListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit BluetoothListModel(BluetoothModel *model, QObject *parent = 0);
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

private slots:
    void onAdapterAdded(const Adapter *adapter);
    void onDeviceApAdded(const Device *device);

private:
    const ItemInfo indexInfo(const int index) const;

private:
    BluetoothModel *m_bluetoothModel;
    QMap<const Adapter*, QList<QJsonObject>> m_adapterList;
};

#endif // BLUETOOTHLISTMODEL_H
