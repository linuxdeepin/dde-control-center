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

#include <QLayout>

#include <networkcontroller.h>
#include <networkdevicebase.h>
#include <wireddevice.h>
#include <wirelessdevice.h>
#include <dslcontroller.h>

using namespace dde::network;
using namespace dccV20;
using namespace dcc;

DCCNetworkModule::DCCNetworkModule()
    : QObject()
    , ModuleInterface()
    , m_hasAp(false)
    , m_hasWired(false)
    , m_hasWireless(false)
    , m_indexWidget(nullptr)
    , m_airplaneMode(new DBusAirplaneMode("com.deepin.daemon.AirplaneMode", "/com/deepin/daemon/AirplaneMode", QDBusConnection::systemBus(), this))
{
    QTranslator *translator = new QTranslator(this);
    translator->load(QString("/usr/share/dcc-network-plugin/translations/dcc-network-plugin_%1.qm").arg(QLocale::system().name()));
    QCoreApplication::installTranslator(translator);

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
}

DCCNetworkModule::~DCCNetworkModule()
{
    if (m_indexWidget)
        m_indexWidget->deleteLater();
}

void DCCNetworkModule::initialize()
{
    NetworkController *networkController = NetworkController::instance();
    connect(networkController, &NetworkController::deviceRemoved, this, &DCCNetworkModule::onDeviceChanged);
    connect(networkController, &NetworkController::deviceAdded, this, &DCCNetworkModule::onDeviceChanged);
    onDeviceChanged();
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

void DCCNetworkModule::removeConnEditPageByDevice(NetworkDeviceBase *dev)
{
    if (m_connEditPage && dev->path() == m_connEditPage->devicePath()) {
        m_connEditPage->onDeviceRemoved();
        m_connEditPage = nullptr;
    }
}

void DCCNetworkModule::onDeviceChanged()
{
    m_hasAp = false;
    m_hasWired = false;
    m_hasWireless = false;

    QList<NetworkDeviceBase *> devices = NetworkController::instance()->devices();
    for (NetworkDeviceBase *dev : devices) {
        if (dev->deviceType() == DeviceType::Wired)
            m_hasWired = true;

        if (dev->deviceType() != DeviceType::Wireless)
            continue;

        m_hasWireless = true;
        if (static_cast<WirelessDevice*>(dev)->supportHotspot()) {
            m_hasAp = true;
            break;
        }
    }

    m_frameProxy->setRemoveableDeviceStatus(tr("Wired Network"), m_hasWired);
    m_frameProxy->setRemoveableDeviceStatus(tr("Wireless Network"), m_hasWireless);
    m_frameProxy->setRemoveableDeviceStatus(tr("Personal Hotspot"), m_hasAp);
}

void DCCNetworkModule::showWirelessEditPage(NetworkDeviceBase *dev, const QString &connUuid, const QString &apPath)
{
    // it will be destroyed by Frame
    m_connEditPage = new ConnectionWirelessEditPage(dev->path(), connUuid, apPath);
    m_connEditPage->setVisible(false);
    connect(m_connEditPage, &ConnectionEditPage::requestNextPage, [ = ](ContentWidget * const w) {
        m_frameProxy->pushWidget(this, w);
    });

    connect(m_connEditPage, &ConnectionEditPage::back, this, [ = ]() {
        m_connEditPage = nullptr;
    });

    NetworkController *networkController = NetworkController::instance();
    connect(networkController, &NetworkController::deviceRemoved, this, [ = ](QList<NetworkDeviceBase *> devices) {
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
    connect(pppoe, &PppoePage::requestFrameKeepAutoHide, [ = ] (const bool autoHide) {
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
    connect(vpn, &VpnPage::requestFrameKeepAutoHide, [ = ] (const bool &hide) {
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
        connect(wirelessPage, &WirelessPage::closeHotspot, this, [ = ] (WirelessDevice *device) {
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

void DCCNetworkModule::showHotspotPage()
{
    HotspotPage *hotspot = new HotspotPage();
    hotspot->onAirplaneModeChanged(m_airplaneMode->enabled());
    connect(hotspot, &HotspotPage::requestNextPage, this, [ = ] (ContentWidget * const w) {
        m_frameProxy->pushWidget(this, w, dccV20::FrameProxyInterface::PushType::CoverTop);
    });
    connect(m_airplaneMode, &DBusAirplaneMode::EnabledChanged, hotspot, &HotspotPage::onAirplaneModeChanged);

    m_frameProxy->pushWidget(this, hotspot);
}

void DCCNetworkModule::showDetailPage()
{
    NetworkDetailPage *detailPage = new NetworkDetailPage;
    detailPage->setVisible(false);

    m_frameProxy->pushWidget(this, detailPage);
    detailPage->setVisible(true);
}
