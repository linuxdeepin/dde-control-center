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
#ifndef BLUETOOTHDBUSPROXY_H
#define BLUETOOTHDBUSPROXY_H

#include <QObject>
class QDBusInterface;
class QDBusMessage;
class QDBusObjectPath;

class BluetoothDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit BluetoothDBusProxy(QObject *parent = nullptr);

    void showBluetoothTransDialog(const QString &address, const QStringList &files);
    // Bluetooth
    bool bluetoothIsValid();
    Q_PROPERTY(uint State READ state NOTIFY StateChanged)
    uint state();
    Q_PROPERTY(bool Transportable READ transportable NOTIFY TransportableChanged)
    bool transportable();
    Q_PROPERTY(bool CanSendFile READ canSendFile NOTIFY CanSendFileChanged)
    bool canSendFile();
    Q_PROPERTY(bool DisplaySwitch READ displaySwitch WRITE setDisplaySwitch NOTIFY DisplaySwitchChanged)
    bool displaySwitch();
    void setDisplaySwitch(bool value);
    // AirplaneMode
    Q_PROPERTY(bool Enabled READ enabled NOTIFY EnabledChanged)
    bool enabled();

Q_SIGNALS: // SIGNALS
    // Bluetooth
    void AdapterAdded(const QString &adapterJSON);
    void AdapterRemoved(const QString &adapterJSON);
    void AdapterPropertiesChanged(const QString &adapterJSON);
    void DeviceAdded(const QString &devJSON);
    void DeviceRemoved(const QString &devJSON);
    void DevicePropertiesChanged(const QString &devJSON);
    void Cancelled(const QDBusObjectPath &device);
    void RequestAuthorization(const QDBusObjectPath &device);
    void RequestConfirmation(const QDBusObjectPath &device, const QString &passkey);
    void RequestPasskey(const QDBusObjectPath &device);
    void RequestPinCode(const QDBusObjectPath &device);
    void DisplayPasskey(const QDBusObjectPath &device, uint passkey, uint entered);
    void DisplayPinCode(const QDBusObjectPath &device, const QString &pinCode);
    // property changed signals
    void StateChanged(uint value) const;
    void TransportableChanged(bool value) const;
    void CanSendFileChanged(bool value) const;
    void DisplaySwitchChanged(bool value) const;

    // AirplaneMode
    // property changed signals
    void EnabledChanged(bool value) const;

public Q_SLOTS: // METHODS
    // Bluetooth
    void ClearUnpairedDevice();
    void ClearUnpairedDevice(QObject *receiver, const char *member);
    void SetAdapterPowered(const QDBusObjectPath &adapter, bool powered);
    void SetAdapterPowered(const QDBusObjectPath &adapter, bool powered, QObject *receiver, const char *member, const char *errorSlot);
    void DisconnectDevice(const QDBusObjectPath &device);
    void RemoveDevice(const QDBusObjectPath &adapter, const QDBusObjectPath &device);
    void ConnectDevice(const QDBusObjectPath &device, const QDBusObjectPath &adapter);
    QString GetDevices(const QDBusObjectPath &adapter);
    void GetDevices(const QDBusObjectPath &adapter, QObject *receiver, const char *member);
    QString GetAdapters();
    void SetAdapterAlias(const QDBusObjectPath &adapter, const QString &alias);
    void SetDeviceAlias(const QDBusObjectPath &device, const QString &alias);
    void RequestDiscovery(const QDBusObjectPath &adapter);
    void Confirm(const QDBusObjectPath &device, bool accept);
    void SetAdapterDiscovering(const QDBusObjectPath &adapter, bool discovering);
    void SetAdapterDiscoverable(const QDBusObjectPath &adapter, bool discoverable);

    // AirplaneMode

private slots:
    void onPropertiesChanged(const QDBusMessage &message);

private:
    QDBusInterface *m_bluetoothInter;
    QDBusInterface *m_airPlaneModeInter;
};

#endif // BLUETOOTHDBUSPROXY_H
