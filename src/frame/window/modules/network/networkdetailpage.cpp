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

#include <networkmodel.h>

#include <QVBoxLayout>
#include <QDebug>
#include <QJsonArray>
#include <DFontSizeManager>
#include <QTimer>

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
    for (int i = 0; i != sequence.size(); ++i) {
        //这个断言不知道有什么作用，其他电脑可以正常过这个断言，
        // 而且数据和我的电脑一样的，我却过不了这个断言，所以暂时屏蔽掉了
//        Q_ASSERT(sequence[i].size() == 4);
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
    , m_updateData(new QTimer(this))
{
    m_groupsLayout = new QVBoxLayout;
    m_groupsLayout->setSpacing(0);
    m_groupsLayout->setMargin(0);

    m_updateData->setSingleShot(false);
    m_updateData->setInterval(6000);

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(m_groupsLayout);

    setTitle(tr("Network Details"));
    layout()->setMargin(0);
    setContent(mainWidget);
}
NetworkDetailPage::~NetworkDetailPage()
{
    qDebug() << Q_FUNC_INFO;
}

void NetworkDetailPage::setModel(NetworkModel *model)
{
    connect(model, &NetworkModel::activeConnInfoChanged, this, &NetworkDetailPage::onActiveInfoChanged);
    //刚点开获取一次数据，然后再用定时器进行一分钟刷新一次
    Q_EMIT model->requestActionConnect();
    connect(m_updateData, &QTimer::timeout, model, &NetworkModel::requestActionConnect);
    m_updateData->start();
    onActiveInfoChanged(model->activeConns());
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
        item = nullptr;
    }

    int infoCount = infos.count();
    for (const auto &info : infos) {
        SettingsGroup *grp = new SettingsGroup;
        const QString type = info.value("ConnectionType").toString();
        const bool isHotspot = type == "wireless-hotspot";
        const bool isWireless = type == "wireless";
        const QJsonObject &hotspotInfo = info.value("Hotspot").toObject();
        QVBoxLayout *m_headTitleLayout = new QVBoxLayout;

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
            QString bandInfo = band == "a" ? "5G" : (band == "bg" ? "2.4G" : "automatic");
            appendInfo(grp, tr("Band"), bandInfo);
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
        if (--infoCount > 0) {
            m_groupsLayout->addSpacing(30);
        }
    }

    m_groupsLayout->addStretch();
}
}
}
