/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef BLUETOOTHOBJECT_H
#define BLUETOOTHOBJECT_H

#include <QFrame>
#include <QVBoxLayout>

#include <dlistwidget.h>
#include <dboxwidget.h>

#include "dbus/dbusbluetooth.h"

DWIDGET_USE_NAMESPACE

class AdapterWidget;
class DeviceItemWidget;
class BluetoothObject : public QObject
{
    Q_OBJECT
public:
    struct DeviceInfo;
    ///本机的蓝牙适配器的信息
    struct AdapterInfo {
        bool powered;
        bool discovering;
        bool discoverable;
        uint discoverableTimeout;
        QString path;
        QString name;
        AdapterWidget *widget = nullptr;///显示蓝牙适配器搜到的设备列表的list
        DBusBluetooth *bluetoothDbus = nullptr;
    };
    ///搜到的蓝牙设备信息
    struct DeviceInfo {
        bool trusted;
        bool paired;
        int state;
        QString path;
        QString name;
        QString icon;
        QString computer;
        struct AdapterInfo *adapterInfo = nullptr;
        DeviceItemWidget *item = nullptr;///

        enum State {
            Disconnected = 0,
            Connecting = 1,
            Connected = 2
        };
    };

    explicit BluetoothObject(QObject *parent = 0);
    ~BluetoothObject();

    QStringList getIds() const;
    AdapterInfo *getAdapterInfoByPath(const QString &path) const;

    bool dbusValid() const;

signals:
    void adapterAdd(const QString &path);
    void adapterRemove(const QString &path);
    void adapterPropertyChanged(const QString &path);

private slots:
    void addAdapter(AdapterInfo *info);
    void removeAdapter(const AdapterInfo *info);

private:
    DBusBluetooth *m_bluetoothDbus;

    QMap<QString, AdapterInfo *> m_pathToAdapterInfoMap;
    QMap<QString, DeviceInfo *> m_pathToDeviceInfoMap;

    AdapterInfo *newAdapterInfoByMap(const QVariantMap &map);
    DeviceInfo *newDeviceInfoByMap(const QVariantMap &map);
    DeviceItemWidget *newDeviceListItem(DeviceInfo *device_info) const;

    void updateAdapterInfoByMap(AdapterInfo *info, const QVariantMap &map);
    void updateDeviceInfoByMap(DeviceInfo *info, const QVariantMap &map);

    Q_SLOT void init();
    void initBackend();
};

#endif // BLUETOOTHOBJECT_H
