/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef NETWORKGLOBAL
#define NETWORKGLOBAL

#include <QString>
#include <QVariant>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#define ASYN_CALL(Fun, Code, captured...) { \
    QDBusPendingCallWatcher * watcher = new QDBusPendingCallWatcher(Fun, this); \
    auto onFinished = [watcher, captured]{ \
        const QVariantList & args = watcher->reply().arguments(); \
        Q_UNUSED(args);\
        Code \
        watcher->deleteLater(); \
    };\
    if(watcher->isFinished()) onFinished();\
    else connect(watcher, &QDBusPendingCallWatcher::finished, onFinished);}

#define TIMER_SINGLESHOT(Time, Code, captured...){ \
    QTimer *timer = new QTimer;\
    timer->setSingleShot(true);\
    QObject::connect(timer, &QTimer::timeout, [timer, captured] {\
        timer->deleteLater();\
        Code\
    });\
    timer->start(Time);\
}

#define SIGNAL_BLOCKE(obj) QSignalBlocker blocker(obj);Q_UNUSED(blocker);

#define SIGNAL_UNBLOCK(obj) blocker.unblock();

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
    const QString VpnStrongSwan = "vpn-strongswan";
    const QString VpnOpenconnect = "vpn-openconnect";
}

namespace ProxyMethod {
    const QString None = "none";
    const QString Manual = "manual";
    const QString Auto = "auto";
}

namespace ProxyType {
    const QString HTTP = "http";
    const QString HTTPS = "https";
    const QString FTP = "ftp";
    const QString SOCKS = "socks";
}

namespace WidgetType {
    const QString SwitchButton = "EditLineSwitchButton";
    const QString ComboBox = "EditLineComboBox";
    const QString EditableComboBox = "EditLineEditComboBox";
    const QString TextInput = "EditLineTextInput";
    const QString PasswordInput = "EditLinePasswordInput";
    const QString FileChooser = "EditLineFileChooser";
    const QString SpinBox = "EditLineSpinner";
    const QString Ipv4Input = "EditLineIpv4Input";
    const QString MissingPackage = "EditLineMissingPackage";
}

class NetworkMainWidget;

namespace DCCNetwork {
    NetworkMainWidget* parentNetworkMainWidget(const QObject *obj);

    template <class T>
    T* parentObject(const QObject *obj)
    {
        QObject *tmp_obj = obj->parent();

        while(tmp_obj) {
            T *widget = qobject_cast<T*>(tmp_obj);

            if(widget)
                return widget;

            tmp_obj = tmp_obj->parent();
        }

        return NULL;
    }
}

#endif // NETWORKGLOBAL

