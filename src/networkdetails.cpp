/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     donghualin <donghualin@uniontech.com>
 *
 * Maintainer: donghualin <donghualin@uniontech.com>
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

#include "networkdetails.h"
#include "networkdevicebase.h"
#include "wireddevice.h"
#include "wirelessdevice.h"

#include <networkmanagerqt/settings.h>
#include <networkmanagerqt/connection.h>
#include <networkmanagerqt/connectionsettings.h>
#include <networkmanagerqt/ipv6setting.h>

#include <QJsonObject>

using namespace NetworkManager;
using namespace dde::network;

NetworkDetails::NetworkDetails(QObject *parent)
    : QObject(parent)
{
}

NetworkDetails::~NetworkDetails()
{
}

static const QString compressedIpv6Addr(const QString &ipv6Adr)
{
    if (ipv6Adr.contains("::"))
        return ipv6Adr;

    // NOTE(sbw): Calculate longest zero sequence, ensure
    int start = 0, len = 0;
    int maxStart = 0, maxLen = 0;
    const QStringList &sequence = ipv6Adr.split(':');
    for (int i = 0; i != sequence.size(); i++) {
        if (sequence[i] == "0000") {
            len += 5;
        } else {
            if (len > maxLen) {
                maxStart = start;
                maxLen = len;
            }
            start = (i + 1) * 5;
            len = 0;
        }
    }

    if (maxLen)
        return QString(ipv6Adr).replace(maxStart, maxLen, ':');

    if (len > maxLen) {
        if (start)
            return QString(ipv6Adr).replace(start, len, ':');

        return QString("::");
    }

    return ipv6Adr;
}

void NetworkDetails::updateData(const QJsonObject &info)
{
    m_items.clear();
    m_name.clear();
    const QString type = info.value("ConnectionType").toString();
    const bool isHotspot = type == "wireless-hotspot";
    const bool isWireless = type == "wireless";
    QJsonObject hotspotInfo = info.value("Hotspot").toObject();
    if (isHotspot) {
        m_name = tr("Hotspot");
        appendInfo(tr("SSID"), hotspotInfo.value("Ssid").toString());
    } else {
        m_name = info.value("ConnectionName").toString();
    }

    if (isWireless) {
        // 协议
        const QString &protocol = info.value("Protocol").toString();
        if (!protocol.isEmpty())
            appendInfo(tr("Protocol"), protocol);

        // 安全类型
        const QString &securityType = info.value("Security").toString();
        appendInfo(tr("Security Type"), securityType);

        // 频段
        const QString &band = hotspotInfo.value("Band").toString();
        QString bandInfo = band == "a" ? "5G" : (band == "bg" ? "2.4G" : "automatic");
        appendInfo(tr("Band"), bandInfo);

        // 网络通道
        const QString &channel = QString::number(hotspotInfo.value("Channel").toInt());
        if (!channel.isEmpty())
            appendInfo(tr("Channel"), channel);
    }

    if (isHotspot) {
        const QString securityType = info.value("Security").toString();
        appendInfo(tr("Security Type"), securityType);
    }
    // 接口
    const auto device = info.value("DeviceInterface").toString();
    if (!device.isEmpty())
        appendInfo(tr("Interface"), device);
    // MAC地址
    const QString mac = info.value("HwAddress").toString();
    if (!mac.isEmpty())
        appendInfo(tr("MAC"), mac);
    // 频段
    if (isHotspot) {
        const QString band = hotspotInfo.value("Band").toString();
        appendInfo(tr("Band"), band);
    } else {
        // ipv4
        const auto ipv4 = info.value("Ip4").toObject();
        if (!ipv4.isEmpty()) {
            // ipv4 地址
            const auto ip4Addr = ipv4.value("Address").toString();
            if (!ip4Addr.isEmpty())
                appendInfo(tr("IPv4"), ip4Addr);
            // ipv4 网关
            const auto gateway = ipv4.value("Gateways").toArray();
            if (!gateway.isEmpty())
                appendInfo(tr("Gateway"), gateway.first().toString());
            // ipv4 首选DNS
            const auto ip4PrimaryDns = ipv4.value("Dnses").toArray();
            if (!ip4PrimaryDns.isEmpty())
                appendInfo(tr("Primary DNS"), ip4PrimaryDns.first().toString());
            // ipv4 子网掩码
            const auto ip4Netmask = ipv4.value("Mask").toString();
            if (!ip4Netmask.isEmpty())
                appendInfo(tr("Netmask"), ip4Netmask);
        }
        // ipv6
        const auto ipv6 = info.value("Ip6").toObject();
        if (!ipv6.isEmpty()) {
            appendInfo(tr("IPv6"), compressedIpv6Addr(ipv6Infomation(info, InfoType::Ip)));
            appendInfo(tr("Gateway"), compressedIpv6Addr(ipv6Infomation(info, InfoType::Gateway)));

            // ipv6 首选DNS
            const auto ip6PrimaryDns = ipv6.value("Dnses").toArray();
            if (!ip6PrimaryDns.isEmpty())
                appendInfo(tr("Primary DNS"), compressedIpv6Addr(ip6PrimaryDns.first().toString()));
            // ipv6 子网掩码
            const auto ip6Prefix = ipv6.value("Prefix").toString();
            if (!ip6Prefix.isEmpty())
                appendInfo(tr("Prefix"), ip6Prefix);
        }
        // 速率
        const QString speed = info.value("Speed").toString();
        if (!speed.isEmpty())
            appendInfo(tr("Speed"), speed);
    }
}

void NetworkDetails::appendInfo(const QString &title, const QString &value)
{
    m_items << qMakePair(title, value);
}

QString NetworkDetails::ipv6Infomation(QJsonObject connectinfo, NetworkDetails::InfoType type)
{
    NetworkManager::Connection::Ptr connection = findConnectionByUuid(connectinfo.value("ConnectionUuid").toString());
    if (connection.isNull())
        return QString();

    ConnectionSettings::Ptr connectionSettings = connection->settings();
    Ipv6Setting::Ptr ipv6Setting = connectionSettings->setting(Setting::Ipv6).staticCast<Ipv6Setting>();
    if (ipv6Setting.isNull())
        return QString();

    QList<IpAddress> addressInfos = ipv6Setting->addresses();
    if (ipv6Setting->method() == Ipv6Setting::Manual) {
        if (addressInfos.count() == 0) {
            qDebug() << "ipv6Setting error! ";
            return QString();
        }

        switch (type) {
        case Ip:
            return addressInfos[0].ip().toString();
        case Gateway:
            return addressInfos[0].gateway().toString();
        }
    }

    if (ipv6Setting->method() == Ipv6Setting::Automatic) {
        const auto ipv6 = connectinfo.value("Ip6").toObject();
        switch (type) {
        case Ip:
            return ipv6.value("Address").toString();
        case Gateway:
            return ipv6.value("Address").toString();
        }
    }

    return QString();
}
