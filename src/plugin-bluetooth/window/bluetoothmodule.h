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
#ifndef BLUETOOTHMODULE_H
#define BLUETOOTHMODULE_H

#include "interface/moduleobject.h"

#include <QDBusObjectPath>

class BluetoothModel;
class BluetoothWorker;
class BluetoothAdapter;
class BluetoothDevice;
class QDBusObjectPath;
class PinCodeDialog;
class AdapterModule;

class BluetoothModule : public DCC_NAMESPACE::ModuleObject
{
    Q_OBJECT
public:
    explicit BluetoothModule(QObject *parent = nullptr);
    ~BluetoothModule() override { }
    void deactive() override;
    bool hasDevice();

Q_SIGNALS:
    void showBluetoothDetail(const BluetoothAdapter *adapter) const;
    void requestModuleVisible(const bool visible) const;

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

protected:
    bool event(QEvent *ev) override;

private:
    void setVisibleState();
    void updateWidget();
    AdapterModule *getAdapter(const BluetoothAdapter *adapter);

private:
    BluetoothModel *m_model;
    BluetoothWorker *m_work;
    QMap<const BluetoothAdapter *, AdapterModule *> m_valueMap;
    QMap<QDBusObjectPath, PinCodeDialog *> m_dialogs;
    int index;
    DCC_NAMESPACE::ModuleObject *m_parent;
};

#endif // BLUETOOTHMODULE_H
