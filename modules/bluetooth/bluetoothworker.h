/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DCC_BLUETOOTH_BLUETOOTHWORKER_H
#define DCC_BLUETOOTH_BLUETOOTHWORKER_H

#include <QObject>

#include <com_deepin_daemon_bluetooth.h>

#include "moduleworker.h"
#include "bluetoothmodel.h"

using  DBusBluetooth = com::deepin::daemon::Bluetooth;

namespace dcc {
namespace bluetooth {

class BluetoothWorker : public QObject, public ModuleWorker
{
    Q_OBJECT
public:
    explicit BluetoothWorker(BluetoothModel *model);

    void activate() Q_DECL_OVERRIDE;
    void deactivate() Q_DECL_OVERRIDE;

signals:
    void deviceEnableChanged();

public slots:
    void setAdapterPowered(const Adapter *adapter, const bool &powered);
    void connectDevice(const Device *device);
    void disconnectDevice(const Device *device);
    void ignoreDevice(const Adapter *adapter, const Device *device);
    void setAlias(const Adapter *adapter, const QString &alias);
    void setAdapterDiscoverable(const QString &path);

private:
    void inflateAdapter(Adapter *adapter, const QJsonObject &adapterObj);
    void inflateDevice(Device *device, const QJsonObject &deviceObj);

private slots:
    void onAdapterPropertiesChanged(const QString &json);
    void onDevicePropertiesChanged(const QString &json);

    void addAdapter(const QString &json);
    void removeAdapter(const QString &json);

    void addDevice(const QString &json);
    void removeDevice(const QString &json);

private:
    DBusBluetooth *m_bluetoothInter;
    BluetoothModel *m_model;
};

} // namespace bluetooth
} // namespace dcc

#endif // DCC_BLUETOOTH_BLUETOOTHWORKER_H
