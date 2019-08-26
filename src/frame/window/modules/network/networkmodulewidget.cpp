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
#include "window/utils.h"
#include "widgets/nextpagewidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/switchwidget.h"

#include <DStyleOption>

#include <QDebug>

#include <networkmodel.h>
#include <networkdevice.h>
#include <wirelessdevice.h>

using namespace dcc::widgets;
using namespace DCC_NAMESPACE::network;
using namespace dde::network;

NetworkModuleWidget::NetworkModuleWidget()
    : QWidget()
    , m_lvnmpages(new DListView(this))
    , m_modelpages(new QStandardItemModel)
{
    setObjectName("Network");
    m_lvnmpages->setFrameShape(QFrame::NoFrame);
    m_lvnmpages->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_lvnmpages->setModel(m_modelpages);

    m_centralLayout = new QVBoxLayout();
    setMinimumWidth(250);
    setLayout(m_centralLayout);

#if !defined(DISABLE_NETWORK_PROXY) || !defined(DISABLE_NETWORK_VPN) || !defined(DISABLE_NETWORK_PPPOE)
#ifndef DISABLE_NETWORK_PPPOE
    DStandardItem *pppit = new DStandardItem(tr("DSL"));
    pppit->setData("ppp", SectionRole);
    pppit->setIcon(QIcon::fromTheme("dcc_dsl"));
    m_modelpages->appendRow(pppit);
#endif

#ifndef DISABLE_NETWORK_VPN
    DStandardItem *vpnit = new DStandardItem(tr("VPN"));
    vpnit->setData("vpn", SectionRole);
    vpnit->setIcon(QIcon::fromTheme("dcc_vpn"));
    m_modelpages->appendRow(vpnit);
#endif

#ifndef DISABLE_NETWORK_PROXY
    DStandardItem *prxyit = new DStandardItem(tr("System Proxy"));
    prxyit->setData("prxy", SectionRole);
    prxyit->setIcon(QIcon::fromTheme("dcc_system_agent"));
    m_modelpages->appendRow(prxyit);
    DStandardItem *aprxit = new DStandardItem(tr("Application Proxy"));
    aprxit->setData("aprx", SectionRole);
    aprxit->setIcon(QIcon::fromTheme("dcc_app_proxy"));
    m_modelpages->appendRow(aprxit);
#endif
#endif
    DStandardItem *infoit = new DStandardItem(tr("Network Details"));
    infoit->setData("info", SectionRole);
    infoit->setIcon(QIcon::fromTheme("dcc_network"));
    m_modelpages->appendRow(infoit);
    m_centralLayout->addWidget(m_lvnmpages);

    connect(m_lvnmpages, &QListView::clicked, this, [this](const QModelIndex &idx) {
        QString type = idx.data(SectionRole).toString();
        if (type == "ppp") {
            Q_EMIT this->requestShowPppPage();
        }
        if (type == "vpn") {
            Q_EMIT this->requestShowVpnPage();
        }
        if (type == "prxy") {
            Q_EMIT this->requestShowProxyPage();
        }
        if (type == "aprx") {
            Q_EMIT this->requestShowChainsPage();
        }
        if (type == "info") {
            Q_EMIT this->requestShowInfomation();
        }
        if (type == "dev_wireless" || type == "dev_ether") {
            Q_EMIT this->requestShowDeviceDetail(idx.data(DeviceRole).value<NetworkDevice*>());
        }
    });
}

void NetworkModuleWidget::setModel(NetworkModel *model)
{
    connect(model, &NetworkModel::deviceListChanged, this, &NetworkModuleWidget::onDeviceListChanged);

#ifndef DISABLE_NETWORK_PROXY
    //connect(model, &NetworkModel::appProxyExistChanged, m_appProxy, &NextPageWidget::setVisible);
    //m_appProxy->setVisible(model->appProxyExist());
#endif

    onDeviceListChanged(model->devices());
}

void NetworkModuleWidget::onDeviceListChanged(const QList<NetworkDevice *> &devices)
{
    QList<QStandardItem*> devits;

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

        devits.push_back(createDeviceGroup(dev, ++count, wiredDevice > 1));
    }

    // add wireless device list
    count = 0;
    for (auto const dev : devices)
    {
        if (dev->type() != NetworkDevice::Wireless)
            continue;

        devits.push_back(createDeviceGroup(dev, ++count, wirelessDevice > 1));
    }
    for (auto it = devits.rbegin(); it!=devits.rend(); ++it) {
        m_modelpages->insertRow(0, *it);
    }
}

QStandardItem* NetworkModuleWidget::createDeviceGroup(NetworkDevice *dev, const int number, const bool multiple)
{
    QString text;

    Q_EMIT dev->statusChanged(dev->statusString());

    if (dev->type() == NetworkDevice::Wired)
    {
        if (multiple)
        {
            text = tr("Wired Network%1").arg(number);
        }
        else
        {
            text = tr("Wired Network");
        }
    }
    else if (dev->type() == NetworkDevice::Wireless)
    {
        /*WirelessDevice *wdev = static_cast<WirelessDevice *>(dev);
        if (wdev->supportHotspot())
        {
            NextPageWidget *hotspot = new NextPageWidget;
            hotspot->setTitle(tr("Hotspot"));

            connect(dev, &NetworkDevice::enableChanged, hotspot, &NextPageWidget::setVisible);
            connect(wdev, &WirelessDevice::hotspotEnabledChanged, this, &NetworkModuleWidget::onDevStatusChanged, Qt::QueuedConnection);
            connect(wdev, &WirelessDevice::hotspotEnabledChanged, hotspot, [=] (const bool enabled) { hotspot->setValue(enabled ? tr("Shared") : QString()); });
            connect(hotspot, &NextPageWidget::clicked, this, [=] { Q_EMIT requestHotspotPage(wdev); });

            hotspot->setVisible(devEnabled);
            Q_EMIT wdev->hotspotEnabledChanged(wdev->hotspotEnabled());
        }*/

        if (multiple)
        {
            text = tr("Wireless Network%1").arg(number);
        }
        else
        {
            text = tr("Wireless Network");
        }
    }
    DStandardItem *ret = new DStandardItem(text);
    ret->setData(dev->type() == NetworkDevice::Wireless ? "dev_wireless" : "dev_ether", SectionRole);
    //TODO: add icon for ethernet
    ret->setIcon(QIcon::fromTheme(dev->type() == NetworkDevice::Wireless ? "dcc_wifi" : ""));
    ret->setData(QVariant::fromValue(dev), DeviceRole);

    return ret;
}
