// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QString>

class QObject;
class BubbleManager;
class NotificationManager
{
public:
    enum NetworkNotifyType {
        WiredConnecting,          // 有线连接中
        WirelessConnecting,       // 无线连接中
        WiredConnected,           // 有线已连接
        WirelessConnected,        // 无线已连接
        WiredDisconnected,        // 有线断开
        WirelessDisconnected,     // 无线断开
        WiredUnableConnect,       // 有线无法连接
        WirelessUnableConnect,    //  无线无法连接
        WiredConnectionFailed,    // 有线连接失败
        WirelessConnectionFailed, // 无线连接失败
        NoSecrets,                // 密码错误
        SsidNotFound,             // 没找到ssid
    };
    static uint Notify(const QString &icon, const QString &body);
    static uint NetworkNotify(NetworkNotifyType type, const QString &name);
    static void InstallEventFilter(QObject *obj);

protected:
    static BubbleManager *BubbleManagerinstance();
};

#endif // NOTIFICATIONMANAGER_H
