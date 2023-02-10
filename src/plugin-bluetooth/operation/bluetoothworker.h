//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCC_BLUETOOTH_BLUETOOTHWORKER_H
#define DCC_BLUETOOTH_BLUETOOTHWORKER_H

#include <QObject>

#include "bluetoothmodel.h"
#include "pincodedialog.h"

class QDBusObjectPath;
class BluetoothDBusProxy;

class BluetoothWorker : public QObject
{
    Q_OBJECT
public:
    explicit BluetoothWorker(BluetoothModel *model, QObject *parent = nullptr);

    BluetoothModel *model() { return m_model; }

    void activate();
    void deactivate();

    void blockDBusSignals(bool block);

    bool displaySwitch();

Q_SIGNALS:
    void requestConfirmation(const QDBusObjectPath &path, const QString &code);
    void pinCodeCancel(const QDBusObjectPath &device);

public Q_SLOTS:
    void setAdapterPowered(const BluetoothAdapter *adapter, const bool &powered);
    void connectDevice(const BluetoothDevice *device, const BluetoothAdapter *adapter);
    void disconnectDevice(const BluetoothDevice *device);
    void ignoreDevice(const BluetoothAdapter *adapter, const BluetoothDevice *device);
    void setAlias(const BluetoothAdapter *adapter, const QString &alias);
    void setDeviceAlias(const BluetoothDevice *device, const QString &alias);
    void setAdapterDiscoverable(const QString &path);
    void pinCodeConfirm(const QDBusObjectPath &path, bool value);
    void setAdapterDiscovering(const QDBusObjectPath &path, bool enable);
    void onRequestSetDiscoverable(const BluetoothAdapter *adapter, const bool &discoverable);
    void setDisplaySwitch(const bool &on);
    void showBluetoothTransDialog(const QString &address, const QStringList &files);

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
    BluetoothWorker(BluetoothWorker const &) = delete;
    BluetoothWorker &operator=(BluetoothWorker const &) = delete;
    ~BluetoothWorker();

    BluetoothDBusProxy *m_bluetoothDBusProxy;
    BluetoothModel *m_model;
    QMap<QDBusObjectPath, PinCodeDialog *> m_dialogs;
    bool m_connectingAudioDevice;
    uint m_state;
};

#endif // DCC_BLUETOOTH_BLUETOOTHWORKER_H
