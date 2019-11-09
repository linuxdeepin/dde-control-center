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
#include "wirelesspage.h"
#include "vpnpage.h"
#include "pppoepage.h"
#include "proxypage.h"
#include "networkdetailpage.h"
#include "wiredpage.h"
#include "hotspotpage.h"
#include "chainsproxypage.h"
#include "chainstypepage.h"
#include "connectionwirelesseditpage.h"
#include "connectioneditpage.h"
#include "widgets/contentwidget.h"
#include "modules/modulewidget.h"

#include <networkworker.h>
#include <networkmodel.h>
#include <networkdevice.h>
#include <wirelessdevice.h>
#include <wireddevice.h>

#include <QTimer>

using namespace dcc;
using namespace DCC_NAMESPACE;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::network;
using namespace dde::network;

NetworkModule::NetworkModule(DCC_NAMESPACE::FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
    , m_networkModel(nullptr)
    , m_networkWorker(nullptr)
    , m_networkWidget(nullptr)
    , m_connEditPage(nullptr)
    , m_initSettingTimer(new QTimer(this))
{
    ConnectionEditPage::setFrameProxy(frame);
}

NetworkModule::~NetworkModule()
{
    m_networkModel->deleteLater();
    m_networkWorker->deleteLater();
}

void NetworkModule::showPage(const QString &jsonData)
{
    // Json data format:
    // Required Keys : conn-type, device-path
    // Optional Keys : conn-uuid, ap-path
    // Valid values of key "conn-type" : wired, wireless, vpn，pppoe (vpn and pppoe have not been supperted for now)

    // Tips:
    // - a new Connection will be created if "conn-uuid" is empty
    // - the value of "ap-path" is path/SettingPath of connecting ap
    // - "ap-path" must not be empty if "conn-uuid" is empty while creating or editing a wireless connection

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData.toLocal8Bit());
    if (!jsonDoc.isEmpty()) {
        QJsonObject jsonObj = jsonDoc.object();
        const QString &devPath = jsonObj.value("device-path").toString();
        const QString &connType = jsonObj.value("conn-type").toString();

        dde::network::NetworkDevice *device = nullptr;
        for (auto dev : m_networkModel->devices()) {
            if (dev->path() == devPath) {
                device = dev;
                break;
            }
        }

        if (device == nullptr) {
            qDebug() << "can not fand device by device-path:" << jsonDoc;
            return;
        }

        const QString &connUuid = jsonObj.value("conn-uuid").toString();
        if (connType == "wired") {
            showWiredEditPage(device, connUuid);
        } else if (connType == "wireless") {
            const QString &apPath = jsonObj.value("ap-path").toString();
            showWirelessEditPage(device, connUuid, apPath);
        } else {
            qDebug() << "unhandled device type:" << jsonDoc;
            return;
        }

        return;
    }

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

void NetworkModule::active()
{
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
    m_frameProxy->pushWidget(this, m_networkWidget);
    m_initSettingTimer->setInterval(100);
    m_initSettingTimer->setSingleShot(true);
    m_initSettingTimer->start();
    connect(m_initSettingTimer, &QTimer::timeout, this, [this] {
        m_networkWidget->initSetting(0);
    });
}

int NetworkModule::load(QString path)
{
    if (!m_networkWidget) {
        active();
    }

    if (m_initSettingTimer) {
        m_initSettingTimer->stop();
    }
    for (NetworkDevice *dev: m_networkModel->devices()) {
        if (dev->path() == path) {
            showDeviceDetailPage(dev);
            m_networkWidget->setIndexFromPath(path);
            return 0;
        }
    }
    int index = m_networkWidget->gotoSetting(path);
    QTimer::singleShot(120, this, [ = ] {
        m_networkWidget->initSetting(index == -1 ? 0 : index);
    });
    return index == -1 ? -1 : 0;
}

const QString NetworkModule::name() const
{
    return QStringLiteral("network");
}

void NetworkModule::popPage()
{
    m_frameProxy->popWidget(this);
}

void NetworkModule::showDeviceDetailPage(NetworkDevice *dev)
{
    ContentWidget *p = nullptr;

    if (dev->type() == NetworkDevice::Wireless) {
        p = new WirelessPage(static_cast<WirelessDevice *>(dev));
        m_networkWorker->queryDeviceStatus(dev->path());
        WirelessPage *wirelessPage = static_cast<WirelessPage *>(p);
        connect(wirelessPage, &WirelessPage::back, this, &NetworkModule::popPage);
        connect(wirelessPage, &WirelessPage::requestDeviceAPList, m_networkWorker, &NetworkWorker::queryAccessPoints);
        connect(wirelessPage, &WirelessPage::requestWirelessScan, m_networkWorker, &NetworkWorker::requestWirelessScan);
        connect(wirelessPage, &WirelessPage::requestConnectAp, m_networkWorker, &NetworkWorker::activateAccessPoint);
        connect(wirelessPage, &WirelessPage::requestDeviceEnabled, m_networkWorker, &NetworkWorker::setDeviceEnable);
        connect(wirelessPage, &WirelessPage::requestDisconnectConnection, m_networkWorker, &NetworkWorker::deactiveConnection);
        connect(wirelessPage, &WirelessPage::requestDeviceRemanage, m_networkWorker, &NetworkWorker::remanageDevice, Qt::QueuedConnection);
        connect(wirelessPage, &WirelessPage::requestNextPage, [ = ](ContentWidget * const w) {
            m_frameProxy->pushWidget(this, w, dccV20::FrameProxyInterface::PushType::CoverTop);
        });
        connect(wirelessPage, &WirelessPage::requestFrameKeepAutoHide, this, &NetworkModule::onSetFrameAutoHide);

        wirelessPage->setModel(m_networkModel);
    } else if (dev->type() == NetworkDevice::Wired) {
        p = new WiredPage(static_cast<WiredDevice *>(dev));

        WiredPage *wiredPage = static_cast<WiredPage *>(p);

//        connect(p, &WiredPage::requestConnectionsList, m_networkWorker, &NetworkWorker::queryDeviceConnections);
        connect(wiredPage, &WiredPage::requestDeviceEnabled, m_networkWorker, &NetworkWorker::setDeviceEnable);
        connect(wiredPage, &WiredPage::requestActiveConnection, m_networkWorker, &NetworkWorker::activateConnection);
        connect(wiredPage, &WiredPage::back, this, &NetworkModule::popPage);
        connect(wiredPage, &WiredPage::requestNextPage, [ = ](ContentWidget * const w) {
            m_frameProxy->pushWidget(this, w, dccV20::FrameProxyInterface::PushType::CoverTop);
        });
        connect(wiredPage, &WiredPage::requestFrameKeepAutoHide, this, &NetworkModule::onSetFrameAutoHide);

        wiredPage->setModel(m_networkModel);
    }

    if (!p) {
        return;
    }

    p->layout()->setMargin(0);
    m_frameProxy->pushWidget(this, p);
}

void NetworkModule::showVpnPage()
{
    VpnPage *p = new VpnPage;

    connect(p, &VpnPage::requestVpnEnabled, m_networkWorker, &NetworkWorker::setVpnEnable);
    connect(p, &VpnPage::requestActivateConnection, m_networkWorker, &NetworkWorker::activateConnection);
    connect(p, &VpnPage::requestNextPage, [ = ](ContentWidget * const w) {
        m_frameProxy->pushWidget(this, w, dccV20::FrameProxyInterface::PushType::CoverTop);
    });
    connect(p, &VpnPage::requestFrameKeepAutoHide, this, &NetworkModule::onSetFrameAutoHide);

    p->setModel(m_networkModel);

    m_frameProxy->pushWidget(this, p);
}

void NetworkModule::showPppPage()
{
    PppoePage *p = new PppoePage;

    connect(p, &PppoePage::requestActivateConnection, m_networkWorker, &NetworkWorker::activateConnection);
    connect(p, &PppoePage::requestNextPage, [ = ](ContentWidget * const w) {
        m_frameProxy->pushWidget(this, w, dccV20::FrameProxyInterface::PushType::CoverTop);
    });
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

void NetworkModule::showDetailPage()
{
    NetworkDetailPage *p = new NetworkDetailPage;

    p->setModel(m_networkModel);

    m_frameProxy->pushWidget(this, p);
}

void NetworkModule::showHotspotPage()
{
    HotspotPage *p = new HotspotPage();

    p->setModel(m_networkModel);
    connect(p, &HotspotPage::requestActivateConnection, m_networkWorker, &NetworkWorker::activateConnection);
    connect(p, &HotspotPage::requestDisconnectConnection, m_networkWorker, &NetworkWorker::deactiveConnection);
    connect(p, &HotspotPage::requestDeviceRemanage, m_networkWorker, &NetworkWorker::remanageDevice, Qt::QueuedConnection);
    connect(p, &HotspotPage::requestNextPage, [ = ](ContentWidget * const w) {
        m_frameProxy->pushWidget(this, w, dccV20::FrameProxyInterface::PushType::CoverTop);
    });

    m_frameProxy->pushWidget(this, p);
}

void NetworkModule::onSetFrameAutoHide(const bool autoHide)
{
    //m_frameProxy->setFrameAutoHide(this, autoHide);
}

void NetworkModule::showWiredEditPage(NetworkDevice *dev, const QString &connUuid)
{
    // it will be destroyed by Frame
    m_connEditPage = new ConnectionEditPage(ConnectionEditPage::ConnectionType::WiredConnection, dev->path(), connUuid);

    connect(m_connEditPage, &ConnectionEditPage::requestNextPage, [ = ](ContentWidget * const w) {
        m_frameProxy->pushWidget(this, w, dccV20::FrameProxyInterface::PushType::CoverTop);
    });
    connect(m_connEditPage, &ConnectionEditPage::requestFrameAutoHide, this, &NetworkModule::onSetFrameAutoHide);
    connect(m_connEditPage, &ConnectionEditPage::back, this, [ = ]() {m_connEditPage = nullptr; });
    connect(dev, &dde::network::NetworkDevice::removed, this, [ = ]() {
        removeConnEditPageByDevice(dev);
    });

    m_connEditPage->initSettingsWidget();

    m_frameProxy->pushWidget(this, m_connEditPage);
}

void NetworkModule::showWirelessEditPage(dde::network::NetworkDevice *dev, const QString &connUuid, const QString &apPath)
{
    // it will be destroyed by Frame
    m_connEditPage = new ConnectionWirelessEditPage(dev->path(), connUuid);

    connect(m_connEditPage, &ConnectionEditPage::requestNextPage, [ = ](ContentWidget * const w) {
        m_frameProxy->pushWidget(this, w);
    });
    connect(m_connEditPage, &ConnectionEditPage::requestFrameAutoHide, this, &NetworkModule::onSetFrameAutoHide);
    connect(m_connEditPage, &ConnectionEditPage::back, this, [ = ]() {
        m_connEditPage = nullptr;
    });
    connect(dev, &dde::network::NetworkDevice::removed, this, [ = ]() {
        removeConnEditPageByDevice(dev);
    });

    if (connUuid.isEmpty()) {
        if (apPath.isEmpty()) {
            qDebug() << "show edit page failed! both of connUuid and apPath are empty";
            m_connEditPage->deleteLater();
            return;
        } else {
            ConnectionWirelessEditPage *wirelessEditPage = static_cast<ConnectionWirelessEditPage *>(m_connEditPage);
            wirelessEditPage->initSettingsWidgetFromAp(apPath);
        }
    } else {
        m_connEditPage->initSettingsWidget();
    }

    m_frameProxy->pushWidget(this, m_connEditPage);
}

void NetworkModule::removeConnEditPageByDevice(dde::network::NetworkDevice *dev)
{
    if (m_connEditPage && dev->path() == m_connEditPage->devicePath()) {
        m_connEditPage->onDeviceRemoved();
        m_connEditPage = nullptr;
    }
}
