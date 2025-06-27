//SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "bluetoothdevicemodel.h"

BluetoothDeviceModel::BluetoothDeviceModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_displaySwitch(false)
{
}

void BluetoothDeviceModel::addDevice(BluetoothDevice *device)
{
    int row = rowCount();
    beginInsertRows(QModelIndex(), row, row);
    m_deviceData.append(device);
    endInsertRows();
}

void BluetoothDeviceModel::removeDevice(const QString &deviceId)
{
    for (int index = 0; index < m_deviceData.count(); index++) {
        const BluetoothDevice *item = m_deviceData.at(index);
        if (item->id() == deviceId) {
            removeRows(index, 1);
            return;
        }
    }
}

int BluetoothDeviceModel::deviceIndex(const QString &deviceId)
{
    for (int index = 0; index < m_deviceData.count(); index++) {
        if (m_deviceData.at(index)->id() == deviceId) {
            return index;
        }
    }

    return -1;
}

bool BluetoothDeviceModel::containDevice(BluetoothDevice *device)
{
    return m_deviceData.contains(device);
}

void BluetoothDeviceModel::updateData(BluetoothDevice *device)
{
    for (int index = 0; index < m_deviceData.count(); index++) {
        const BluetoothDevice *item = m_deviceData.at(index);
        if (item->id() == device->id()) {
            bool isConnected = (device->state() == BluetoothDevice::StateConnected && device->connectState());
            if (isConnected && index > 0) {
                moveToTop(device->id());
            } else {
                QModelIndex modelIndex = createIndex(index, 0);
                emit dataChanged(modelIndex, modelIndex, {});
            }
            return;
        }
    }
    qDebug() << " updateAdapter new device : " << device->id();
    addDevice(device);
}

void BluetoothDeviceModel::updateAllData()
{
    for (int index = 0; index < m_deviceData.count(); index++) {
        QModelIndex modelIndex = createIndex(index, 0);
        emit dataChanged(modelIndex, modelIndex, {});
    }
}

void BluetoothDeviceModel::moveToTop(const QString &deviceId)
{
    int currentIndex = deviceIndex(deviceId);
    if (currentIndex <= 0) {
        return;
    }
    
    beginMoveRows(QModelIndex(), currentIndex, currentIndex, QModelIndex(), 0);
    BluetoothDevice* device = m_deviceData.takeAt(currentIndex);
    m_deviceData.prepend(device);
    endMoveRows();
}

int BluetoothDeviceModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return m_deviceData.count();
}

QVariant BluetoothDeviceModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_deviceData.count())
        return QVariant();

    const BluetoothDevice* bluetoothDeviceData = m_deviceData[index.row()];

    switch (role) {

    case Name:
        return bluetoothDeviceData->alias().isEmpty() ? bluetoothDeviceData->name() : bluetoothDeviceData->alias();

    case Id:
        return bluetoothDeviceData->id();
    case Visiable:
        return displaySwitch() ? true : !bluetoothDeviceData->name().isEmpty();
    case IconName:
        return bluetoothDeviceData->deviceType();
    case ConnectStatusText:
        return getConnectStatusText((int)bluetoothDeviceData->state(), bluetoothDeviceData->connectState());
    case ConnectStatus:
        return getConnectStatus(bluetoothDeviceData->state(), bluetoothDeviceData->connectState());
    case AdapterId:
        return adapterId();
    case Battery:
        return QString("%1%").arg(bluetoothDeviceData->battery());
    case BatteryIconPath:
        return getBatteryIconPath(bluetoothDeviceData->battery());
    case CanSendFile:
        return bluetoothDeviceData->canSendFile();
    case Address:
        return bluetoothDeviceData->address();
    default:
        break;
    }
    return QVariant();
}

void BluetoothDeviceModel::insertItem(int index, BluetoothDevice *device)
{
    if (index < 0 || index > m_deviceData.size()) return; // 确保索引有效
    beginInsertRows(QModelIndex(), index, index);
    m_deviceData.insert(index, device);
    endInsertRows();
}

bool BluetoothDeviceModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    m_deviceData.remove(row);
    endRemoveRows();
    return true;
}

QString BluetoothDeviceModel::adapterId() const
{
    return m_adapterId;
}

void BluetoothDeviceModel::setAdapterId(const QString &newAdapterId)
{
    m_adapterId = newAdapterId;
}

bool BluetoothDeviceModel::displaySwitch() const
{
    return m_displaySwitch;
}

void BluetoothDeviceModel::setDisplaySwitch(bool newDisplaySwitch)
{
    m_displaySwitch = newDisplaySwitch;
}

QString BluetoothDeviceModel::getConnectStatusText(int state, bool connectState) const
{
    if (state == BluetoothDevice::StateConnected && connectState) {
       return tr("Connected");
    } else if (state == BluetoothDevice::StateUnavailable || state == BluetoothDevice::StateDisconnecting) {
       return tr("Not connected");
    }

    return "";
}

QString BluetoothDeviceModel::getBatteryIconPath(int percentage) const
{
    /* 0-5%、6-10%、11%-20%、21-30%、31-40%、41-50%、51-60%、61%-70%、71-80%、81-90%、91-100% */
    QString percentageStr;
    if (percentage <= 5) {
        percentageStr = "000";
    } else if (percentage <= 10) {
        percentageStr = "010";
    } else if (percentage <= 20) {
        percentageStr = "020";
    } else if (percentage <= 30) {
        percentageStr = "030";
    } else if (percentage <= 40) {
        percentageStr = "040";
    } else if (percentage <= 50) {
        percentageStr = "050";
    } else if (percentage <= 60) {
        percentageStr = "060";
    } else if (percentage <= 70) {
        percentageStr = "070";
    } else if (percentage <= 80) {
        percentageStr = "080";
    } else if (percentage <= 90) {
        percentageStr = "090";
    } else if (percentage <= 100) {
        percentageStr = "100";
    } else {
        percentageStr = "unknow";
    }

    QString iconName = QString("battery-%1-symbolic").arg(percentageStr);
    return "qrc:/icons/deepin/builtin/icons/" + iconName + "_20px.svg";
}

int BluetoothDeviceModel::getConnectStatus(BluetoothDevice::State state, bool connectState) const
{

    if (state == BluetoothDevice::StateConnected) {
        return connectState ? state : BluetoothDevice::StateUnavailable;
    }

    return state;
}
