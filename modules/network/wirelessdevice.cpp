/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

bool WirelessDevice::supportHotspot() const
{
    return info()["SupportHotspot"].toBool();
}

const QString WirelessDevice::hotspotUuid() const
{
    Q_ASSERT(hotspotEnabled());

    return m_hotspotInfo.value("ConnectionUuid").toString();
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

void WirelessDevice::setHotspotInfo(const QJsonObject &hotspotInfo)
{
    Q_ASSERT(supportHotspot());

    const bool changed = m_hotspotInfo.isEmpty() != hotspotInfo.isEmpty();

    m_hotspotInfo = hotspotInfo;

    if (changed)
        emit hotspotEnabledChanged(hotspotEnabled());
}
