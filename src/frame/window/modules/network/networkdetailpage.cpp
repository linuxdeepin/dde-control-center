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

#include "networkdetailpage.h"
#include "widgets/settingshead.h"
#include "widgets/settingsgroup.h"
#include "widgets/settingsheaderitem.h"
#include "widgets/translucentframe.h"
#include "widgets/titlevalueitem.h"
#include "window/utils.h"

#include <networkmanagerqt/settings.h>
#include <networkmanagerqt/connection.h>
#include <networkmanagerqt/connectionsettings.h>
#include <networkmanagerqt/ipv6setting.h>
#include <com_deepin_daemon_network.h>

#include <QVBoxLayout>
#include <QDebug>
#include <QJsonArray>
#include <DFontSizeManager>
#include <QDBusInterface>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingCall>
#include <QDBusPendingReply>

using namespace dcc::widgets;

const QString compressedIpv6Addr(const QString &ipv6Adr)
{
    if (ipv6Adr.contains("::"))
        return ipv6Adr;

    // NOTE(sbw): Calculate longest zero sequence, ensure
    int start = 0, len = 0;
    int maxStart = 0, maxLen = 0;
    const auto &sequence = ipv6Adr.split(':');
    for (int i = 0; i != sequence.size(); ++i) {
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

    if (maxLen) {
        return QString(ipv6Adr).replace(maxStart, maxLen, ':');
    } else if (len > maxLen) {
        if (start) {
            return QString(ipv6Adr).replace(start, len, ':');
        } else {
            return QString("::");
        }
    } else {
        return ipv6Adr;
    }
}

namespace DCC_NAMESPACE {
namespace network {

NetworkDetailPage::NetworkDetailPage(QWidget *parent)
    : ContentWidget(parent)
{
    m_groupsLayout = new QVBoxLayout;
    m_groupsLayout->setSpacing(0);
    m_groupsLayout->setMargin(0);
    m_groupsLayout->setContentsMargins(ThirdPageContentsMargins);

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(m_groupsLayout);

    setTitle(tr("Network Details"));
    layout()->setMargin(0);
    setContent(mainWidget);
}

void NetworkDetailPage::updateNetworkInfo()
{
   QDBusInterface *networkInter = new QDBusInterface("com.deepin.daemon.Network",
                                                     "/com/deepin/daemon/Network",
                                                     "com.deepin.daemon.Network",
                                                     QDBusConnection::sessionBus(), this);
   QDBusPendingCall async = networkInter->asyncCall("GetActiveConnectionInfo");
   QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(async, this);
   connect(w, &QDBusPendingCallWatcher::finished, this, [this, w](){
       QDBusPendingReply<QString> reply = *w;
       if (reply.isError()) {
           qDebug() << "GetActiveConnectionInfo error";
           return;
       }
       QList<QJsonObject> activeinfos;
       QJsonArray activeConns = QJsonDocument::fromJson(reply.value().toUtf8()).array();
       for (const auto info : activeConns)
       {
           const auto &connInfo = info.toObject();
           activeinfos << connInfo;
       }
       onActiveInfoChanged(activeinfos);
       w->deleteLater();
   });
}

void NetworkDetailPage::onActiveInfoChanged(const QList<QJsonObject> &infos)
{
    auto appendInfo = [](SettingsGroup *g, const QString &t, const QString &v) {
        TitleValueItem *i = new TitleValueItem;
        i->setTitle(t);
        i->setValue(v);
        g->appendItem(i);
        if (t == "IPv6") {
            i->setWordWrap(false);
        }
    };

    // clear old infos
    while (QLayoutItem *item = m_groupsLayout->takeAt(0)) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    int infoCount = infos.count();
    for (const auto &info : infos) {
        SettingsGroup *grp = new SettingsGroup;
        const QString type = info.value("ConnectionType").toString();
        const bool isHotspot = type == "wireless-hotspot";
        const bool isWireless = type == "wireless";
        const QJsonObject &hotspotInfo = info.value("Hotspot").toObject();

        // 设置活跃网络标题
        SettingsHead *head = new SettingsHead;
        head->setEditEnable(false);
        head->setContentsMargins(20,0,0,0);

        if (isHotspot) {
            head->setTitle(tr("Hotspot"));
            grp->appendItem(head, SettingsGroup::NoneBackground);

            const QString ssid = hotspotInfo.value("Ssid").toString();
            appendInfo(grp, tr("SSID"), ssid);
        } else {
            const QString name = info.value("ConnectionName").toString();
            head->setTitle(name);
            grp->appendItem(head, SettingsGroup::NoneBackground);
        }

        if (isWireless) {
            // protocol
            const QString &protocol = info.value("Protocol").toString();
            if (!protocol.isEmpty())
                appendInfo(grp, tr("Protocol"), protocol);

            // security type
            const QString &securityType = info.value("Security").toString();
            appendInfo(grp, tr("Security Type"), securityType);

            // band
            const QString &band = hotspotInfo.value("Band").toString();
            QString bandInfo = band == "a" ? "5G" : (band == "bg" ? "2.4G" : "automatic");
            appendInfo(grp, tr("Band"), bandInfo);

            // channel
            const QString &channel = QString::number(hotspotInfo.value("Channel").toInt());
            if (!channel.isEmpty())
                appendInfo(grp, tr("Channel"), channel);
        }

        // encrypt method
        if (isHotspot) {
            const QString securityType = info.value("Security").toString();
            appendInfo(grp, tr("Security Type"), securityType);
        }
        // device interface
        const auto device = info.value("DeviceInterface").toString();
        if (!device.isEmpty())
            appendInfo(grp, tr("Interface"), device);
        // mac info
        const QString mac = info.value("HwAddress").toString();
        if (!mac.isEmpty())
            appendInfo(grp, tr("MAC"), mac);
        // band
        if (isHotspot) {
            const QString band = hotspotInfo.value("Band").toString();
            appendInfo(grp, tr("Band"), band);
        } else {
            // ipv4 info
            const auto ipv4 = info.value("Ip4").toObject();
            if (!ipv4.isEmpty()) {
                // ipv4 address
                const auto ip4Addr = ipv4.value("Address").toString();
                if (!ip4Addr.isEmpty())
                    appendInfo(grp, tr("IPv4"), ip4Addr);
                // ipv4 gateway
                const auto gateway = ipv4.value("Gateways").toArray();
                if (!gateway.isEmpty())
                    appendInfo(grp, tr("Gateway"), gateway.first().toString());
                // ipv4 primary dns
                const auto ip4PrimaryDns = ipv4.value("Dnses").toArray();
                if (!ip4PrimaryDns.isEmpty())
                    appendInfo(grp, tr("Primary DNS"), ip4PrimaryDns.first().toString());
                // ipv4 netmask
                const auto ip4Netmask = ipv4.value("Mask").toString();
                if (!ip4Netmask.isEmpty())
                    appendInfo(grp, tr("Netmask"), ip4Netmask);
            }
            // ipv6 info
            const auto ipv6 = info.value("Ip6").toObject();
            if (!ipv6.isEmpty()) {
                appendInfo(grp, tr("IPv6"), compressedIpv6Addr(ipv6Infomation(info, NetworkDetailPage::Ip)));
                appendInfo(grp, tr("Gateway"), compressedIpv6Addr(ipv6Infomation(info, NetworkDetailPage::Gateway)));

                // ipv6 primary dns
                const auto ip6PrimaryDns = ipv6.value("Dnses").toArray();
                if (!ip6PrimaryDns.isEmpty())
                    appendInfo(grp, tr("Primary DNS"), compressedIpv6Addr(ip6PrimaryDns.first().toString()));
                // ipv6 netmask
                const auto ip6Prefix = ipv6.value("Prefix").toString();
                if (!ip6Prefix.isEmpty())
                    appendInfo(grp, tr("Prefix"), ip6Prefix);
            }
            // speed info
            const QString speed = info.value("Speed").toString();
            if (!speed.isEmpty())
                appendInfo(grp, tr("Speed"), speed);
        }
        m_groupsLayout->addWidget(grp);
        if (--infoCount > 0) {
            m_groupsLayout->addSpacing(30);
        }
    }

    m_groupsLayout->addStretch();
}

QString NetworkDetailPage::ipv6Infomation(QJsonObject connectinfo, NetworkDetailPage::InfoType type)
{
    NetworkManager::Connection::Ptr connection = findConnectionByUuid(connectinfo.value("ConnectionUuid").toString());
    if (connection.isNull())
        return "";
    NetworkManager::ConnectionSettings::Ptr connectionSettings = connection->settings();
    NetworkManager::Ipv6Setting::Ptr ipv6Setting = connectionSettings->setting(Setting::Ipv6).staticCast<NetworkManager::Ipv6Setting>();
    if (ipv6Setting.isNull())
        return "";
    QList<NetworkManager::IpAddress> addressInfos = ipv6Setting->addresses();
    if (ipv6Setting->method() == Ipv6Setting::Manual) {

        if (addressInfos.count() == 0) {
            qDebug() << "ipv6Setting error! ";
            return "";
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
    return "";
}
}
}
