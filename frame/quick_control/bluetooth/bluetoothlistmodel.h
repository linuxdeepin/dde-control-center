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
#include <QTimer>

using dcc::bluetooth::BluetoothModel;
using dcc::bluetooth::Adapter;
using dcc::bluetooth::Device;

struct ItemInfo
{
    const dcc::bluetooth::Adapter *adapter = nullptr;
    const Device *device = nullptr;
};

class BluetoothListModel : public QAbstractListModel
{
    Q_OBJECT
public:

    enum BluetoothListRole {
        UnusedRole = Qt::UserRole,
        ItemHoveredRole,
        ItemIsHeaderRole,
        ItemConnectedRole,
        ItemAdapterRole,
        ItemDeviceRole,
        ItemConnectingRole,
        ItemIsSettingRole,
        ItemNextRole,
        ItemIsPowerOffRole,
        ItemLastRole,
        ItemRefreshRole,
        ItemCountRole,
    };

    explicit BluetoothListModel(BluetoothModel *model, QObject *parent = 0);
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    void refreshData();

public slots:
    void setCurrentHovered(const QModelIndex &index);

private slots:
    void onAdapterAdded(const Adapter *adapter);
    void onAdapterRemove(const dcc::bluetooth::Adapter *adapter);
    void onAdapterChanged();
    void onDeviceAdded(const dcc::bluetooth::Device *device);
    void onDeviceRemove(const QString &deviceId);
    void onDeviceChanged(const dcc::bluetooth::Adapter * const adapter, const dcc::bluetooth::Device * const device);
    void onDevicePairedChanged();
    void refershConnectAnimation();
    void onAdapterPowerChanged(const bool powered);

private:
    const ItemInfo indexInfo(const int index) const;
    int indexof(const dcc::bluetooth::Adapter * const adapter) const;
    const Adapter * adapterById(const QString &id);
    const Adapter * findAdapter(const QString &id);

private:
    BluetoothModel *m_bluetoothModel;
    QMap<const Adapter*, QList<const Device*>> m_adapterList;
    QModelIndex m_currentIndex;
    QTimer *m_connectTimer;
    QModelIndex m_activeIndex;
    QTimer *m_refreshTimer;
};

Q_DECLARE_METATYPE(ItemInfo)

#endif // BLUETOOTHLISTMODEL_H
