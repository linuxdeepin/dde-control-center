//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef BLUETOOTHDEVICEMODEL_H
#define BLUETOOTHDEVICEMODEL_H

#include <QAbstractItemModel>

#include <DStyledItemDelegate>

class QPainter;
class QStyleOptionViewItem;
class QModelIndex;
class BluetoothDevice;
class BluetoothAdapter;
struct BluetoothDeviceItemAction;

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
    QIcon getBatteryIcon(int percentage);

private Q_SLOTS:
    void addDevice(const BluetoothDevice *device);
    void removeDevice(const QString &deviceId);
    void updateData();
    void onPairedChanged(const bool &paired);

private:
    void updateItem(BluetoothDeviceItemAction *item);

private:
    const static QMap<QString, QString> deviceType2Icon;
    bool m_paired;
    QList<BluetoothDeviceItemAction *> m_allData;
    QList<BluetoothDeviceItemAction *> m_data;
    const BluetoothAdapter *m_adapter;
    QWidget *m_parent;
    bool m_showAnonymous;
};

class BluetoothDeviceDelegate : public Dtk::Widget::DStyledItemDelegate
{
public:
    BluetoothDeviceDelegate(QAbstractItemView *parent = nullptr);

protected:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // BLUETOOTHDEVICEMODEL_H
