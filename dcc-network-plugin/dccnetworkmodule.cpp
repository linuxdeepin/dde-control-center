/*
 * Copyright (C) 2020 ~ 2021 Uniontech Technology Co., Ltd.
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

#include "dccnetworkmodule.h"
#include "window/gsettingwatcher.h"
#include "chainsproxypage.h"
#include "connectioneditpage.h"
#include "connectionwirelesseditpage.h"
#include "hotspotpage.h"
#include "networkdetailpage.h"
#include "networkmodulewidget.h"
#include "pppoepage.h"
#include "proxypage.h"
#include "vpnpage.h"
#include "wiredpage.h"
#include "wirelesspage.h"
#include "window/gsettingwatcher.h"

#include <QLayout>

#include <networkcontroller.h>
#include <networkdevicebase.h>
#include <wireddevice.h>
#include <wirelessdevice.h>
#include <dslcontroller.h>
#include <hotspotcontroller.h>

#include <NetworkManagerQt/Manager>

using namespace dde::network;
using namespace dccV20;
using namespace dcc;

DCCNetworkModule::DCCNetworkModule()
    : QObject()
    , ModuleInterface()
    , m_indexWidget(nullptr)
    , m_connEditPage(nullptr)
    , m_airplaneMode(new DBusAirplaneMode("com.deepin.daemon.AirplaneMode", "/com/deepin/daemon/AirplaneMode", QDBusConnection::systemBus(), this))
{
    QTranslator *translator = new QTranslator(this);
    translator->load(QString("/usr/share/dcc-network-plugin/translations/dcc-network-plugin_%1.qm").arg(QLocale::system().name()));
    QCoreApplication::installTranslator(translator);

    connect(NetworkManager::notifier(), &Notifier::isStartingUpChanged, this, [] {
        // 在服务重新启动后，需要手动调用一下每个设备的可用连接的函数，否则NetworkManager无法获取到连接
        Device::List devices = networkInterfaces();
        for (Device::Ptr device : devices)
            device->availableConnections();
    });
}

DCCNetworkModule::~DCCNetworkModule()
{
    if (m_indexWidget)
        m_indexWidget->deleteLater();
}

void DCCNetworkModule::preInitialize(bool sync, FrameProxyInterface::PushType)
{
    Q_UNUSED(sync);

    GSettingWatcher::instance()->insertState("networkWireless");
    GSettingWatcher::instance()->insertState("networkWired");
    GSettingWatcher::instance()->insertState("networkDsl");
    GSettingWatcher::instance()->insertState("networkVpn");
    GSettingWatcher::instance()->insertState("systemProxy");
    GSettingWatcher::instance()->insertState("applicationProxy");
    GSettingWatcher::instance()->insertState("networkDetails");
    GSettingWatcher::instance()->insertState("personalHotspot");

    // 初始化时采用同步方式获取所有网络数据，加载后使用异步操作进行通信
    NetworkController::setActiveSync(true);

    addChildPageTrans();
    initSearchData();
}

void DCCNetworkModule::initialize()
{
    NetworkController *networkController = NetworkController::instance();
    connect(networkController, &NetworkController::deviceRemoved, this, &DCCNetworkModule::deviceChanged);
    connect(networkController, &NetworkController::deviceAdded, this, &DCCNetworkModule::deviceChanged);
    deviceChanged();
    // 后续的操作仍然使用异步，防止操作卡顿
    networkController->updateSync(false);
}

void DCCNetworkModule::active()
{
    Q_ASSERT(m_frameProxy);
    ConnectionEditPage::setFrameProxy(m_frameProxy);

    m_indexWidget = new NetworkModuleWidget;

    connect(m_indexWidget, &NetworkModuleWidget::requestShowPppPage, this, &DCCNetworkModule::showPppPage);
    connect(m_indexWidget, &NetworkModuleWidget::requestShowVpnPage, this, &DCCNetworkModule::showVPNPage);
    connect(m_indexWidget, &NetworkModuleWidget::requestShowDeviceDetail, this, &DCCNetworkModule::showDeviceDetailPage);
    connect(m_indexWidget, &NetworkModuleWidget::requestShowChainsPage, this, &DCCNetworkModule::showChainsProxyPage);
    connect(m_indexWidget, &NetworkModuleWidget::requestShowProxyPage, this, &DCCNetworkModule::showProxyPage);
    connect(m_indexWidget, &NetworkModuleWidget::requestHotspotPage, this, &DCCNetworkModule::showHotspotPage);
    connect(m_indexWidget, &NetworkModuleWidget::requestShowInfomation, this, &DCCNetworkModule::showDetailPage);
    connect(m_indexWidget, &NetworkModuleWidget::destroyed, [ this ] {
        m_indexWidget = nullptr;
    });

    m_frameProxy->pushWidget(this, m_indexWidget);
    m_indexWidget->setVisible(true);
    initListConfig();
    m_indexWidget->showDefaultWidget();
}

QStringList DCCNetworkModule::availPage() const
{
    QStringList list;
    list << "DSL" << "DSL/Create PPPoE Connection" << "VPN" << "VPN/Create VPN" << "VPN/Import VPN"
         << "System Proxy" << "Application Proxy" << "Network Details"
         << "Wired Network" << "Wired Network/addWiredConnection"
         << "Wireless Network" << "WirelessPage" << "Personal Hotspot";

    QList<NetworkDeviceBase *> devices = NetworkController::instance()->devices();
    for (NetworkDeviceBase *dev: devices)
        list << dev->path();

    return list;
}

const QString DCCNetworkModule::displayName() const
{
    return tr("Network");
}

QIcon DCCNetworkModule::icon() const
{
    return QIcon::fromTheme("dcc_nav_network");
}

QString DCCNetworkModule::translationPath() const
{
    return QString(":/translations/dcc-network-plugin_%1.ts");
}

QString DCCNetworkModule::path() const
{
    return "mainwindow";
}

QString DCCNetworkModule::follow() const
{
    return "personalization";
}

const QString DCCNetworkModule::name() const
{
    return "network";
}

void DCCNetworkModule::showPage(const QString &pageName)
{
    Q_UNUSED(pageName);
}

QWidget *DCCNetworkModule::moduleWidget()
{
    return m_indexWidget;
}

int DCCNetworkModule::load(const QString &path)
{
    if (!m_indexWidget)
        active();

    QList<NetworkDeviceBase *> devices = NetworkController::instance()->devices();
    QStringList devPaths = path.split(",");
    if (devPaths.size() > 1) {
        for (NetworkDeviceBase *dev: devices) {
            if (dev->path() == devPaths.at(0)) {
                showDeviceDetailPage(dev, devPaths.at(1));
                m_indexWidget->setIndexFromPath(devPaths.at(0));
                return 0;
            }
        }
    }

    for (NetworkDeviceBase *dev: devices) {
        if (dev->path() == path) {
            showDeviceDetailPage(dev);
            m_indexWidget->setIndexFromPath(path);
            return 0;
        }
    }

    QStringList pathList = path.split("/");
    int index = m_indexWidget->gotoSetting(pathList.at(0));

    QString searchPath = "";
    if (pathList.count() > 1)
        searchPath = pathList[1];

    m_indexWidget->initSetting(index == -1 ? 0 : index, searchPath);

    return index == -1 ? -1 : 0;
}

void DCCNetworkModule::initListConfig()
{
    auto func_is_visible = [](const QString &key)->bool {
        if (key.isEmpty())
            return false;

        return GSettingWatcher::instance()->get(key).toBool();
    };

    auto setModulVisible = [ func_is_visible, this ](const QString &key, bool visible = true) {
        bool isVisible = func_is_visible(key) && visible;
        m_indexWidget->setModelVisible(key, isVisible);
    };

    setModulVisible("networkWired");
    setModulVisible("networkWireless");
    setModulVisible("personalHotspot");
    setModulVisible("applicationProxy");
    setModulVisible("networkDetails");
    setModulVisible("networkDsl", hasModule(PageType::DSLPage));
    setModulVisible("systemProxy");
    setModulVisible("networkVpn");
}

bool DCCNetworkModule::hasModule(const PageType &type)
{
    if (type == PageType::NonePage)
        return false;

    auto deviceExist = [ = ](const DeviceType &deviceType) {
        QList<NetworkDeviceBase *> devices = NetworkController::instance()->devices();
        for (NetworkDeviceBase *device : devices) {
            if (device->deviceType() == deviceType)
                return true;
        }

        return false;
    };

    switch (type) {
    case PageType::WiredPage:
    case PageType::DSLPage:
        return deviceExist(DeviceType::Wired);
    case PageType::WirelessPage:
        return deviceExist(DeviceType::Wireless);
    case PageType::HotspotPage:
        return NetworkController::instance()->hotspotController()->supportHotspot();
    default: break;
    }

    return true;
}

void DCCNetworkModule::initSearchData()
{
    if (!m_frameProxy)
        return;

    const QString& module = m_frameProxy->moduleDisplayName(name());
    const QString& applicationProxy = tr("Application Proxy");
    const QString& personalHost = tr("Personal Hotspot");
    const QString& networkDetail = tr("Network Details");
    const QString& systemProxy = tr("System Proxy");
    const QString& wiredNetwork = tr("Wired Network");
    const QString& wirelessNetwork = tr("Wireless Network");
    const QString& vpn = tr("VPN");
    const bool wiredVisible = hasModule(PageType::WiredPage);
    const bool wirelessVisible = hasModule(PageType::WirelessPage);
    const bool hotspotsVisible = hasModule(PageType::HotspotPage);
    static QMap<QString, bool> gsettingsMap;

    auto func_is_visible = [ = ](const QString &gsettings)->bool {
        if (gsettings.isEmpty())
            return false;

        bool ret = GSettingWatcher::instance()->get(gsettings).toBool();
        gsettingsMap.insert(gsettings, ret);

        return ret;
    };

    const QStringList& gslist {
        "networkWired"
        , "networkWireless"
        , "personalHotspot"
        , "applicationProxy"
        , "networkDetails"
        , "networkDsl"
        , "systemProxy"
        , "networkVpn"
    };
    auto func_wired_visible = [ = ](bool visible) {
        bool isVisible = func_is_visible("networkWired");
        bool bWireNetwork = isVisible && visible;
        if (m_indexWidget)
            m_indexWidget->setModelVisible("networkWired", isVisible);
        m_frameProxy->setWidgetVisible(module, wiredNetwork, bWireNetwork);
        //~ contents_path /network/Wired Network
        //~ child_page Wired Network
        m_frameProxy->setDetailVisible(module, wiredNetwork, tr("Wired Network Adapter"), bWireNetwork);
        //~ contents_path /network/Wired Network/addWiredConnection
        //~ child_page Wired Network
        m_frameProxy->setDetailVisible(module, wiredNetwork, tr("Add Network Connection"), bWireNetwork);
    };

    auto func_wireless_visible = [ = ](bool visible) {
        bool configVisible = func_is_visible("networkWireless");
        bool bWirelessNetwork = configVisible && visible;
        if (m_indexWidget)
            m_indexWidget->setModelVisible("networkWireless", configVisible);
        m_frameProxy->setWidgetVisible(module, wirelessNetwork, bWirelessNetwork);
        //~ contents_path /network/Wireless Network
        //~ child_page Wireless Network
        m_frameProxy->setDetailVisible(module, wirelessNetwork, tr("Wireless Network Adapter"), bWirelessNetwork);
        //~ contents_path /network/Wireless Network/Connect to hidden network
        //~ child_page Wireless Network
        m_frameProxy->setDetailVisible(module, wirelessNetwork, tr("Connect to hidden network"), bWirelessNetwork);
    };

    auto func_dsl_visible = [ = ](bool visible) {
        bool dslVisible = func_is_visible("networkDsl") && visible;
        if (m_indexWidget)
            m_indexWidget->setModelVisible("networkDsl", dslVisible);
        //~ contents_path /network/DSL
        //~ child_page_hide DSL
        const QString dsl = tr("DSL");
        m_frameProxy->setWidgetVisible(module, dsl, dslVisible);
        //~ contents_path /network/DSL/Create PPPoE Connection
        //~ child_page DSL
        m_frameProxy->setDetailVisible(module, dsl, tr("Create PPPoE Connection"), dslVisible);
    };

    auto func_vpn_visible = [ = ] {
        bool bVPN = func_is_visible("networkVpn");
        if (m_indexWidget)
            m_indexWidget->setModelVisible("networkVpn", bVPN);
        m_frameProxy->setWidgetVisible(module, vpn, bVPN);
        //~ contents_path /network/VPN
        //~ child_page VPN
        m_frameProxy->setDetailVisible(module, vpn, tr("VPN Status"), bVPN);
        //~ contents_path /network/VPN/Create VPN
        //~ child_page VPN
        m_frameProxy->setDetailVisible(module, vpn, tr("Create VPN"), bVPN);
        //~ contents_path /network/VPN/Import VPN
        //~ child_page VPN
        m_frameProxy->setDetailVisible(module, vpn, tr("Import VPN"), bVPN);
    };

    auto func_sysproxy_visible = [ = ] {
        bool bSystemProxy = func_is_visible("systemProxy");
        if (m_indexWidget)
            m_indexWidget->setModelVisible("systemProxy", bSystemProxy);
        m_frameProxy->setWidgetVisible(module, systemProxy, bSystemProxy);
        // 系统代理的搜索放到proxyType类中了，因为此处的Proxy Type与下面的应用代理的Proxy Type有冲突，导致搜索列表无法识别
        m_frameProxy->setDetailVisible(module, systemProxy, tr("Proxy Type"), bSystemProxy);
        m_frameProxy->setDetailVisible(module, systemProxy, tr("Configuration URL"), bSystemProxy);
        m_frameProxy->setDetailVisible(module, systemProxy, systemProxy, bSystemProxy);
    };

    auto func_appproxy_visible = [ = ] {
        bool bAppProxy = func_is_visible("applicationProxy");
        if (m_indexWidget)
            m_indexWidget->setModelVisible("applicationProxy", bAppProxy);
        m_frameProxy->setWidgetVisible(module, applicationProxy, bAppProxy);
        //~ contents_path /network/Application Proxy
        //~ child_page Application Proxy
        m_frameProxy->setDetailVisible(module, applicationProxy, tr("Proxy Type"), bAppProxy);
        //~ contents_path /network/Application Proxy
        //~ child_page Application Proxy
        m_frameProxy->setDetailVisible(module, applicationProxy, tr("IP Address"), bAppProxy);
        //~ contents_path /network/Application Proxy
        //~ child_page Application Proxy
        m_frameProxy->setDetailVisible(module, applicationProxy, tr("Port"), bAppProxy);
        //~ contents_path /network/Application Proxy
        //~ child_page Application Proxy
        m_frameProxy->setDetailVisible(module, applicationProxy, tr("Username"), bAppProxy);
        //~ contents_path /network/Application Proxy
        //~ child_page Application Proxy
        m_frameProxy->setDetailVisible(module, applicationProxy, tr("Password"), bAppProxy);
    };

    auto func_perhotspot_visible = [ = ](bool visible) {
        bool configVisible = func_is_visible("personalHotspot");
        bool bPersonalHost = configVisible && visible;
        if (m_indexWidget)
            m_indexWidget->setModelVisible("personalHotspot", bPersonalHost);
        m_frameProxy->setWidgetVisible(module, personalHost, bPersonalHost);
        //~ contents_path /network/Personal Hotspot
        //~ child_page Personal Hotspot
        m_frameProxy->setDetailVisible(module, personalHost, tr("Hotspot"), bPersonalHost);
        //~ contents_path /network/Personal Hotspot/Create Hotspot
        //~ child_page Personal Hotspot
        m_frameProxy->setDetailVisible(module, personalHost, tr("Create Hotspot"), bPersonalHost);
    };

    auto func_netdetails_visible = [ = ] {
        bool bNetworkDetail = func_is_visible("networkDetails");
        if (m_indexWidget)
            m_indexWidget->setModelVisible("networkDetails", bNetworkDetail);
        m_frameProxy->setWidgetVisible(module, networkDetail, bNetworkDetail);
        //~ contents_path /network/Network Details
        //~ child_page Network Details
        m_frameProxy->setDetailVisible(module, networkDetail, tr("Interface"), bNetworkDetail);
        //~ contents_path /network/Network Details
        //~ child_page Network Details
        m_frameProxy->setDetailVisible(module, networkDetail, tr("MAC"), bNetworkDetail);
        //~ contents_path /network/Network Details
        //~ child_page Network Details
        m_frameProxy->setDetailVisible(module, networkDetail, tr("Band"), bNetworkDetail);
        //~ contents_path /network/Network Details
        //~ child_page Network Details
        m_frameProxy->setDetailVisible(module, networkDetail, tr("Port"), bNetworkDetail);
        //~ contents_path /network/Network Details
        //~ child_page Network Details
        m_frameProxy->setDetailVisible(module, networkDetail, tr("IPv4"), bNetworkDetail);
        //~ contents_path /network/Network Details
        //~ child_page Network Details
        m_frameProxy->setDetailVisible(module, networkDetail, tr("Gateway"), bNetworkDetail);
        //~ contents_path /network/Network Details
        //~ child_page Network Details
        m_frameProxy->setDetailVisible(module, networkDetail, tr("Primary DNS"), bNetworkDetail);
        //~ contents_path /network/Network Details
        //~ child_page Network Details
        m_frameProxy->setDetailVisible(module, networkDetail, tr("Netmask"), true);
        //~ contents_path /network/Network Details
        //~ child_page Network Details
        m_frameProxy->setDetailVisible(module, networkDetail, tr("IPv6"), true);
        //~ contents_path /network/Network Details
        //~ child_page Network Details
        m_frameProxy->setDetailVisible(module, networkDetail, tr("Prefix"), true);
        //~ contents_path /network/Network Details
        //~ child_page Network Details
        m_frameProxy->setDetailVisible(module, networkDetail, tr("Speed"), true);
    };

    auto func_process_all = [ = ] {
        func_appproxy_visible();
        func_netdetails_visible();
        func_dsl_visible(wiredVisible);
        func_sysproxy_visible();
        func_vpn_visible();
        func_wired_visible(wiredVisible);
        func_wireless_visible(wirelessVisible);
        func_perhotspot_visible(hotspotsVisible);
     };

    connect(this, &DCCNetworkModule::deviceChanged, this, [ = ] {
        func_wired_visible(hasModule(PageType::WiredPage));
        func_wireless_visible(hasModule(PageType::WirelessPage));
        func_perhotspot_visible(hasModule(PageType::HotspotPage));
        m_frameProxy->updateSearchData(module);
    });

    connect(GSettingWatcher::instance(), &GSettingWatcher::notifyGSettingsChanged, this, [ = ](const QString &gsetting, const QString &state) {
        if (gsetting.isEmpty() || !gsettingsMap.contains(gsetting) || !gslist.contains(gsetting))
            return;

        if (gsettingsMap.value(gsetting) == GSettingWatcher::instance()->get(gsetting).toBool())
            return;

        if ("applicationProxy" == gsetting) {
            func_appproxy_visible();
        } else if ("networkDetails" == gsetting) {
            func_netdetails_visible();
        } else if ("networkDsl" == gsetting) {
            func_dsl_visible(wiredVisible);
        } else if ("systemProxy" == gsetting) {
            func_sysproxy_visible();
        } else if ("networkVpn" == gsetting) {
            func_vpn_visible();
        } else if ("networkWired" == gsetting || "networkWireless" == gsetting || "personalHotspot" == gsetting) {
            func_wired_visible(wiredVisible);
            func_wireless_visible(wirelessVisible);
            func_perhotspot_visible(hotspotsVisible);
        } else {
            qWarning() << " not contains the gsettings : " << gsetting << state;
            return;
        }

        qInfo() << " [notifyGSettingsChanged]  gsetting, state :" << gsetting << state;
        m_frameProxy->updateSearchData(module);
    });

    func_process_all();
}

void DCCNetworkModule::addChildPageTrans() const
{
    if (m_frameProxy) {
        m_frameProxy->addChildPageTrans("Wired Network", tr("Wired Network"));
        m_frameProxy->addChildPageTrans("Wireless Network", tr("Wireless Network"));
        m_frameProxy->addChildPageTrans("DSL", tr("DSL"));
        m_frameProxy->addChildPageTrans("VPN", tr("VPN"));
        m_frameProxy->addChildPageTrans("System Proxy", tr("System Proxy"));
        m_frameProxy->addChildPageTrans("Application Proxy", tr("Application Proxy"));
        m_frameProxy->addChildPageTrans("Personal Hotspot", tr("Personal Hotspot"));
        m_frameProxy->addChildPageTrans("Network Details", tr("Network Details"));
    }
}

void DCCNetworkModule::removeConnEditPageByDevice(NetworkDeviceBase *dev)
{
    if (m_connEditPage && dev->path() == m_connEditPage->devicePath()) {
        m_connEditPage->onDeviceRemoved();
        m_connEditPage = nullptr;
    }
}

void DCCNetworkModule::showWirelessEditPage(NetworkDeviceBase *dev, const QString &connUuid, const QString &apPath)
{
    // it will be destroyed by Frame
    m_connEditPage = new ConnectionWirelessEditPage(dev->path(), connUuid, apPath);
    m_connEditPage->setVisible(false);
    connect(m_connEditPage, &ConnectionEditPage::requestNextPage, [ this ] (ContentWidget * const w) {
        m_frameProxy->pushWidget(this, w);
    });

    connect(m_connEditPage, &ConnectionEditPage::back, this, [ this ] {
        m_connEditPage = nullptr;
    });

    NetworkController *networkController = NetworkController::instance();
    connect(networkController, &NetworkController::deviceRemoved, this, [ this, dev ] (QList<NetworkDeviceBase *> devices) {
        Q_UNUSED(devices);
        removeConnEditPageByDevice(dev);
    });

    if (connUuid.isEmpty()) {
        if (apPath.isEmpty()) {
            m_connEditPage->deleteLater();
            return;
        }

        ConnectionWirelessEditPage *wirelessEditPage = static_cast<ConnectionWirelessEditPage *>(m_connEditPage);
        wirelessEditPage->initSettingsWidgetFromAp();
    } else {
        m_connEditPage->initSettingsWidget();
    }

    m_frameProxy->pushWidget(this, m_connEditPage);
    m_connEditPage->setVisible(true);
}

void DCCNetworkModule::showPppPage(const QString &searchPath)
{
    PppoePage *pppoe = new PppoePage;
    pppoe->setVisible(false);
    connect(pppoe, &PppoePage::requestNextPage, [ = ](ContentWidget * const w) {
        m_frameProxy->pushWidget(this, w, FrameProxyInterface::PushType::CoverTop);
    });
    connect(pppoe, &PppoePage::requestFrameKeepAutoHide, [ = ](const bool autoHide) {
        Q_UNUSED(autoHide);
    });

    m_frameProxy->pushWidget(this, pppoe);
    pppoe->setVisible(true);
    pppoe->jumpPath(searchPath);
}

void DCCNetworkModule::showVPNPage(const QString &searchPath)
{
    VpnPage *vpn = new VpnPage;
    vpn->setVisible(false);
    connect(vpn, &VpnPage::requestNextPage, [ = ](ContentWidget * const w) {
        m_frameProxy->pushWidget(this, w, dccV20::FrameProxyInterface::PushType::CoverTop);
    });
    connect(vpn, &VpnPage::requestFrameKeepAutoHide, [ = ](const bool &hide) {
        Q_UNUSED(hide);
    });

    m_frameProxy->pushWidget(this, vpn);
    vpn->setVisible(true);
    vpn->jumpPath(searchPath);
}

void DCCNetworkModule::showDeviceDetailPage(NetworkDeviceBase *dev, const QString &searchPath)
{
    ContentWidget *devicePage = nullptr;

    if (dev->deviceType() == DeviceType::Wireless) {
        WirelessPage *wirelessPage = new WirelessPage(static_cast<WirelessDevice *>(dev));
        wirelessPage->setVisible(false);
        devicePage = wirelessPage;
        connect(wirelessPage, &WirelessPage::requestNextPage, this, [ = ](ContentWidget * const w) {
            m_frameProxy->pushWidget(this, w, dccV20::FrameProxyInterface::PushType::CoverTop);
            wirelessPage->setVisible(true);
        });
        connect(wirelessPage, &WirelessPage::closeHotspot, this, [ = ](WirelessDevice *device) {
            m_indexWidget->setLastDevicePath(device->path());
        });
        connect(m_airplaneMode, &DBusAirplaneMode::EnabledChanged, wirelessPage, &WirelessPage::onAirplaneModeChanged);
        wirelessPage->onAirplaneModeChanged(m_airplaneMode->enabled());

        wirelessPage->jumpByUuid(searchPath);
    } else if (dev->deviceType() == DeviceType::Wired) {
        devicePage = new WiredPage(static_cast<WiredDevice *>(dev));
        devicePage->setVisible(false);

        WiredPage *wiredPage = static_cast<WiredPage *>(devicePage);
        connect(wiredPage, &WiredPage::requestNextPage, [ = ](ContentWidget * const w) {
            m_frameProxy->pushWidget(this, w, dccV20::FrameProxyInterface::PushType::CoverTop);
        });

        wiredPage->jumpPath(searchPath);
    } else
        return;

    devicePage->layout()->setMargin(0);
    m_frameProxy->pushWidget(this, devicePage);
    devicePage->setVisible(true);
}

void DCCNetworkModule::showChainsProxyPage()
{
    ChainsProxyPage *chains = new ChainsProxyPage;
    chains->setVisible(false);

    m_frameProxy->pushWidget(this, chains);
    chains->setVisible(true);
}

void DCCNetworkModule::showProxyPage()
{
    ProxyPage *proxy = new ProxyPage;
    proxy->setVisible(false);

    m_frameProxy->pushWidget(this, proxy);
    proxy->setVisible(true);
}

void DCCNetworkModule::showHotspotPage(const QString &searchPath)
{
    HotspotPage *hotspot = new HotspotPage();
    hotspot->onAirplaneModeChanged(m_airplaneMode->enabled());
    connect(hotspot, &HotspotPage::requestNextPage, this, [ = ](ContentWidget * const w) {
        m_frameProxy->pushWidget(this, w, dccV20::FrameProxyInterface::PushType::CoverTop);
    });
    connect(m_airplaneMode, &DBusAirplaneMode::EnabledChanged, hotspot, &HotspotPage::onAirplaneModeChanged);
    m_frameProxy->pushWidget(this, hotspot);
    hotspot->jumpPath(searchPath);
}

void DCCNetworkModule::showDetailPage()
{
    NetworkDetailPage *detailPage = new NetworkDetailPage;
    detailPage->setVisible(false);

    m_frameProxy->pushWidget(this, detailPage);
    detailPage->setVisible(true);
}
