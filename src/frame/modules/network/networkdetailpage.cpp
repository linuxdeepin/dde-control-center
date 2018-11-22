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
#include "widgets/settingsgroup.h"
#include "widgets/settingsheaderitem.h"
#include "widgets/translucentframe.h"
#include "widgets/titlevalueitem.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QJsonArray>

#include <networkmodel.h>

using namespace dcc::widgets;
using namespace dde::network;

const QString compressedIpv6Addr(const QString &ipv6Adr)
{
    if (ipv6Adr.contains("::"))
        return ipv6Adr;

    // NOTE(sbw): Calculate longest zero sequence, ensure
    int start = 0, len = 0;
    int maxStart = 0, maxLen = 0;
    const auto &sequence = ipv6Adr.split(':');
    for (int i(0); i != sequence.size(); ++i)
    {
        Q_ASSERT(sequence[i].size() == 4);

        if (sequence[i] == "0000")
        {
            len += 5;
        } else {
            if (len > maxLen)
            {
                maxStart = start;
                maxLen = len;
            }

            start = (i + 1) * 5;
            len = 0;
        }
    }

    if (maxLen)
        return QString(ipv6Adr).replace(maxStart, maxLen, ':');
    else if (len > maxLen)
        if (start)
            return QString(ipv6Adr).replace(start, len, ':');
        else
            return QString("::");
    else
        return ipv6Adr;
}

namespace dcc {

namespace network {

NetworkDetailPage::NetworkDetailPage(QWidget *parent)
    : ContentWidget(parent)
{
    m_groupsLayout = new QVBoxLayout;
    m_groupsLayout->setSpacing(0);
    m_groupsLayout->setMargin(0);

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(m_groupsLayout);

    setTitle(tr("Network Details"));
    setContent(mainWidget);
}

void NetworkDetailPage::setModel(NetworkModel *model)
{
    connect(model, &NetworkModel::activeConnInfoChanged, this, &NetworkDetailPage::onActiveInfoChanged);

    onActiveInfoChanged(model->activeConnInfos());
}

void NetworkDetailPage::onActiveInfoChanged(const QList<QJsonObject> &infos)
{
    auto appendInfo = [](SettingsGroup *g, const QString &t, const QString &v)
    {
        TitleValueItem *i = new TitleValueItem;
        i->setTitle(t);
        i->setValue(v);
        g->appendItem(i);
        if (t == "IPv6") {
            i->setWordWrap(false);
        }
    };


    // clear old infos
    while (QLayoutItem *item = m_groupsLayout->takeAt(0))
    {
        if (item->widget())
            item->widget()->deleteLater();
        delete item;
    }

    m_groupsLayout->addSpacing(10);

    for (const auto &info : infos)
    {
        SettingsGroup *grp = new SettingsGroup;

        const QString type = info.value("ConnectionType").toString();
        const bool isHotspot = type == "wireless-hotspot";
        QJsonObject hotspotInfo;
        if (isHotspot)
        {
            grp->setHeaderVisible(true);
            grp->headerItem()->setTitle(tr("Hotspot"));

            hotspotInfo = info.value("Hotspot").toObject();

            const QString ssid = hotspotInfo.value("Ssid").toString();
            appendInfo(grp, tr("SSID"), ssid);
        } else {
            const QString name = info.value("ConnectionName").toString();
            appendInfo(grp, name, "");
        }

        // encrypt method
        if (isHotspot)
        {
            const QString securityType = info.value("Security").toString();
            appendInfo(grp, tr("Security"), securityType);
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
        if (isHotspot)
        {
            const QString band = hotspotInfo.value("Band").toString();
            appendInfo(grp, tr("Band"), band);
        } else {
            // ipv4 info
            const auto ipv4 = info.value("Ip4").toObject();
            if (!ipv4.isEmpty())
            {
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
            if (!ipv6.isEmpty())
            {
                // ipv6 address
                const auto ip6Addr = ipv6.value("Address").toString();
                if (!ip6Addr.isEmpty())
                    appendInfo(grp, tr("IPv6"), compressedIpv6Addr(ip6Addr));

                // ipv6 gateway
                const auto gateway = ipv6.value("Gateways").toArray();
                if (!gateway.isEmpty())
                    appendInfo(grp, tr("Gateway"), compressedIpv6Addr(gateway.first().toString()));

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
        m_groupsLayout->addSpacing(10);
    }

    m_groupsLayout->addStretch();
}

}

}
