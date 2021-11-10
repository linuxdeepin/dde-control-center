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

#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QString>

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

protected:
    static BubbleManager *BubbleManagerinstance();
};

#endif // NOTIFICATIONMANAGER_H
