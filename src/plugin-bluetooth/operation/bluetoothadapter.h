//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCC_BLUETOOTH_ADAPTER_H
#define DCC_BLUETOOTH_ADAPTER_H

#include <QObject>

#include "bluetoothdevice.h"
#include "bluetoothdevicemodel.h"

class QJsonObject;
class BluetoothDBusProxy;

class BluetoothAdapter : public QObject
{
    Q_OBJECT
public:
    explicit BluetoothAdapter(BluetoothDBusProxy *proxy, QObject *parent = 0);

    inline QString name() const { return m_name; }
    void setName(const QString &name);

    inline QString id() const { return m_id; }
    void setId(const QString &id);

    QMap<QString, const BluetoothDevice *> devices() const;
    QList<QString> devicesId() const;
    const BluetoothDevice *deviceById(const QString &id) const;

    inline bool powered() const { return m_powered; }
    void setPowered(bool powered, bool discovering);
    void setAdapterPowered(const bool &powered);

    inline bool discoverabled() const { return m_discoverable; }
    void setDiscoverabled(const bool discoverable);

    inline bool discovering() const { return m_discovering; }

    void inflate(const QJsonObject &obj);
    void inflateDevice(BluetoothDevice *device, const QJsonObject &deviceObj);

    BluetoothDeviceModel *myDevices() const;

    BluetoothDeviceModel *otherDevices() const;

    void updateDeviceData(BluetoothDevice *device);
    void setdisplaySwitch(bool displaySwitch);

    bool myDeviceVisible() const;
    void setMyDeviceVisible(bool newMyDeviceVisible);

    bool otherDeviceVisible() const;
    void setOtherDeviceVisible(bool newOtherDeviceVisible);

public Q_SLOTS:
    void addDevice(const BluetoothDevice *device);
    void removeDevice(const QString &deviceId);

    void onGetDevices(QString replyStr);

Q_SIGNALS:
    void nameChanged(const QString &name) const;
    void deviceAdded(const BluetoothDevice *device) const;
    void deviceRemoved(const QString &deviceId) const;
    void poweredChanged(const bool &powered, const bool &discovering) const;
    void loadStatus() const;
    void discoverableChanged(const bool &discoverable) const;
    void closeDetailPage() const;
    void myDeviceVisibleChanged(const QString &id);

private Q_SLOTS:
    void onClearUnpairedDevice();
    void onSetAdapterPowered();
    void onSetAdapterPoweredError();
    void onDeviceStateChanged();

private:
    QString m_id;
    QString m_name;
    bool m_powered;
    bool m_discovering;
    bool m_discoverable;
    QMap<QString, const BluetoothDevice *> m_devices;

    BluetoothDeviceModel* m_myDevices;
    BluetoothDeviceModel* m_otherDevices;

    bool m_myDeviceVisible = false;
    bool m_otherDeviceVisible = false;

    //按序存放设备id,确定设备显示顺序
    QList<QString> m_devicesId;
    BluetoothDBusProxy *m_bluetoothDBusProxy;
};

#endif // DCC_BLUETOOTH_ADAPTER_H
