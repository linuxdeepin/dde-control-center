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

QString getApUuidBySsid(const QString &ssid, DBusNetwork *dbusNetwork)
{
    QVariantList list = connectiosArray(ConnectionTypeWireless, dbusNetwork).toVariantList();
    foreach (QVariant v, list) {
        if (v.toMap().value("Ssid").toString() == ssid)
            return v.toMap().value("Uuid").toString();
    }

    return QString();
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

}
