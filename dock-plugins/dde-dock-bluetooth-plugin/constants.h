/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef CONSTANTS_H
#define CONSTANTS_H

#define ASYN_CALL(Fun, Code, captured...) { \
    QDBusPendingCallWatcher * watcher = new QDBusPendingCallWatcher(Fun, this); \
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [watcher, captured]{ \
        const QVariantList & args = watcher->reply().arguments(); \
        Code \
        watcher->deleteLater(); \
    }); }

#define TIMER_SINGLESHOT(Time, Code, captured...){ \
    QTimer *timer = new QTimer;\
    timer->setSingleShot(true);\
    QObject::connect(timer, &QTimer::timeout, [timer, captured] {\
        timer->deleteLater();\
        Code\
    });\
    timer->start(Time);\
}

#define BLUETOOTH_PLUGIN_WIDTH 220
#define DEVICE_LIST_MAX_HEIGHT 300

#endif // CONSTANTS_H
