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
    m_hasAp = false;
    m_hasWired = false;
    m_hasWireless = false;
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

void NetworkModule::onDeviceListChanged(const QList<dde::network::NetworkDevice *> &devices)
{
    // add wireless device list
    m_hasAp = false;
    m_hasWired = false;
    m_hasWireless = false;
    for (auto const dev : devices) {
        if (dev->type() == NetworkDevice::Wired) {
            m_hasWired = true;
        }
        if (dev->type() != NetworkDevice::Wireless)
            continue;
        m_hasWireless = true;
        if (qobject_cast<WirelessDevice *>(dev)->supportHotspot()) {
            m_hasAp = true;
        }
    }
    qDebug() << "[Network] device state : " << m_hasWired << "," << m_hasWireless << "," << m_hasAp;
    m_frameProxy->setRemoveableDeviceStatus(tr("Wired Network"), m_hasWired);
    m_frameProxy->setRemoveableDeviceStatus(tr("Wireless Network"), m_hasWireless);
    m_frameProxy->setRemoveableDeviceStatus(tr("Personal Hotspot"), m_hasAp);
}

void NetworkModule::preInitialize(bool sync , FrameProxyInterface::PushType pushtype)
{
    Q_UNUSED(pushtype);
    m_networkModel = new NetworkModel;
    m_networkWorker = new NetworkWorker(m_networkModel, nullptr, sync);

    m_networkModel->moveToThread(qApp->thread());
    m_networkWorker->moveToThread(qApp->thread());

    connect(m_networkModel, &NetworkModel::deviceListChanged, this, &NetworkModule::onDeviceListChanged);
    onDeviceListChanged(m_networkModel->devices());
}

void NetworkModule::initialize()
{

}

void NetworkModule::active()
{
    m_networkWidget = new NetworkModuleWidget;
    m_networkWidget->setVisible(false);
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
    m_networkWidget->setVisible(true);
    m_networkWidget->initSetting(0, "");
}

int NetworkModule::load(const QString &path)
{
    if (!m_networkWidget) {
        active();
    }

    if (m_initSettingTimer) {
        m_initSettingTimer->stop();
    }

    QStringList devPaths = path.split(",");
    if (devPaths.size() > 1) {
        for (NetworkDevice *dev: m_networkModel->devices()) {
            if (dev->path() == devPaths.at(0)) {
                showDeviceDetailPage(dev, devPaths.at(1));
                m_networkWidget->setIndexFromPath(devPaths.at(0));
                return 0;
            }
        }
    }

    for (NetworkDevice *dev: m_networkModel->devices()) {
        if (dev->path() == path) {
            showDeviceDetailPage(dev);
            qDebug() << "path" << path;
            m_networkWidget->setIndexFromPath(path);
            return 0;
        }
    }
    QStringList pathList = path.split("/");
    int index = m_networkWidget->gotoSetting(pathList.at(0));
    QTimer::singleShot(120, this, [ = ] {
        if (pathList.count() > 1) {
            m_networkWidget->initSetting(index == -1 ? 0 : index, pathList.at(1));
        } else {
            m_networkWidget->initSetting(index == -1 ? 0 : index, "");
        }
    });
    return index == -1 ? -1 : 0;
}

QStringList NetworkModule::availPage() const
{
    QStringList list;
    list << "DSL" << "DSL/Create PPPoE Connection" << "VPN" << "VPN/Create VPN" << "VPN/Import VPN"
         << "System Proxy" << "Application Proxy" << "Network Details";
    if (m_hasWired) {
        list << "Wired Network" << "Wired Network/Add Network Connection";
    }
    if (m_hasWireless) {
        list << "Wireless Network";
    }
    if (m_hasAp) {
        list << "Personal Hotspot";
    }
    for (NetworkDevice *dev: m_networkModel->devices()) {
        list << dev->path();
    }
    return list;
}

const QString NetworkModule::name() const
{
    return QStringLiteral("network");
}

