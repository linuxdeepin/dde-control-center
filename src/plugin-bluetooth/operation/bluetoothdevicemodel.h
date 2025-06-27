//SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef BLUETOOTHDEVICEMODEL_H
#define BLUETOOTHDEVICEMODEL_H

#include <QAbstractListModel>

#include "bluetoothdevice.h"

class BluetoothDeviceModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum deviceRoles {
        Name = Qt::UserRole + 1,
        Id,
        Visiable,
        IconName,
        ConnectStatus,
        ConnectStatusText,
        AdapterId,
        Battery,
        BatteryIconPath,
        CanSendFile,
        Address
    };


    explicit BluetoothDeviceModel(QObject *parent = nullptr);

    void addDevice(BluetoothDevice* device);

    void removeDevice(const QString &deviceId);

    int deviceIndex(const QString &deviceId);

    bool containDevice(BluetoothDevice* device);

    void updateData(BluetoothDevice* device);

    void updateAllData();

    void moveToTop(const QString &deviceId);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void insertItem(int index, BluetoothDevice* device);

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        roles[Name] = "name";
        roles[Id] = "id";
        roles[Visiable] = "visiable";
        roles[IconName] = "iconName";
        roles[ConnectStatus] = "connectStatus";
        roles[ConnectStatusText] = "connectStatusText";
        roles[AdapterId] = "adapterId";
        roles[Battery] = "battery";
        roles[BatteryIconPath] = "batteryIconPath";
        roles[CanSendFile] = "canSendFile";
        roles[Address] = "address";
        return roles;
    }

    QString adapterId() const;
    void setAdapterId(const QString &newAdapterId);

    bool displaySwitch() const;
    void setDisplaySwitch(bool newDisplaySwitch);

    QString getConnectStatusText(int state, bool connectState) const;
    QString getBatteryIconPath(int percentage) const;
    int getConnectStatus(BluetoothDevice::State state, bool connectState) const;

private:
    QString m_adapterId;
    bool m_displaySwitch;
    QList<BluetoothDevice*> m_deviceData;
};

#endif // BLUETOOTHDEVICEMODEL_H
