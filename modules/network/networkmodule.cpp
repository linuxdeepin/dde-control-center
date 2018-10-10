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
 *             listenerri <listenerri@gmail.com>
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

#include "networkmodule.h"
#include "networkmodulewidget.h"
#include "contentwidget.h"
#include "modulewidget.h"
#include "wirelesspage.h"
#include "vpnpage.h"
#include "pppoepage.h"
#include "proxypage.h"
#include "networkdetailpage.h"
#include "wiredpage.h"
#include "hotspotpage.h"
#include "chainsproxypage.h"
#include "chainstypepage.h"

#include <networkworker.h>
#include <networkmodel.h>
#include <networkdevice.h>
#include <wirelessdevice.h>
#include <wireddevice.h>

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::network;
using namespace dde::network;

NetworkModule::NetworkModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent),
      ModuleInterface(frame),

      m_networkModel(nullptr),
      m_networkWorker(nullptr),
      m_networkWidget(nullptr)
{
}

NetworkModule::~NetworkModule()
{
    m_networkModel->deleteLater();
    m_networkWorker->deleteLater();
}

void NetworkModule::showPage(const QString &pageName)
{
    Q_UNUSED(pageName)

    NetworkDevice *wireless = nullptr;

    for (NetworkDevice *dev : m_networkModel->devices()) {
        if (dev->type() == NetworkDevice::Wireless) {
            if (wireless)
                return;

            wireless = dev;
        }
    }

    if (!wireless)
        return;

    showDeviceDetailPage(wireless);
}

void NetworkModule::initialize()
{
    m_networkModel = new NetworkModel;
    m_networkWorker = new NetworkWorker(m_networkModel);

    m_networkModel->moveToThread(qApp->thread());
    m_networkWorker->moveToThread(qApp->thread());
}

void NetworkModule::reset()
{

}

void NetworkModule::moduleActive()
{
    m_networkWorker->active();
}

void NetworkModule::moduleDeactive()
{
    m_networkWorker->deactive();
}

void NetworkModule::contentPopped(ContentWidget * const w)
{
    w->deleteLater();
}

const QString NetworkModule::name() const
{
    return QStringLiteral("network");
}

ModuleWidget *NetworkModule::moduleWidget()
{
    if (m_networkWidget)
        return m_networkWidget;

    m_networkWidget = new NetworkModuleWidget;
    m_networkWidget->setModel(m_networkModel);
    connect(m_networkWidget, &NetworkModuleWidget::requestShowDeviceDetail, this, &NetworkModule::showDeviceDetailPage);
    connect(m_networkWidget, &NetworkModuleWidget::requestShowVpnPage, this, &NetworkModule::showVpnPage);
    connect(m_networkWidget, &NetworkModuleWidget::requestShowPppPage, this, &NetworkModule::showPppPage);
    connect(m_networkWidget, &NetworkModuleWidget::requestShowChainsPage, this, &NetworkModule::showChainsProxyPage);
    connect(m_networkWidget, &NetworkModuleWidget::requestShowProxyPage, this, &NetworkModule::showProxyPage);
    connect(m_networkWidget, &NetworkModuleWidget::requestHotspotPage, this, &NetworkModule::showHotspotPage);
    connect(m_networkWidget, &NetworkModuleWidget::requestShowInfomation, this, &NetworkModule::showDetailPage);
    connect(m_networkWidget, &NetworkModuleWidget::requestDeviceEnable, m_networkWorker, &NetworkWorker::setDeviceEnable);

    return m_networkWidget;
}

void NetworkModule::showDeviceDetailPage(NetworkDevice *dev)
{
    if (dev->type() == NetworkDevice::Wireless)
    {
        WirelessPage *p = new WirelessPage(static_cast<WirelessDevice *>(dev));
        connect(p, &WirelessPage::requestDeviceAPList, m_networkWorker, &NetworkWorker::queryAccessPoints);
        connect(p, &WirelessPage::requestWirelessScan, m_networkWorker, &NetworkWorker::requestWirelessScan);
        connect(p, &WirelessPage::requestConnectAp, m_networkWorker, &NetworkWorker::activateAccessPoint);
        connect(p, &WirelessPage::requestDeviceEnabled, m_networkWorker, &NetworkWorker::setDeviceEnable);
        connect(p, &WirelessPage::requestDisconnectConnection, m_networkWorker, &NetworkWorker::deactiveConnection);
        connect(p, &WirelessPage::requestDeviceRemanage, m_networkWorker, &NetworkWorker::remanageDevice, Qt::QueuedConnection);
        connect(p, &WirelessPage::requestNextPage, [=](ContentWidget * const w) { m_frameProxy->pushWidget(this, w); });
        connect(p, &WirelessPage::requestFrameKeepAutoHide, this, &NetworkModule::onSetFrameAutoHide);
        p->setModel(m_networkModel);

        m_frameProxy->pushWidget(this, p);
    }
    else if (dev->type() == NetworkDevice::Wired)
    {
        WiredPage *p = new WiredPage(static_cast<WiredDevice *>(dev));
//        connect(p, &WiredPage::requestConnectionsList, m_networkWorker, &NetworkWorker::queryDeviceConnections);
        connect(p, &WiredPage::requestActiveConnection, m_networkWorker, &NetworkWorker::activateConnection);
        connect(p, &WiredPage::requestNextPage, [=](ContentWidget * const w) { m_frameProxy->pushWidget(this, w); });
        connect(p, &WiredPage::requestFrameKeepAutoHide, this, &NetworkModule::onSetFrameAutoHide);
        p->setModel(m_networkModel);

        m_frameProxy->pushWidget(this, p);
    }
}

