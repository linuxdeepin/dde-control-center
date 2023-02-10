//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef BLUETOOTHMODULE_H
#define BLUETOOTHMODULE_H

#include "interface/pagemodule.h"

#include <QDBusObjectPath>

class BluetoothModel;
class BluetoothWorker;
class BluetoothAdapter;
class BluetoothDevice;
class QDBusObjectPath;
class PinCodeDialog;
class AdapterModule;

class BluetoothModule : public DCC_NAMESPACE::PageModule
{
    Q_OBJECT

public:
    explicit BluetoothModule(QObject *parent = nullptr);
    ~BluetoothModule() override { }
    void active() override;
    void deactive() override;
    bool hasDevice();

Q_SIGNALS:
    void showBluetoothDetail(const BluetoothAdapter *adapter) const;

    void requestSetToggleAdapter(const BluetoothAdapter *adapter, const bool &toggled);
    void requestConnectDevice(const BluetoothDevice *device, const BluetoothAdapter *adapter);
    void requestDisconnectDevice(const BluetoothDevice *device);
    void requestSetAlias(const BluetoothAdapter *adapter, const QString &alias);
    void requestDiscoverable(const BluetoothAdapter *adapter, const bool &discoverable);

public Q_SLOTS:
    void showPinCode(const QDBusObjectPath &device, const QString &code);
    void closePinCode(const QDBusObjectPath &device);

    void addAdapter(const BluetoothAdapter *adapter);
    void removeAdapter(const BluetoothAdapter *adapter);
    void requestRefresh(const BluetoothAdapter *adapter);
    void updateVisible();

private:
    void updateWidget();
    AdapterModule *getAdapter(const BluetoothAdapter *adapter);

private:
    BluetoothModel *m_model;
    BluetoothWorker *m_work;
    QMap<const BluetoothAdapter *, AdapterModule *> m_valueMap;
    QMap<QDBusObjectPath, PinCodeDialog *> m_dialogs;
};

#endif // BLUETOOTHMODULE_H
