/*
* Copyright (C) 2021 ~ 2021 Uniontech Software Technology Co.,Ltd.
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
#include "notificationmanager.h"
#include "notification/bubblemanager.h"

const QString notifyIconNetworkOffline = "notification-network-offline";
const QString notifyIconWiredConnected = "notification-network-wired-connected";
const QString notifyIconWiredDisconnected = "notification-network-wired-disconnected";
const QString notifyIconWiredError = "notification-network-wired-disconnected";
const QString notifyIconWirelessConnected = "notification-network-wireless-full";
const QString notifyIconWirelessDisconnected = "notification-network-wireless-disconnected";
const QString notifyIconWirelessDisabled = "notification-network-wireless-disabled";
const QString notifyIconWirelessError = "notification-network-wireless-disconnected";
const QString notifyIconVpnConnected = "notification-network-vpn-connected";
const QString notifyIconVpnDisconnected = "notification-network-vpn-disconnected";
const QString notifyIconProxyEnabled = "notification-network-proxy-enabled";
const QString notifyIconProxyDisabled = "notification-network-proxy-disabled";
const QString notifyIconNetworkConnected = "notification-network-wired-connected";
const QString notifyIconNetworkDisconnected = "notification-network-wired-disconnected";
const QString notifyIconMobile2gConnected = "notification-network-mobile-2g-connected";
const QString notifyIconMobile2gDisconnected = "notification-network-mobile-2g-disconnected";
const QString notifyIconMobile3gConnected = "notification-network-mobile-3g-connected";
const QString notifyIconMobile3gDisconnected = "notification-network-mobile-3g-disconnected";
const QString notifyIconMobile4gConnected = "notification-network-mobile-4g-connected";
const QString notifyIconMobile4gDisconnected = "notification-network-mobile-4g-disconnected";
const QString notifyIconMobileUnknownConnected = "notification-network-mobile-unknown-connected";
const QString notifyIconMobileUnknownDisconnected = "notification-network-mobile-unknown-disconnected";

BubbleManager *NotificationManager::BubbleManagerinstance()
{
    static BubbleManager *s_bubbleManager = new BubbleManager();
    return s_bubbleManager;
}
uint NotificationManager::Notify(const QString &icon, const QString &body)
{
    return BubbleManagerinstance()->Notify("dde-control-center", 0, icon, "", body);
}

uint NotificationManager::NetworkNotify(NetworkNotifyType type, const QString &name)
{
    switch (type) {
    case WiredConnecting:
        return NotificationManager::Notify(notifyIconWiredConnected, QObject::tr("Connecting %1").arg(name));
    case WirelessConnecting:
        return NotificationManager::Notify(notifyIconWirelessConnected, QObject::tr("Connecting %1").arg(name));
    case WiredConnected:
        return NotificationManager::Notify(notifyIconWiredConnected, QObject::tr("%1 connected").arg(name));
    case WirelessConnected:
        return NotificationManager::Notify(notifyIconWirelessConnected, QObject::tr("%1 connected").arg(name));
    case WiredDisconnected:
        return NotificationManager::Notify(notifyIconWiredDisconnected, QObject::tr("%1 disconnected").arg(name));
    case WirelessDisconnected:
        return NotificationManager::Notify(notifyIconWirelessDisconnected, QObject::tr("%1 disconnected").arg(name));
    case WiredUnableConnect:
        return NotificationManager::Notify(notifyIconWiredDisconnected, QObject::tr("Unable to connect %1, please check your router or net cable.").arg(name));
    case WirelessUnableConnect:
        return NotificationManager::Notify(notifyIconWirelessDisconnected, QObject::tr("Unable to connect %1, please keep closer to the wireless router").arg(name));
    case WiredConnectionFailed:
        return NotificationManager::Notify(notifyIconWiredDisconnected, QObject::tr("Connection failed, unable to connect %1, wrong password").arg(name));
    case WirelessConnectionFailed:
        return NotificationManager::Notify(notifyIconWirelessConnected, QObject::tr("Connection failed, unable to connect %1, wrong password").arg(name));
    case NoSecrets:
        return NotificationManager::Notify(notifyIconWirelessDisconnected, QObject::tr("Password is required to connect %1").arg(name));
    case SsidNotFound:
        return NotificationManager::Notify(notifyIconWirelessDisconnected, QObject::tr("The %1 802.11 WLAN network could not be found").arg(name));
    }
    return 0;
}
