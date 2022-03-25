/*
* Copyright (C) 2021 ~ 2023 Deepin Technology Co., Ltd.
*
* Author:     caixiangrong <caixiangrong@uniontech.com>
*
* Maintainer: caixiangrong <caixiangrong@uniontech.com>
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
#ifndef BLUETOOTHDEVICEMODEL_H
#define BLUETOOTHDEVICEMODEL_H

#include <QAbstractItemModel>

#include <DStyledItemDelegate>

class QPainter;
class QStyleOptionViewItem;
class QModelIndex;
class BluetoothDevice;
class BluetoothAdapter;
struct ItemAction;

class BluetoothDeviceModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit BluetoothDeviceModel(const BluetoothAdapter *adapter, bool paired = false, QWidget *parent = nullptr);
    virtual ~BluetoothDeviceModel() override;
    void setPairedDevices(bool paired);
    QModelIndex index(const BluetoothDevice *device);
    // Basic functionality:
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

Q_SIGNALS:
    void detailClick(const BluetoothDevice *device);
    void requestSetDevAlias(const BluetoothDevice *device, const QString &devAlias);

public Q_SLOTS:
    void showAnonymous(bool show);

private Q_SLOTS:
    void addDevice(const BluetoothDevice *device);
    void removeDevice(const QString &deviceId);
    void updateData();
    void onPairedChanged(const bool &paired);

private:
    void updateItem(ItemAction *item);

private:
    bool m_paired;
    QList<ItemAction *> m_allData;
    QList<ItemAction *> m_data;
    QMap<const BluetoothDevice *, Dtk::Widget::DViewItemActionList> m_mapActionlist;
    const BluetoothAdapter *m_adapter;
    QWidget *m_parent;
    bool m_showAnonymous;
};

#endif // BLUETOOTHDEVICEMODEL_H
