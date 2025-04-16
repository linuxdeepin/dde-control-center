//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCC_BLUETOOTH_BLUETOOTHWORKER_H
#define DCC_BLUETOOTH_BLUETOOTHWORKER_H

#include <QObject>

#include "bluetoothmodel.h"

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

    Q_INVOKABLE void setAdapterPowered(const QString adapterId, bool powered);
    Q_INVOKABLE void setAdapterDiscoverable(const QString adapterId, bool discoverable);
    Q_INVOKABLE void setAdapterAlias(const QString adapterId, const QString &alias);
    Q_INVOKABLE void connectDevice(const QString & deviceId, const QString adapterId);
    Q_INVOKABLE void jumpToAirPlaneMode();
    Q_INVOKABLE void setDeviceAlias(const QString &deviceId, const QString &alias);
    Q_INVOKABLE void disconnectDevice(const QString & deviceId);
    Q_INVOKABLE void ignoreDevice(const QString & deviceId, const QString adapterId);
    Q_INVOKABLE void setAdapterDiscovering(const QString &path, bool enable);
    Q_INVOKABLE void playErrorSound();

public Q_SLOTS:

    void ignoreDevice(const BluetoothAdapter *adapter, const BluetoothDevice *device);
    void setAdapterDiscoverable(const QString &path);

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
    bool m_connectingAudioDevice;
    uint m_state;
};

#endif // DCC_BLUETOOTH_BLUETOOTHWORKER_H