const QString NetworkModule::displayName() const
{
    return tr("Network");
}

void NetworkModule::popPage()
{
    m_frameProxy->popWidget(this);
    m_networkWidget->initSetting(0, "");
}

void NetworkModule::showDeviceDetailPage(NetworkDevice *dev, const QString &searchPath)
{
    ContentWidget *p = nullptr;

    if (dev->type() == NetworkDevice::Wireless) {
        m_wirelessPage = new WirelessPage(static_cast<WirelessDevice *>(dev));
        m_wirelessPage->setVisible(false);
        p = m_wirelessPage;
        m_networkWorker->queryDeviceStatus(dev->path());
        connect(m_wirelessPage, &WirelessPage::back, this, &NetworkModule::popPage);
        connect(m_wirelessPage, &WirelessPage::requestDeviceAPList, m_networkWorker, &NetworkWorker::queryAccessPoints);
        connect(m_wirelessPage, &WirelessPage::requestWirelessScan, m_networkWorker, &NetworkWorker::requestWirelessScan);
        connect(m_wirelessPage, &WirelessPage::requestConnectAp, m_networkWorker, &NetworkWorker::activateAccessPoint);
        connect(m_wirelessPage, &WirelessPage::requestDeviceEnabled, m_networkWorker, &NetworkWorker::setDeviceEnable);
        connect(m_wirelessPage, &WirelessPage::requestDisconnectConnection, m_networkWorker, &NetworkWorker::deactiveConnection);
        connect(m_wirelessPage, &WirelessPage::requestDeviceRemanage, m_networkWorker, &NetworkWorker::remanageDevice, Qt::QueuedConnection);
        connect(m_wirelessPage, &WirelessPage::requestNextPage, [ = ](ContentWidget * const w) {
            m_frameProxy->pushWidget(this, w, dccV20::FrameProxyInterface::PushType::CoverTop);
            m_wirelessPage->setVisible(true);
        });
        connect(m_wirelessPage, &WirelessPage::requestFrameKeepAutoHide, this, &NetworkModule::onSetFrameAutoHide);

        m_wirelessPage->setModel(m_networkModel);
        m_wirelessPage->jumpByUuid(searchPath);
    } else if (dev->type() == NetworkDevice::Wired) {
        p = new WiredPage(static_cast<WiredDevice *>(dev));
        p->setVisible(false);

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
        wiredPage->jumpPath(searchPath);
    }

    if (!p) {
        return;
    }

    p->layout()->setMargin(0);
    m_frameProxy->pushWidget(this, p);
    p->setVisible(true);
}

void NetworkModule::showVpnPage(const QString &searchPath)
{
    VpnPage *p = new VpnPage;
    p->setVisible(false);
    connect(p, &VpnPage::requestVpnEnabled, m_networkWorker, &NetworkWorker::setVpnEnable);
    connect(p, &VpnPage::requestActivateConnection, m_networkWorker, &NetworkWorker::activateConnection);
    connect(p, &VpnPage::requestNextPage, [ = ](ContentWidget * const w) {
        m_frameProxy->pushWidget(this, w, dccV20::FrameProxyInterface::PushType::CoverTop);
    });
    connect(p, &VpnPage::requestFrameKeepAutoHide, this, &NetworkModule::onSetFrameAutoHide);

    p->setModel(m_networkModel);

    m_frameProxy->pushWidget(this, p);
    p->setVisible(true);
    p->jumpPath(searchPath);
}

void NetworkModule::showPppPage(const QString &searchPath)
{
    PppoePage *p = new PppoePage;
    p->setVisible(false);
    connect(p, &PppoePage::requestActivateConnection, m_networkWorker, &NetworkWorker::activateConnection);
    connect(p, &PppoePage::requestNextPage, [ = ](ContentWidget * const w) {
        m_frameProxy->pushWidget(this, w, dccV20::FrameProxyInterface::PushType::CoverTop);
    });
    connect(p, &PppoePage::requestFrameKeepAutoHide, this, &NetworkModule::onSetFrameAutoHide);
    p->setModel(m_networkModel);

    m_frameProxy->pushWidget(this, p);
    p->setVisible(true);
    p->jumpPath(searchPath);
}