void NetworkModule::showVpnPage()
{
    VpnPage *p = new VpnPage;

    connect(p, &VpnPage::requestVpnEnabled, m_networkWorker, &NetworkWorker::setVpnEnable);
    connect(p, &VpnPage::requestActivateConnection, m_networkWorker, &NetworkWorker::activateConnection);
    connect(p, &VpnPage::requestNextPage, [=](ContentWidget * const w) { m_frameProxy->pushWidget(this, w); });
    connect(p, &VpnPage::requestFrameKeepAutoHide, this, &NetworkModule::onSetFrameAutoHide);

    p->setModel(m_networkModel);

    m_frameProxy->pushWidget(this, p);
}

void NetworkModule::showPppPage()
{
    PppoePage *p = new PppoePage;

    connect(p, &PppoePage::requestActivateConnection, m_networkWorker, &NetworkWorker::activateConnection);
    connect(p, &PppoePage::requestNextPage, [=](ContentWidget * const w) { m_frameProxy->pushWidget(this, w); });
    connect(p, &PppoePage::requestFrameKeepAutoHide, this, &NetworkModule::onSetFrameAutoHide);
    p->setModel(m_networkModel);

    m_frameProxy->pushWidget(this, p);
}

void NetworkModule::showChainsProxyPage()
{
    m_networkWorker->queryChains();

    ChainsProxyPage *chains = new ChainsProxyPage;
    chains->setModel(m_networkModel);

    connect(chains, &ChainsProxyPage::requestSet, m_networkWorker, &NetworkWorker::setChainsProxy);
    connect(chains, &ChainsProxyPage::requestShowTypePage, this, &NetworkModule::showChainsProxyTypePage);

    m_frameProxy->pushWidget(this, chains);
}

void NetworkModule::showProxyPage()
{
    ProxyPage *p = new ProxyPage;

    connect(p, &ProxyPage::requestQueryProxyData, m_networkWorker, &NetworkWorker::queryProxyData);
    connect(p, &ProxyPage::requestSetProxyMethod, m_networkWorker, &NetworkWorker::setProxyMethod);
    connect(p, &ProxyPage::requestSetIgnoreHosts, m_networkWorker, &NetworkWorker::setProxyIgnoreHosts);
    connect(p, &ProxyPage::requestSetProxy, m_networkWorker, &NetworkWorker::setProxy);
    connect(p, &ProxyPage::requestSetAutoProxy, m_networkWorker, &NetworkWorker::setAutoProxy);

    p->setModel(m_networkModel);

    m_frameProxy->pushWidget(this, p);
}

void NetworkModule::showChainsProxyTypePage()
{
    ChainsTypePage *page = new ChainsTypePage;
    page->setDefault(m_networkModel->getChainsProxy().type);

    connect(page, &ChainsTypePage::requestTypeChanged, m_networkWorker, &NetworkWorker::onChainsTypeChanged);

    m_frameProxy->pushWidget(this, page);
}

void NetworkModule::showDetailPage()
{
    NetworkDetailPage *p = new NetworkDetailPage;

    p->setModel(m_networkModel);

    m_frameProxy->pushWidget(this, p);
}

void NetworkModule::showHotspotPage(WirelessDevice *wdev)
{
    HotspotPage *p = new HotspotPage(wdev);

    p->setModel(m_networkModel);
    connect(p, &HotspotPage::requestActivateConnection, m_networkWorker, &NetworkWorker::activateConnection);
    connect(p, &HotspotPage::requestDisconnectConnection, m_networkWorker, &NetworkWorker::deactiveConnection);
    connect(p, &HotspotPage::requestDeviceRemanage, m_networkWorker, &NetworkWorker::remanageDevice, Qt::QueuedConnection);
    connect(p, &HotspotPage::requestNextPage, [=](ContentWidget * const w) { m_frameProxy->pushWidget(this, w); });

    m_frameProxy->pushWidget(this, p);
}

void NetworkModule::onSetFrameAutoHide(const bool autoHide)
{
    m_frameProxy->setFrameAutoHide(this, autoHide);
}
