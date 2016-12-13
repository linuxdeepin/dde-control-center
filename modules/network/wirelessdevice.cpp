#include "wirelessdevice.h"

using namespace dcc::network;

#include <QSet>
#include <QDebug>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>

WirelessDevice::WirelessDevice(const QJsonObject &info, QObject *parent)
    : NetworkDevice(NetworkDevice::Wireless, info, parent)
{

}

void WirelessDevice::setAPList(const QString &apList)
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

void WirelessDevice::updateAPInfo(const QString &apInfo)
{
    qDebug() << apInfo;
}
