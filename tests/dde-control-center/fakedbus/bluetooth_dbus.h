// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef BLUETOOTH_DBUS_H
#define BLUETOOTH_DBUS_H

#include <QDBusContext>
#include <QObject>
#include <QDBusInterface>
#include <QJsonDocument>

#define BLUETOOTH_SERVICE_NAME "com.deepin.daemon.Bluetooth"
#define BLUETOOTH_SERVICE_PATH "/com/deepin/daemon/Bluetooth"

class Bluetooth : public QObject, protected QDBusContext
{
    Q_OBJECT
public:
    Bluetooth(QObject *parent = nullptr);
    virtual ~Bluetooth();

    Q_CLASSINFO("D-Bus Interface", BLUETOOTH_SERVICE_NAME)

    Q_PROPERTY(bool DisplaySwitch READ displaySwitch WRITE setDisplaySwitch NOTIFY DisplaySwitchChanged)
    bool displaySwitch() { return m_displaySwitch; }
    void setDisplaySwitch(bool value) { m_displaySwitch = value; }

    Q_PROPERTY(uint State READ state NOTIFY StateChanged)
    uint state() { return m_state; }

    Q_PROPERTY(bool Transportable READ transportable NOTIFY TransportableChanged)
    bool transportable() { return m_transportable; }

public Q_SLOTS:
    void CancelTransferSession(QDBusObjectPath sessionPath);
    void ClearUnpairedDevice();
    void SetAdapterPowered(QDBusObjectPath adapter, bool powered);
    void DisconnectDevice(QDBusObjectPath device);
    void RemoveDevice(QDBusObjectPath adapter, QDBusObjectPath device);
    void ConnectDevice(QDBusObjectPath device, QDBusObjectPath adapter);
    QString GetDevices(QDBusObjectPath adapter);
    QString GetAdapters();
    void SetAdapterAlias(QDBusObjectPath adapter, QString alias);
    void SetDeviceAlias(QDBusObjectPath device, QString alias);
    void RequestDiscovery(QDBusObjectPath adapter);
    void Confirm(QDBusObjectPath device, bool accept);
    void SetAdapterDiscovering(QDBusObjectPath adapter, bool discovering);
    void SetAdapterDiscoverable(QDBusObjectPath adapter, bool discoverable);
    QString DebugInfo();
    void FeedPasskey(QDBusObjectPath device, bool accept, quint32 passkey);
    void FeedPinCode(QDBusObjectPath device, bool accept, QString pinCode);
    QDBusObjectPath SendFiles(QString device, QStringList files);
    void SetAdapterDiscoverableTimeout(QDBusObjectPath adapter, quint32 timeout);
    void SetDeviceTrusted(QDBusObjectPath device, bool trusted);

Q_SIGNALS:
    void AdapterAdded(QString);
    void AdapterPropertiesChanged(QString);
    void AdapterRemoved(QString);
    void Cancelled(QDBusObjectPath);
    void DeviceAdded(QString);
    void DevicePropertiesChanged(QString);
    void DeviceRemoved(QString);
    void DisplayPasskey(QDBusObjectPath, quint32, quint32);
    void DisplayPinCode(QDBusObjectPath, QString);
    void RequestAuthorization(QDBusObjectPath);
    void RequestConfirmation(QDBusObjectPath, QString);
    void RequestPasskey(QDBusObjectPath);
    void RequestPinCode(QDBusObjectPath);
    void ObexSessionCreated(QDBusObjectPath);
    void ObexSessionRemoved(QDBusObjectPath);
    void ObexSessionProgress(QDBusObjectPath, quint64, quint64, qint32);
    void TransferCreated(QString, QDBusObjectPath, QDBusObjectPath);
    void TransferRemoved(QString, QDBusObjectPath, QDBusObjectPath, bool);
    void TransferFailed(QString, QDBusObjectPath, QString);
    void ConnectFailed(QDBusObjectPath);

    // begin property changed signals
    void DisplaySwitchChanged(bool  value) const;
    void StateChanged(uint  value) const;
    void TransportableChanged(bool  value) const;

private:
    bool m_displaySwitch{false};
    bool m_transportable{false};
    quint32 m_state{0};
};
#endif
