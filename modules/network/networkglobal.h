#ifndef NETWORKGLOBAL
#define NETWORKGLOBAL

#include <QString>
#include <QVariant>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#define ASYN_CALL(Fun, Code, captured...) { \
    QDBusPendingCallWatcher * watcher = new QDBusPendingCallWatcher(Fun, this); \
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [watcher, captured]{ \
        const QVariantList & args = watcher->reply().arguments(); \
        Code \
        watcher->deleteLater(); \
    }); }

//class AsynGetProperty : public QObject
//{
//public:
//    AsynGetProperty(QObject *parent = 0);
//};

//// device type
namespace DeviceType {
    const QString Unknown = "unknown";
    const QString Wired = "wired";
    const QString Wireless = "wireless";
    const QString Unused1 = "unused1";
    const QString Unused2 = "unused2";
    const QString Bt = "bt";
    const QString OlpcMesh = "olpc-mesh";
    const QString Wimax = "wimax";
    const QString Modem = "modem";
    const QString Infiniband = "infiniband";
    const QString Bond = "bond";
    const QString Vlan = "vlan";
    const QString Adsl = "adsl";
    const QString Bridge = "bridge";
}

//// device state
namespace DeviceState {
enum DeviceState{
    Unknown = 0,
    Unmanaged = 10,
    Unavailable = 20,
    Disconnected = 30,
    Prepare = 40,
    Config = 50,
    NeedAuth = 60,
    IpConfig = 70,
    IpCheck = 80,
    Secondaries = 90,
    Activated = 100,
    Deactivation = 110,
    Failed = 120
};
}

//// active connection state
namespace ActiveConnectionState {
enum ActiveConnectionState{
    Unknown = 0,
    Activating = 1,
    Activated = 2,
    Deactivating = 3,
    Deactivated = 4
};
}

//// networking state
namespace NetworkingState {
enum NetworkingState{
    Unknown = 0,
    Asleep = 10,
    Disconnected = 20,
    Disconnecting = 30,
    Connecting = 40,
    ConnectedLocal = 50,
    ConnectedSite = 60,
    ConnectedGlobal = 70
};
}

//// connection type
namespace ConnectionType {
    const QString Wired = "wired";
    const QString Wireless = "wireless";
    const QString WirelessAdhoc = "wireless-adhoc";
    const QString WirelessHotspot = "wireless-hotspot";
    const QString Pppoe = "pppoe";
    const QString Mobile = "mobile";
    const QString MobileGsm = "mobile-gsm";
    const QString MobileCdma = "mobile-cdma";
    const QString Vpn = "vpn";
    const QString VpnL2tp = "vpn-l2tp";
    const QString VpnPptp = "vpn-pptp";
    const QString VpnVpnc = "vpn-vpnc";
    const QString VpnOpenvpn = "vpn-openvpn";
    const QString VpnOpenconnect = "vpn-openconnect";
}

class NetworkMainWidget;

namespace DCCNetwork {
    NetworkMainWidget* parentNetworkMainWidget(const QObject *obj);
}

namespace BaseLineEditType {
enum BaseLineEditType {
    Normal,
    Password,
    FileChooser
};
}

#endif // NETWORKGLOBAL

