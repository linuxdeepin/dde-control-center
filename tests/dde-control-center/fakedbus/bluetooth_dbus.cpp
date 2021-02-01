#include "bluetooth_dbus.h"

Bluetooth::Bluetooth(QObject *parent)
{
    Q_UNUSED(parent)
}

Bluetooth::~Bluetooth()
{

}

void Bluetooth::CancelTransferSession(QDBusObjectPath sessionPath)
{
    Q_UNUSED(sessionPath)
}

void Bluetooth::ClearUnpairedDevice()
{

}

void Bluetooth::SetAdapterPowered(QDBusObjectPath adapter, bool powered)
{
    Q_UNUSED(adapter)
    Q_UNUSED(powered)
}

void Bluetooth::DisconnectDevice(QDBusObjectPath device)
{
    Q_UNUSED(device)
}

void Bluetooth::RemoveDevice(QDBusObjectPath adapter, QDBusObjectPath device)
{
    Q_UNUSED(adapter)
    Q_UNUSED(device)
}

void Bluetooth::ConnectDevice(QDBusObjectPath device, QDBusObjectPath adapter)
{
    Q_UNUSED(device)
    Q_UNUSED(adapter)
}

QString Bluetooth::GetDevices(QDBusObjectPath adapter)
{
    Q_UNUSED(adapter)
    return R"([{"Path":"/org/bluez/hci0/dev_A4_50_46_BC_4A_5B","AdapterPath":"/org/bluez/hci0","Alias":"UnitTest","Trusted":false,"Paired":true,"State":0,"ServicesResolved":false,"ConnectState":true,"UUIDs":[],"Name":"UnitTest","Icon":"phone","RSSI":-73,"Address":"A4:50:46:BC:4A:5B"}])";
}

QString Bluetooth::GetAdapters()
{
    return "";
}

void Bluetooth::SetAdapterAlias(QDBusObjectPath adapter, QString alias)
{
    Q_UNUSED(adapter)
    Q_UNUSED(alias)
}

void Bluetooth::SetDeviceAlias(QDBusObjectPath device, QString alias)
{
    Q_UNUSED(device)
    Q_UNUSED(alias)
}

void Bluetooth::RequestDiscovery(QDBusObjectPath adapter)
{
    Q_UNUSED(adapter)
}

void Bluetooth::Confirm(QDBusObjectPath device, bool accept)
{
    Q_UNUSED(device)
    Q_UNUSED(accept)
}

void Bluetooth::SetAdapterDiscovering(QDBusObjectPath adapter, bool discovering)
{
    Q_UNUSED(adapter)
    Q_UNUSED(discovering)
}

void Bluetooth::SetAdapterDiscoverable(QDBusObjectPath adapter, bool discoverable)
{
    Q_UNUSED(adapter)
    Q_UNUSED(discoverable)
}

QString Bluetooth::DebugInfo()
{
    return "";
}

void Bluetooth::FeedPasskey(QDBusObjectPath device, bool accept, quint32 passkey)
{
    Q_UNUSED(device)
    Q_UNUSED(accept)
    Q_UNUSED(passkey)
}

void Bluetooth::FeedPinCode(QDBusObjectPath device, bool accept, QString pinCode)
{
    Q_UNUSED(device)
    Q_UNUSED(accept)
    Q_UNUSED(pinCode)
}

QDBusObjectPath Bluetooth::SendFiles(QString device, QStringList files)
{
    Q_UNUSED(device)
    Q_UNUSED(files)
    return QDBusObjectPath();
}

void Bluetooth::SetAdapterDiscoverableTimeout(QDBusObjectPath adapter, quint32 timeout)
{
    Q_UNUSED(adapter)
    Q_UNUSED(timeout)
}

void Bluetooth::SetDeviceTrusted(QDBusObjectPath device, bool trusted)
{
    Q_UNUSED(device)
    Q_UNUSED(trusted)
}

