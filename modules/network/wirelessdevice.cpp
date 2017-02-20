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
    QSet<QString> ssidList = m_aps;
    m_aps.clear();

    const QJsonArray aps = QJsonDocument::fromJson(apList.toUtf8()).array();
    for (auto const item : aps)
    {
        const auto ap = item.toObject();
        const auto ssid = ap.value("Ssid").toString();

        if (m_aps.contains(ssid))
            emit apInfoChanged(ap);
        else
            emit apAdded(ap);

        m_aps << ssid;
    }

    for (const auto ssid : ssidList)
    {
        if (!m_aps.contains(ssid))
            emit apRemoved(ssid);
    }
}

void WirelessDevice::updateAPInfo(const QString &apInfo)
{
    const auto ap = QJsonDocument::fromJson(apInfo.toUtf8()).object();
    const auto ssid = ap.value("Ssid").toString();

    if (m_aps.contains(ssid))
        emit apInfoChanged(ap);
    else
        emit apAdded(ap);

    m_aps << ssid;
}

void WirelessDevice::deleteAP(const QString &apInfo)
{
    const auto ap = QJsonDocument::fromJson(apInfo.toUtf8()).object();
    const auto ssid = ap.value("Ssid").toString();

    m_aps.remove(ssid);
    emit apRemoved(ssid);
}

void WirelessDevice::setActiveApName(const QString &name)
{
    if (m_activeAp != name)
    {
        const QString oldName = m_activeAp;
        m_activeAp = name;

        emit activeApChanged(oldName, m_activeAp);
    }
}
