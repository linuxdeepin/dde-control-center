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

#include "networkmodulewidget.h"
#include "widgets/nextpagewidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/switchwidget.h"

#include <networkmodel.h>
#include <networkdevice.h>
#include <wirelessdevice.h>
#include <QDebug>

using namespace dcc::widgets;
using namespace dcc::network;
using namespace dde::network;

NetworkModuleWidget::NetworkModuleWidget()
    : ModuleWidget()
    , m_devicesLayout(new QVBoxLayout)
#ifndef DISABLE_NETWORK_PPPOE
    , m_pppBtn(new NextPageWidget)
#endif
#ifndef DISABLE_NETWORK_VPN
    , m_vpnBtn(new NextPageWidget)
#endif
#ifndef DISABLE_NETWORK_PROXY
    , m_proxyBtn(new NextPageWidget)
#endif
    , m_detailBtn(new NextPageWidget)
{
    setObjectName("Network");

#ifndef DISABLE_NETWORK_PPPOE
    m_pppBtn->setTitle(tr("DSL"));
#endif

#ifndef DISABLE_NETWORK_VPN
    m_vpnBtn->setTitle(tr("VPN"));
#endif

#ifndef DISABLE_NETWORK_PROXY
    m_proxyBtn->setTitle(tr("System Proxy"));
    m_appProxy = new NextPageWidget;
    m_appProxy->setTitle(tr("Application Proxy"));
#endif

    m_detailBtn->setTitle(tr("Network Details"));

    m_devicesLayout->setMargin(0);
    m_devicesLayout->setSpacing(10);
    m_centralLayout->addLayout(m_devicesLayout);

#if !defined(DISABLE_NETWORK_PROXY) || !defined(DISABLE_NETWORK_VPN) || !defined(DISABLE_NETWORK_PPPOE)
    SettingsGroup *connGroup = new SettingsGroup;
#ifndef DISABLE_NETWORK_PPPOE
    connGroup->appendItem(m_pppBtn);
#endif

#ifndef DISABLE_NETWORK_VPN
    connGroup->appendItem(m_vpnBtn);
#endif

#ifndef DISABLE_NETWORK_PROXY
    connGroup->appendItem(m_appProxy);
    connGroup->appendItem(m_proxyBtn);
#endif
    m_centralLayout->addWidget(connGroup);
#endif

    SettingsGroup *detailGroup = new SettingsGroup;
    detailGroup->appendItem(m_detailBtn);
    m_centralLayout->addWidget(detailGroup);

    setTitle(tr("Network"));

    connect(m_detailBtn, &NextPageWidget::clicked, this, &NetworkModuleWidget::requestShowInfomation);

#ifndef DISABLE_NETWORK_VPN
    connect(m_vpnBtn, &NextPageWidget::clicked, this, &NetworkModuleWidget::requestShowVpnPage);
#endif

#ifndef DISABLE_NETWORK_PPPOE
    connect(m_pppBtn, &NextPageWidget::clicked, this, &NetworkModuleWidget::requestShowPppPage);
#endif

#ifndef DISABLE_NETWORK_PROXY
    connect(m_proxyBtn, &NextPageWidget::clicked, this, &NetworkModuleWidget::requestShowProxyPage);
    connect(m_appProxy, &NextPageWidget::clicked, this, &NetworkModuleWidget::requestShowChainsPage);
#endif
}

void NetworkModuleWidget::setModel(NetworkModel *model)
{
    connect(model, &NetworkModel::deviceListChanged, this, &NetworkModuleWidget::onDeviceListChanged);

#ifndef DISABLE_NETWORK_PROXY
    connect(model, &NetworkModel::appProxyExistChanged, m_appProxy, &NextPageWidget::setVisible);
    m_appProxy->setVisible(model->appProxyExist());
#endif

    onDeviceListChanged(model->devices());
}

void NetworkModuleWidget::onDeviceListChanged(const QList<NetworkDevice *> &devices)
{
    bool recreate = false;
    const auto devs = m_devicesWidgets.values().toSet();

    if (devs.size() != devices.size())
    {
        recreate = true;
    } else {
        for (auto *dev : devices)
        {
            if (!devs.contains(dev))
                recreate = true;
            break;
        }
    }
    if (!recreate)
        return;

    // remove old widgets
    while (QLayoutItem *item = m_devicesLayout->takeAt(0))
    {
        item->widget()->deleteLater();
        delete item;
    }
    m_devicesWidgets.clear();

    int wiredDevice = 0;
    int wirelessDevice = 0;
    for (auto const dev : devices)
        switch (dev->type())
        {
        case NetworkDevice::Wired:      ++wiredDevice;      break;
        case NetworkDevice::Wireless:   ++wirelessDevice;   break;
        default:;
        }

    // add wired device list
    int count = 0;
    for (auto const dev : devices)
    {
        if (dev->type() != NetworkDevice::Wired)
            continue;

        createDeviceGroup(dev, ++count, wiredDevice > 1);
    }

    // add wireless device list
    count = 0;
    for (auto const dev : devices)
    {
        if (dev->type() != NetworkDevice::Wireless)
            continue;

        createDeviceGroup(dev, ++count, wirelessDevice > 1);
    }
}

