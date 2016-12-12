#include "networkmodel.h"
#include "networkdevice.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>

using namespace dcc::network;

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
    qDeleteAll(m_devices);
}

void NetworkModel::onDevicesPropertyChanged(const QString &devices)
{
    const QJsonObject data = QJsonDocument::fromJson(devices.toUtf8()).object();

    QList<NetworkDevice *> newList;
    for (auto it(data.constBegin()); it != data.constEnd(); ++it) {
        const auto type = parseDeviceType(it.key());
        const auto list = it.value().toArray();

        for (auto const l : list)
        {
            auto *device = new NetworkDevice(type, l.toObject());
            newList.append(device);
        }
    }

    qDeleteAll(m_devices);
    m_devices = newList;
    emit deviceListChanged(m_devices);
}
