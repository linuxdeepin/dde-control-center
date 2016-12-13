#include "networkdevice.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>

using namespace dcc::network;

NetworkDevice::NetworkDevice(const DeviceType type, const QJsonObject &data, QObject *parent)
    : QObject(parent),

      m_type(type),
      m_data(data)
{

}

NetworkDevice::NetworkDevice(const NetworkDevice &device)
    : NetworkDevice(device.type(), device.data(), device.parent())
{

}

void NetworkDevice::setEnabled(const bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        emit enableChanged(m_enabled);
    }
}

const QString NetworkDevice::path() const
{
    return m_data.value("Path").toString();
}

void NetworkDevice::setAPList(const QString &apList)
{
    m_ssidSet.clear();

    const QJsonArray aps = QJsonDocument::fromJson(apList.toUtf8()).array();
    for (auto const item : aps)
    {
        const auto ap = item.toObject();
        const auto ssid = ap.value("Ssid").toString();

        if (m_ssidSet.contains(ssid))
            continue;
        m_ssidSet << ssid;

        emit apAdded(ap);
    }
}

void NetworkDevice::updateAPInfo(const QString &apInfo)
{
    qDebug() << apInfo;
}
