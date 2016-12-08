#include "networkmodel.h"
#include "networkdevice.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>

NetworkDevice::DeviceType parseDeviceType(const QString &type)
{
    if (type == "wireless") {
        return NetworkDevice::Wireless;
    }
    if (type == "wired") {
        return NetworkDevice::Wired;
    }

    Q_UNREACHABLE_IMPL();
    return NetworkDevice::Wired;
}

NetworkModel::NetworkModel(QObject *parent)
    : QObject(parent)
{

}

NetworkModel::~NetworkModel()
{
    qDeleteAll(m_wiredDevices);
    qDeleteAll(m_wirelessDevices);
}

void NetworkModel::onDevicesPropertyChanged(const QString &devices)
{
    const QJsonObject data = QJsonDocument::fromJson(devices.toUtf8()).object();
    for (auto it(data.constBegin()); it != data.constEnd(); ++it) {
        const auto type = parseDeviceType(it.key());
        const auto list = it.value().toArray();

        updateDeviceList(type, list);
    }
}

void NetworkModel::updateDeviceList(const NetworkDevice::DeviceType type, const QJsonArray &list)
{
    // construct new list
    QList<NetworkDevice *> currentList;
    for (auto const l : list) {
        auto *device = new NetworkDevice(type, l.toObject());
        currentList.append(device);
    }

    // TODO: remove old list
    // update old list
    switch (type) {
    case NetworkDevice::Wired:
        m_wiredDevices = currentList;
        emit wiredDeviceListChanged(m_wiredDevices);
        break;
    case NetworkDevice::Wireless:
        emit wirelessDeviceListChanged(m_wirelessDevices);
        m_wirelessDevices = currentList;
        break;
    default:;
    }
}
