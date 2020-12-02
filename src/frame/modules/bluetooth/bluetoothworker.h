/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#ifndef DCC_BLUETOOTH_BLUETOOTHWORKER_H
#define DCC_BLUETOOTH_BLUETOOTHWORKER_H

#include <QObject>

#include <com_deepin_daemon_bluetooth.h>

#include "modules/moduleworker.h"
#include "bluetoothmodel.h"
#include "pincodedialog.h"

using  DBusBluetooth = com::deepin::daemon::Bluetooth;

namespace dcc {
namespace bluetooth {

class BluetoothWorker : public QObject, public ModuleWorker
{
    Q_OBJECT
public:
    static BluetoothWorker &Instance(bool sync = false);

    BluetoothModel *model() { return m_model; }

    void activate() Q_DECL_OVERRIDE;
    void deactivate() Q_DECL_OVERRIDE;

    void blockDBusSignals(bool block);


Q_SIGNALS:
    void deviceEnableChanged();
    void requestConfirmation(const QDBusObjectPath &path, const QString &code);
    void pinCodeCancel(const QDBusObjectPath &device);

public Q_SLOTS:
    void setAdapterPowered(const Adapter *adapter, const bool &powered);
    void connectDevice(const Device *device, const Adapter *adapter);
    void disconnectDevice(const Device *device);
    void ignoreDevice(const Adapter *adapter, const Device *device);
    void setAlias(const Adapter *adapter, const QString &alias);
    void setDeviceAlias(const Device *device, const QString &alias);
    void setAdapterDiscoverable(const QString &path);
    void pinCodeConfirm(const QDBusObjectPath &path, bool value);
    void setAdapterDiscovering(const QDBusObjectPath &path, bool enable);
    void onRequestSetDiscoverable(const Adapter *adapter, const bool &discoverable);

private:
    void inflateAdapter(Adapter *adapter, const QJsonObject &adapterObj);
    void inflateDevice(Device *device, const QJsonObject &deviceObj);

private Q_SLOTS:
    void onAdapterPropertiesChanged(const QString &json);
    void onDevicePropertiesChanged(const QString &json);

    void addAdapter(const QString &json);
    void removeAdapter(const QString &json);

    void addDevice(const QString &json);
    void removeDevice(const QString &json);

    void refresh(bool beFirst = false);

private:
    explicit BluetoothWorker(BluetoothModel *model, bool sync = false);
    BluetoothWorker(BluetoothWorker const &) = delete;
    BluetoothWorker& operator =(BluetoothWorker const &) = delete;
    ~BluetoothWorker();

    DBusBluetooth *m_bluetoothInter;
    BluetoothModel *m_model;
    QMap<QDBusObjectPath, PinCodeDialog*> m_dialogs;
};

} // namespace bluetooth
} // namespace dcc

#endif // DCC_BLUETOOTH_BLUETOOTHWORKER_H
