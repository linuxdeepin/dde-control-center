// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DCC_BLUETOOTH_BLUETOOTHWORKER_H
#define DCC_BLUETOOTH_BLUETOOTHWORKER_H

#include <QObject>

#include <com_deepin_daemon_bluetooth.h>
#include <com_deepin_daemon_airplanemode.h>

#include "bluetoothmodel.h"
#include "pincodedialog.h"

using DBusBluetooth = com::deepin::daemon::Bluetooth;
using DBusAirplaneMode = com::deepin::daemon::AirplaneMode;

namespace dcc {
namespace bluetooth {

class BluetoothWorker : public QObject
{
    Q_OBJECT
public:
    static BluetoothWorker &Instance(bool sync = false);

    BluetoothModel *model() { return m_model; }

    void activate();
    void deactivate();

    void blockDBusSignals(bool block);
    inline DBusBluetooth *getDBusObject() { return m_bluetoothInter; }

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
    void setDisplaySwitch(const bool &on);
    void handleDbusSignal(QDBusMessage mes);

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
    void onStateChanged(uint state);

private:
    explicit BluetoothWorker(BluetoothModel *model, bool sync = false);
    BluetoothWorker(BluetoothWorker const &) = delete;
    BluetoothWorker& operator =(BluetoothWorker const &) = delete;
    ~BluetoothWorker();

    DBusBluetooth *m_bluetoothInter;
    DBusAirplaneMode *m_airPlaneModeInter;
    BluetoothModel *m_model;
    QMap<QString, QPointer<PinCodeDialog>> m_dialogs;
    bool m_connectingAudioDevice;
    uint m_state;
    QTimer *m_powerSwitchTimer;
};

} // namespace bluetooth
} // namespace dcc

#endif // DCC_BLUETOOTH_BLUETOOTHWORKER_H
