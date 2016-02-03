/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "networkdata.h"

namespace NetworkPlugin {

QJsonArray deviceArray(const QString &deviceType, DBusNetwork *dbusNetwork)
{
    QJsonObject allObj = QJsonDocument::fromJson(dbusNetwork->devices().toUtf8()).object();
    return allObj.value(deviceType).toArray();
}

DeviceInfo getDeviceInfoById(const QString &uuid, DBusNetwork *dbusNetwork) {
    DeviceInfo info;
    QJsonArray array = deviceArray(NetworkPlugin::ConnectionTypeWireless, dbusNetwork);
    foreach (QJsonValue value, array) {
        QJsonObject obj = value.toObject();
        if (obj.value("UniqueUuid").toString() == uuid) {
            info.activeAp = obj.value("ActiveAp").toString();
            info.hwAddress = obj.value("HwAddress").toString();
            info.managed = obj.value("Managed").toBool();
            info.mobileNetworkType = obj.value("MobileNetworkType").toString();
            info.mobileSignalQuality = obj.value("MobileSignalQuality").toString();
            info.path = obj.value("Path").toString();
            info.state = obj.value("State").toInt();
            info.uniqueUnid = uuid;
            info.usbDevice = obj.value("UsbDevice").toBool();
            info.vendor = obj.value("Vendor").toString();
        }
    }

    return info;
}

QJsonArray connectiosArray(const QString &deviceType, DBusNetwork *dbusNetwork)
{
    QJsonObject allObj = QJsonDocument::fromJson(dbusNetwork->connections().toUtf8()).object();
    return allObj.value(deviceType).toArray();
}

QStringList getApUuidsBySsid(const QString &ssid, DBusNetwork *dbusNetwork)
{
    QStringList uuids;
    QVariantList list = connectiosArray(ConnectionTypeWireless, dbusNetwork).toVariantList();
    for (QVariant v : list) {
        if (v.toMap().value("Ssid").toString() == ssid)
            uuids.append(v.toMap().value("Uuid").toString());
    }

    return uuids;
}

ConnectionState getActiveConnectionsStateByUuid(const QString &uuid, DBusNetwork *dbusNetwork)
{
    QJsonObject allObj = QJsonDocument::fromJson(dbusNetwork->activeConnections().toUtf8()).object();
    QStringList keys = allObj.keys();
    foreach (QString key, keys) {
        QJsonObject obj = allObj.value(key).toObject();
        if (obj.value("Uuid").toString() == uuid)
            return ConnectionState(obj.value("State").toInt());
    }

    return ActiveConnectionStateUnknown;
}

QStringList getActiveConnectionDevices(DBusNetwork *dbusNetwork)
{
    QStringList list;
    QJsonObject allObj = QJsonDocument::fromJson(dbusNetwork->activeConnections().toUtf8()).object();
    QStringList keys = allObj.keys();
    foreach (QString key, keys) {
        QJsonObject obj = allObj.value(key).toObject();
        list << obj.value("Devices").toArray().first().toString();
    }

    return list;
}

QList<ActiveConnectionInfo> getActiveConnectionsInfo(DBusNetwork *dbusNetwork)
{
    QList<ActiveConnectionInfo> list;
    QJsonArray allArray = QJsonDocument::fromJson(dbusNetwork->GetActiveConnectionInfo().value().toUtf8()).array();

    foreach (QJsonValue value, allArray) {
        QJsonObject obj = value.toObject();
        if (!value.toObject().isEmpty()) {
            ActiveConnectionInfo info;
            info.connectionName = obj["ConnectionName"].toString();
            info.connectionType = obj["ConnectionType"].toString();
            info.deviceInterface = obj["DeviceInterface"].toString();
            info.deviceType = obj["DeviceType"].toString();
            info.hwAddress = obj["HwAddress"].toString();
            info.ip4 = obj["Ip4"].toObject();
            info.ip6 = obj["Ip6"].toObject();
            info.isPrimaryConnection = obj["IsPrimaryConnection"].toBool();
            info.mobileNetworkType = obj["MobileNetworkType"].toString();
            info.security = obj["Security"].toString();
            info.speed = obj["Speed"].toString();
            list << info;
        }
    }

    return list;
}

bool hasVpn(DBusNetwork *dbusNetwork)
{
    if (dbusNetwork->isValid()) {
        QJsonArray array = connectiosArray(ConnectionTypeVpn, dbusNetwork);
        return array.size() > 0;
    }
    else {
        return false;
    }
}

bool vpnIsConnected(DBusNetwork *dbusNetwork)
{
    if (dbusNetwork->isValid()) {
        QList<ActiveConnectionInfo> infoList = getActiveConnectionsInfo(dbusNetwork);
        for (ActiveConnectionInfo info : infoList) {
            if (info.connectionType.startsWith("vpn"))
                return true;
        }
        return false;
    }
    else
        return false;
}

int wiredDevicesCount(DBusNetwork *dbusNetwork)
{
    if (dbusNetwork->isValid()) {
        QJsonArray array = deviceArray(ConnectionTypeWired, dbusNetwork);
        return array.size();
    }
    else {
        return 0;
    }
}

bool wiredIsConnected(DBusNetwork *dbusNetwork)
{
    if (!dbusNetwork->isValid())
        return false;

    QList<ActiveConnectionInfo> infoList = getActiveConnectionsInfo(dbusNetwork);
    foreach (ActiveConnectionInfo info, infoList) {
        if (info.connectionType == ConnectionTypeWired)
            return true;
    }

    return false;
}

QString wiredIp4Address(DBusNetwork *dbusNetwork)
{
    if (dbusNetwork->isValid()) {
        QList<NetworkPlugin::ActiveConnectionInfo> infoList = NetworkPlugin::getActiveConnectionsInfo(dbusNetwork);
        foreach (NetworkPlugin::ActiveConnectionInfo info, infoList) {
            if (info.connectionType == NetworkPlugin::ConnectionTypeWired)
                return info.ip4["Address"].toString();
        }

    }
    return QString();
}

int wirelessDevicesCount(DBusNetwork *dbusNetwork)
{
    if (dbusNetwork->isValid()) {
        QJsonArray array = deviceArray(ConnectionTypeWireless, dbusNetwork);
        return array.size();
    }
    else {
        return 0;
    }
}


QMap<QString, QString> wirelessDevices(DBusNetwork *dbusNetwork)
{
    QMap<QString, QString> tmpMap;
    if (dbusNetwork->isValid()) {
        QJsonArray pathArray = deviceArray(ConnectionTypeWireless, dbusNetwork);
        foreach (QJsonValue pathValue, pathArray) {
            tmpMap.insert(pathValue.toObject().value("UniqueUuid").toString(),
                          pathValue.toObject().value("Vendor").toString());
        }
    }

    return tmpMap;
}

int wirelessApMaxStrength(DBusNetwork *dbusNetwork)
{
    if (dbusNetwork->isValid()) {
        int maxStrength = -1;
        QStringList uuids = wirelessDevices(dbusNetwork).keys();
        for (QString uuid : uuids) {
            DeviceInfo info = getDeviceInfoById(uuid, dbusNetwork);
            QString activeApPath = info.activeAp;

            QString accessPoints = dbusNetwork->GetAccessPoints(QDBusObjectPath(info.path)).value();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(accessPoints.toUtf8());
            QVariantList mapList = jsonDoc.array().toVariantList();

            for(const QVariant &map : mapList) {
                if (map.toMap().value("Path").toString() == activeApPath) {
                    maxStrength = qMax(maxStrength, map.toMap().value("Strength").toInt());//fixme
                    break;
                }
            }
        }

        return maxStrength;
    }
    else {
        return -1;
    }
}

QList<BluetoothAdapterInfo> bluetoothAdapters(DBusBluetooth *dbusBluetooth)
{
    QList<BluetoothAdapterInfo> list;

    if (dbusBluetooth->isValid()) {
        const QString &adapters = dbusBluetooth->GetAdapters();
        QJsonArray array = QJsonDocument::fromJson(adapters.toUtf8()).array();

        for (QJsonValue value : array) {
            QJsonObject obj = value.toObject();
            if (!obj.isEmpty()) {
                BluetoothAdapterInfo ai;
//                ai.alias = obj["Alias"].toString();
//                ai.discoverable = obj["Discoverable"].toBool();
//                ai.discoverableTimeout = obj["DiscoverableTimeout"].toInt();
//                ai.discovering = obj["Discovering"].toBool();
//                ai.name = obj["Name"].toString();
                ai.path = obj["Path"].toString();
                ai.powered = obj["Powered"].toBool();

                list << ai;
            }
        }
    }

    return list;
}

//bool bluetoothAdaptersActived(DBusBluetooth *dbusBluetooth)
//{
//    const QString &devices = dbusBluetooth->GetDevices();

//    QVariantMap allObjMap = QJsonDocument::fromJson(devices.toUtf8()).object().toVariantMap();
//    //all adapter
//    for (QVariant mv : allObjMap.values()) {
//        //all devices of the adapter
//        QVariantList list = mv.toList();
//        foreach (QVariant v, list) {
//            int state = v.toMap().value("State").toInt();
//            if (state == BluetoothStateConnected)
//                return true;
//        }
//    }

//    return false;
//}

}
