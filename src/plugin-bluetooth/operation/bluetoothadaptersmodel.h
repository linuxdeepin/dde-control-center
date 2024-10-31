//SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef BLUETOOTHADAPTERSMODEL_H
#define BLUETOOTHADAPTERSMODEL_H

#include <QAbstractListModel>

#include "bluetoothadapter.h"

class BlueToothAdaptersModel : public QAbstractListModel
{
    Q_OBJECT

public:

    enum bluethoothAdapterRoles {
        Name = Qt::UserRole + 1,
        Id,
        Powered,
        Discovering,
        Discoverabled,
        NameDetail,
        MyDevice,
        OtherDevice,
        MyDeviceVisiable,
        OtherDeviceVisiable
    };

    explicit BlueToothAdaptersModel(QObject *parent = nullptr);

    void updateAdapter(BluetoothAdapter* adapter);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void addAdapter(BluetoothAdapter* adapter);

    void removeAdapter(const QString &adapterId);
    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        roles[Name] = "name";
        roles[Id] = "id";
        roles[Powered] = "powered";
        roles[Discovering] = "discovering";
        roles[Discoverabled] = "discoverabled";
        roles[NameDetail] = "nameDetail";
        roles[MyDevice] = "myDevice";
        roles[OtherDevice] = "otherDevice";
        roles[MyDeviceVisiable] = "myDeviceVisiable";
        roles[OtherDeviceVisiable] = "otherDeviceVisiable";
        return roles;
    }

 public Q_SLOTS:
    void setDisplaySwitch(bool newDisplaySwitch);
    void onUpdateAdapter(const QString & adapterId);

private:
    QList<BluetoothAdapter*> m_bluetoothAdapterList;
};

#endif // BLUETOOTHADAPTERSMODEL_H