void NetworkModuleWidget::onDevStatusChanged()
{
    NetworkDevice *dev = static_cast<NetworkDevice *>(sender());
    NextPageWidget *w = m_devicesWidgets.key(dev);
    if (!w || !dev)
        return;

    if (dev->type() == NetworkDevice::Wireless)
    {
        WirelessDevice *wdev = static_cast<WirelessDevice *>(dev);

        if (wdev->hotspotEnabled())
            return w->setValue(QString());
    }

    w->setValue(dev->statusString());
}

void NetworkModuleWidget::onNextPageClicked()
{
    NextPageWidget *w = qobject_cast<NextPageWidget *>(sender());
    Q_ASSERT(w);

    NetworkDevice *dev = m_devicesWidgets[w];
    Q_ASSERT(dev);

    Q_EMIT requestShowDeviceDetail(dev);
}

void NetworkModuleWidget::createDeviceGroup(NetworkDevice *dev, const int number, const bool multiple)
{
    SwitchWidget *s = new SwitchWidget;
    NextPageWidget *w = new NextPageWidget;
    SettingsGroup *g = new SettingsGroup;
    g->appendItem(s);
    g->appendItem(w);

    connect(s, &SwitchWidget::checkedChanged, [=](const bool checked) { Q_EMIT requestDeviceEnable(dev->path(), checked); });
    connect(w, &NextPageWidget::clicked, this, &NetworkModuleWidget::onNextPageClicked);
    connect(dev, &NetworkDevice::enableChanged, s, &SwitchWidget::setChecked);
    connect(dev, &NetworkDevice::enableChanged, w, &NextPageWidget::setVisible);
    connect(dev, static_cast<void (NetworkDevice::*)(const QString &) const>(&NetworkDevice::statusChanged), this, &NetworkModuleWidget::onDevStatusChanged, Qt::QueuedConnection);

    const bool devEnabled = dev->enabled();
    s->setChecked(devEnabled);
    w->setVisible(devEnabled);
    Q_EMIT dev->statusChanged(dev->statusString());

    if (dev->type() == NetworkDevice::Wired)
    {
        if (multiple)
        {
            s->setTitle(tr("Wired Network Card%1").arg(number));
            w->setTitle(tr("Wired Network%1").arg(number));
        }
        else
        {
            s->setTitle(tr("Wired Network Card"));
            w->setTitle(tr("Wired Network"));
        }
    }
    else if (dev->type() == NetworkDevice::Wireless)
    {
        WirelessDevice *wdev = static_cast<WirelessDevice *>(dev);
        if (wdev->supportHotspot())
        {
            NextPageWidget *hotspot = new NextPageWidget;
            hotspot->setTitle(tr("Hotspot"));
            g->appendItem(hotspot);

            connect(dev, &NetworkDevice::enableChanged, hotspot, &NextPageWidget::setVisible);
            connect(wdev, &WirelessDevice::hotspotEnabledChanged, this, &NetworkModuleWidget::onDevStatusChanged, Qt::QueuedConnection);
            connect(wdev, &WirelessDevice::hotspotEnabledChanged, hotspot, [=] (const bool enabled) { hotspot->setValue(enabled ? tr("Shared") : QString()); });
            connect(hotspot, &NextPageWidget::clicked, this, [=] { Q_EMIT requestHotspotPage(wdev); });

            hotspot->setVisible(devEnabled);
            Q_EMIT wdev->hotspotEnabledChanged(wdev->hotspotEnabled());
        }

        if (multiple)
        {
            s->setTitle(tr("Wireless Network Card%1").arg(number));
            w->setTitle(tr("Wireless Network%1").arg(number));
        }
        else
        {
            s->setTitle(tr("Wireless Network Card"));
            w->setTitle(tr("Wireless Network"));
        }
    }

    m_devicesWidgets[w] = dev;
    m_devicesLayout->addWidget(g);
}