void NetworkModule::showChainsProxyPage()
{
    m_networkWorker->queryChains();

    ChainsProxyPage *chains = new ChainsProxyPage;
    chains->setVisible(false);
    chains->setModel(m_networkModel);

    connect(chains, &ChainsProxyPage::requestSet, m_networkWorker, &NetworkWorker::setChainsProxy);

    m_frameProxy->pushWidget(this, chains);
    chains->setVisible(true);
}

void NetworkModule::showProxyPage()
{
    ProxyPage *p = new ProxyPage;
    p->setVisible(false);
    connect(p, &ProxyPage::requestQueryProxyData, m_networkWorker, &NetworkWorker::queryProxyData);
    connect(p, &ProxyPage::requestSetProxyMethod, m_networkWorker, &NetworkWorker::setProxyMethod);
    connect(p, &ProxyPage::requestSetIgnoreHosts, m_networkWorker, &NetworkWorker::setProxyIgnoreHosts);
    connect(p, &ProxyPage::requestSetProxy, m_networkWorker, &NetworkWorker::setProxy);
    connect(p, &ProxyPage::requestSetAutoProxy, m_networkWorker, &NetworkWorker::setAutoProxy);

    p->setModel(m_networkModel);

    m_frameProxy->pushWidget(this, p);
    p->setVisible(true);
}

void NetworkModule::showDetailPage()
{
    NetworkDetailPage *p = new NetworkDetailPage;
    p->setVisible(false);
    p->updateNetworkInfo();

    m_frameProxy->pushWidget(this, p);
    p->setVisible(true);
}

void NetworkModule::showHotspotPage()
{
    HotspotPage *p = new HotspotPage();
    p->setVisible(false);
    p->setModel(m_networkModel);
    connect(p, &HotspotPage::requestActivateConnection, m_networkWorker, &NetworkWorker::activateConnection);
    connect(p, &HotspotPage::requestDisconnectConnection, m_networkWorker, &NetworkWorker::deactiveConnection);
    connect(p, &HotspotPage::requestDeviceRemanage, m_networkWorker, &NetworkWorker::remanageDevice, Qt::QueuedConnection);
    connect(p, &HotspotPage::requestNextPage, [ = ](ContentWidget * const w) {
        m_frameProxy->pushWidget(this, w, dccV20::FrameProxyInterface::PushType::CoverTop);
    });

    m_frameProxy->pushWidget(this, p);
    p->setVisible(true);
}

void NetworkModule::onSetFrameAutoHide(const bool autoHide)
{
    Q_UNUSED(autoHide);
    //m_frameProxy->setFrameAutoHide(this, autoHide);
}

void NetworkModule::showWiredEditPage(NetworkDevice *dev, const QString &connUuid)
{
    // it will be destroyed by Frame
    m_connEditPage = new ConnectionEditPage(ConnectionEditPage::ConnectionType::WiredConnection, dev->path(), connUuid);
    m_connEditPage->setVisible(false);
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
    m_connEditPage->setVisible(true);
}

void NetworkModule::showWirelessEditPage(dde::network::NetworkDevice *dev, const QString &connUuid, const QString &apPath)
{
    // it will be destroyed by Frame
    m_connEditPage = new ConnectionWirelessEditPage(dev->path(), connUuid);
    m_connEditPage->setVisible(false);
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
    m_connEditPage->setVisible(true);
}

void NetworkModule::removeConnEditPageByDevice(dde::network::NetworkDevice *dev)
{
    if (m_connEditPage && dev->path() == m_connEditPage->devicePath()) {
        m_connEditPage->onDeviceRemoved();
        m_connEditPage = nullptr;
    }
}
