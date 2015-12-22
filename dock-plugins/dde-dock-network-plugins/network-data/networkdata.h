#ifndef NETWORKDATA
#define NETWORKDATA
#include <QString>
#include "dbus/dbusnetwork.h"
#include "dbus/dbusbluetooth.h"

#define ASYN_CALL(Fun, Code, captured...) { \
    QDBusPendingCallWatcher * watcher = new QDBusPendingCallWatcher(Fun, this); \
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [watcher, captured]{ \
        const QVariantList & args = watcher->reply().arguments(); \
        Code \
        watcher->deleteLater(); \
    }); }

namespace NetworkPlugin {

struct BluetoothAdapterInfo{
    bool powered;
//    bool discovering;
//    bool discoverable;
//    uint discoverableTimeout;
//    QString alias;
    QString path;
//    QString name;
};

enum BluetoothState{
    BluetoothStateDisconnected = 0,
    BluetoothStateConnecting = 1,
    BluetoothStateConnected = 2
};

struct DeviceInfo {
    bool managed;
    bool usbDevice;
    int state;
    QString path;
    QString hwAddress;
    QString vendor;
    QString uniqueUnid;
    QString activeAp;
    QString mobileNetworkType;
    QString mobileSignalQuality;
};

struct ActiveConnectionInfo {
    bool isPrimaryConnection;
    QString connectionType;
    QString connectionName;
    QString mobileNetworkType;
    QString security;
    QString deviceType;
    QString deviceInterface;
    QString hwAddress;
    QString speed;
    QJsonObject ip4;
    QJsonObject ip6;
};

// device state
enum DeviceState {
    DeviceStateUnknown = 0,
    DeviceStateUnmanaged = 10,
    DeviceStateUnavailable = 20,
    DeviceStateDisconnected = 30,
    DeviceStatePrepare = 40,
    DeviceStateConfig = 50,
    DeviceStateNeedAuth = 60,
    DeviceStateIpConfig = 70,
    DeviceStateIpCheck = 80,
    DeviceStateSecondaries = 90,
    DeviceStateActivated = 100,
    DeviceStateDeactivating = 110,
    DeviceStateFailed = 120
};

enum NetworkingState{
    NetworkingUnknown = 0,
    NetworkingAsleep = 10,
    NetworkingDisconnected = 20,
    NetworkingDisconnecting = 30,
    NetworkingConnecting = 40,
    NetworkingConnectedLocal = 50,
    NetworkingConnectedSite = 60,
    NetworkingConnectedGlobal = 70
};

// active connection state
enum ConnectionState {
    ActiveConnectionStateUnknown = 0,
    ActiveConnectionStateActivating = 1,
    ActiveConnectionStateActivated = 2,
    ActiveConnectionStateDeactivating = 3,
    ActiveConnectionStateDeactivate = 4
};

// connection type
const QString ConnectionTypeWired = "wired";
const QString ConnectionTypeWireless = "wireless";
const QString ConnectionTypeWirelessAdhoc = "wireless-adhoc";
const QString ConnectionTypeWirelessHotspot = "wireless-hotspot";
const QString ConnectionTypePppoe = "pppoe";
const QString ConnectionTypeMobile = "mobile";
const QString ConnectionTypeMobileGsm = "mobile-gsm";
const QString ConnectionTypeMobileCdma = "mobile-cdma";
const QString ConnectionTypeVpn = "vpn";
const QString ConnectionTypeVpnL2tp = "vpn-l2tp";
const QString ConnectionTypeVpnPptp = "vpn-pptp";
const QString ConnectionTypeVpnVpnc = "vpn-vpnc";
const QString ConnectionTypeVpnOpenvpn = "vpn-openvpn";
const QString ConnectionTypeVpnOpenconnect = "vpn-openconnect";

//public methods
QJsonArray deviceArray(const QString &deviceType, DBusNetwork *dbusNetwork);

QJsonArray connectiosArray(const QString &deviceType, DBusNetwork *dbusNetwork);

QString getApUuidBySsid(const QString &ssid, DBusNetwork *dbusNetwork);
ConnectionState getActiveConnectionsStateByUuid(const QString &uuid, DBusNetwork *dbusNetwork);
QStringList getActiveConnectionDevices(DBusNetwork *dbusNetwork);

DeviceInfo getDeviceInfoById(const QString &uuid, DBusNetwork *dbusNetwork);
QList<ActiveConnectionInfo> getActiveConnectionsInfo(DBusNetwork *dbusNetwork);

bool hasVpn(DBusNetwork *dbusNetwork);
bool vpnIsConnected(DBusNetwork *dbusNetwork);
int wiredDevicesCount(DBusNetwork *dbusNetwork);
QString wiredIp4Address(DBusNetwork *dbusNetwork);
bool wiredIsConnected(DBusNetwork *dbusNetwork);
int wirelessDevicesCount(DBusNetwork *dbusNetwork);
int wirelessApMaxStrength(DBusNetwork *dbusNetwork);
QMap<QString, QString> wirelessDevices(DBusNetwork *dbusNetwork);

QList<BluetoothAdapterInfo> bluetoothAdapters(DBusBluetooth *dbusBluetooth);
bool bluetoothAdaptersActived(DBusBluetooth *dbusBluetooth);
}

#endif // NETWORKDATA

