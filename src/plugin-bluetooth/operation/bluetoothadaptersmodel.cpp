//SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "bluetoothadaptersmodel.h"

BlueToothAdaptersModel::BlueToothAdaptersModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

void BlueToothAdaptersModel::updateAdapter(BluetoothAdapter *adapter)
{
    for(int index = 0; index < m_bluetoothAdapterList.count(); index++ ) {
        const BluetoothAdapter *item = m_bluetoothAdapterList.at(index);
        if (item->id() == adapter->id()) {
            QModelIndex modelIndex = createIndex(index, 0);
            qDebug() << "updateAdapter data: " << adapter->name() << adapter->discovering();
            emit dataChanged(modelIndex, modelIndex, {});
            return;
        }
    }
    addAdapter(adapter);
}

int BlueToothAdaptersModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return m_bluetoothAdapterList.count();
}

QVariant BlueToothAdaptersModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() < 0 || index.row() >= m_bluetoothAdapterList.count())
        return QVariant();

    const BluetoothAdapter* bluetoothAdapterData = m_bluetoothAdapterList[index.row()];

    switch (role) {
    case  Name:
        return bluetoothAdapterData->name();
    case Id:
        return bluetoothAdapterData->id();
    case Powered:
        return bluetoothAdapterData->powered();
    case Discovering:
        return bluetoothAdapterData->discovering();
    case Discoverabled:
        return bluetoothAdapterData->discoverabled();
    case NameDetail:
        return bluetoothAdapterData->powered() ? tr("Bluetooth is turned on, and the name is displayed as \"%1\"").arg(bluetoothAdapterData->name()) : tr("Bluetooth is turned off, and the name is displayed as \"%1\"").arg(bluetoothAdapterData->name()) ;
    case MyDevice:
        return QVariant::fromValue(bluetoothAdapterData->myDevices());
    case OtherDevice:
        return QVariant::fromValue(bluetoothAdapterData->otherDevices());
    case MyDeviceVisiable:
        return bluetoothAdapterData->myDeviceVisible() && bluetoothAdapterData->powered();
    case OtherDeviceVisiable:
        return bluetoothAdapterData->otherDeviceVisible();
    default:
        break;
    }
    return QVariant();
}

void BlueToothAdaptersModel::addAdapter(BluetoothAdapter* adapter)
{
    int row = rowCount();
    beginInsertRows(QModelIndex(), row, row);
    m_bluetoothAdapterList.append(adapter);
    endInsertRows();

    connect(adapter, &BluetoothAdapter::myDeviceVisibleChanged, this, &BlueToothAdaptersModel::onUpdateAdapter);
}

void BlueToothAdaptersModel::removeAdapter(const QString &adapterId)
{
    for(int index = 0; index < m_bluetoothAdapterList.count(); index++ ) {
        const BluetoothAdapter *item = m_bluetoothAdapterList.at(index);
        if (item->id() == adapterId) {
            removeRows(index, 1);
            return;
        }
    }
}

bool BlueToothAdaptersModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!


    endInsertRows();
    return true;
}

bool BlueToothAdaptersModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    disconnect(m_bluetoothAdapterList[row], &BluetoothAdapter::myDeviceVisibleChanged, this, &BlueToothAdaptersModel::onUpdateAdapter);
    m_bluetoothAdapterList.remove(row);
    endRemoveRows();
    return true;
}

void BlueToothAdaptersModel::setDisplaySwitch(bool newDisplaySwitch)
{
    for (auto item : m_bluetoothAdapterList) {
        item->setdisplaySwitch(newDisplaySwitch);
    }
}

void BlueToothAdaptersModel::onUpdateAdapter(const QString &adapterId)
{
    for(int index = 0; index < m_bluetoothAdapterList.count(); index++ ) {
        const BluetoothAdapter *item = m_bluetoothAdapterList.at(index);
        if (item->id() == adapterId) {
            QModelIndex modelIndex = createIndex(index, 0);
            emit dataChanged(modelIndex, modelIndex, {});
            return;
        }
    }
}
