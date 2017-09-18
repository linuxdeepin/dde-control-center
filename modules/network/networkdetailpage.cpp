/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
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
#include "networkmodel.h"
#include "settingsgroup.h"
#include "settingsheaderitem.h"
#include "translucentframe.h"
#include "titlevalueitem.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QJsonArray>

using namespace dcc::widgets;

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
//        qDebug() << info;

        SettingsGroup *grp = new SettingsGroup;

        const QString name = info.value("ConnectionName").toString();
        if (!name.isEmpty())
            appendInfo(grp, name, "");

        // mac info
        const QString mac = info.value("HwAddress").toString();
        if (!mac.isEmpty())
            appendInfo(grp, tr("MAC"), mac);

        // ipv4 info
        const auto ipv4 = info.value("Ip4").toObject();
        if (!ipv4.isEmpty())
        {
            // ipv4 address
            const auto ip4Addr = ipv4.value("Address").toString();
            if (!ip4Addr.isEmpty())
                appendInfo(grp, tr("IPv4 Address"), ip4Addr);

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

        // device interface
        const auto device = info.value("DeviceInterface").toString();
        if (!device.isEmpty())
            appendInfo(grp, tr("Interface"), device);

        // speed info
        const QString speed = info.value("Speed").toString();
        if (!speed.isEmpty())
            appendInfo(grp, tr("Speed"), speed);

        m_groupsLayout->addWidget(grp);
        m_groupsLayout->addSpacing(10);
    }

    m_groupsLayout->addStretch();
}

}

}
