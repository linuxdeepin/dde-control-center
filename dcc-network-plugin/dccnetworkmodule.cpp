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
    , m_indexWidget(new NetworkModuleWidget(nullptr))
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

    connect(m_indexWidget, &NetworkModuleWidget::requestShowPppPage, this, &DCCNetworkModule::showPppPage);
    connect(m_indexWidget, &NetworkModuleWidget::requestShowVpnPage, this, &DCCNetworkModule::showVPNPage);
    connect(m_indexWidget, &NetworkModuleWidget::requestShowDeviceDetail, this, &DCCNetworkModule::showDeviceDetailPage);
    connect(m_indexWidget, &NetworkModuleWidget::requestShowChainsPage, this, &DCCNetworkModule::showChainsProxyPage);
    connect(m_indexWidget, &NetworkModuleWidget::requestShowProxyPage, this, &DCCNetworkModule::showProxyPage);
    connect(m_indexWidget, &NetworkModuleWidget::requestHotspotPage, this, &DCCNetworkModule::showHotspotPage);
    connect(m_indexWidget, &NetworkModuleWidget::requestShowInfomation, this, &DCCNetworkModule::showDetailPage);
}

DCCNetworkModule::~DCCNetworkModule()
{
    delete m_indexWidget;
}

void DCCNetworkModule::initialize()
{
    NetworkController *networkController = NetworkController::instance();
    connect(networkController, &NetworkController::deviceRemoved, this, &DCCNetworkModule::onDeviceChanged);
    connect(networkController, &NetworkController::deviceAdded, this, &DCCNetworkModule::onDeviceChanged);
    onDeviceChanged();
}

void DCCNetworkModule::active()
{
    Q_ASSERT(m_frameProxy);

    m_frameProxy->pushWidget(this, m_indexWidget, dccV20::FrameProxyInterface::PushType::Normal);
}

const QString DCCNetworkModule::displayName() const
{
    return tr("network");
}

QIcon DCCNetworkModule::icon() const
{
    return QIcon::fromTheme("network/dcc_nav_network");
}

QString DCCNetworkModule::translationPath() const
{
    return QString("/usr/share/dcc-network-plugin/translations");
}

QString DCCNetworkModule::path() const
{
    return "network";
}

QString DCCNetworkModule::follow() const
{
    return "5";
}

const QString DCCNetworkModule::name() const
{
    return tr("network");
}

void DCCNetworkModule::showPage(const QString &pageName)
{
    Q_UNUSED(pageName);
}

QWidget *DCCNetworkModule::moduleWidget()
{
    return m_indexWidget;
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
    m_connEditPage = new ConnectionWirelessEditPage(dev->path(), connUuid);
    m_connEditPage->setVisible(false);
    connect(m_connEditPage, &ConnectionEditPage::requestNextPage, [ = ](ContentWidget * const w) {
        m_frameProxy->pushWidget(this, w);
    });

    connect(m_connEditPage, &ConnectionEditPage::requestFrameAutoHide, [ = ] {});
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
        wirelessEditPage->initSettingsWidgetFromAp(apPath);
    } else {
        m_connEditPage->initSettingsWidget();
    }

    m_frameProxy->pushWidget(this, m_connEditPage);
    m_connEditPage->setVisible(true);
}

void DCCNetworkModule::showPppPage(const QString &searchPath)
{
    PppoePage *p = new PppoePage;
    p->setVisible(false);
    connect(p, &PppoePage::requestNextPage, [ = ](ContentWidget * const w) {
        m_frameProxy->pushWidget(this, w, FrameProxyInterface::PushType::CoverTop);
    });
    connect(p, &PppoePage::requestFrameKeepAutoHide, [ = ] (const bool autoHide) {
        Q_UNUSED(autoHide);
    });

    m_frameProxy->pushWidget(this, p);
    p->setVisible(true);
    p->jumpPath(searchPath);
}

void DCCNetworkModule::showVPNPage(const QString &searchPath)
{
    VpnPage *p = new VpnPage;
    p->setVisible(false);
    connect(p, &VpnPage::requestNextPage, [ = ](ContentWidget * const w) {
        m_frameProxy->pushWidget(this, w, dccV20::FrameProxyInterface::PushType::CoverTop);
    });
    connect(p, &VpnPage::requestFrameKeepAutoHide, [ = ] (const bool &hide) {
        Q_UNUSED(hide);
    });

    m_frameProxy->pushWidget(this, p);
    p->setVisible(true);
    p->jumpPath(searchPath);
}

void DCCNetworkModule::showDeviceDetailPage(NetworkDeviceBase *dev, const QString &searchPath)
{
    ContentWidget *devicePage = nullptr;

    if (dev->deviceType() == DeviceType::Wireless) {
        WirelessPage *wirelessPage = new WirelessPage(static_cast<WirelessDevice *>(dev));
        wirelessPage->setVisible(false);
        devicePage = wirelessPage;
        connect(wirelessPage, &WirelessPage::back, [ = ] () {
            m_frameProxy->popWidget(this);
        });
        connect(wirelessPage, &WirelessPage::requestNextPage, [ = ](ContentWidget * const w) {
            m_frameProxy->pushWidget(this, w, dccV20::FrameProxyInterface::PushType::CoverTop);
            wirelessPage->setVisible(true);
        });
        connect(wirelessPage, &WirelessPage::requestFrameKeepAutoHide, [ = ] {});

        wirelessPage->jumpByUuid(searchPath);
    } else if (dev->deviceType() == DeviceType::Wired) {
        devicePage = new WiredPage(static_cast<WiredDevice *>(dev));
        devicePage->setVisible(false);

        WiredPage *wiredPage = static_cast<WiredPage *>(devicePage);
        connect(wiredPage, &WiredPage::back, [ = ] {
            m_frameProxy->popWidget(this);
        });
        connect(wiredPage, &WiredPage::requestNextPage, [ = ](ContentWidget * const w) {
            m_frameProxy->pushWidget(this, w, dccV20::FrameProxyInterface::PushType::CoverTop);
        });
        connect(wiredPage, &WiredPage::requestFrameKeepAutoHide, [ = ] {});

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

    connect(chains, &ChainsProxyPage::back, [ = ] {
        m_frameProxy->popWidget(this);
    });
}

void DCCNetworkModule::showProxyPage()
{
    ProxyPage *p = new ProxyPage;
    p->setVisible(false);

    m_frameProxy->pushWidget(this, p);
    p->setVisible(true);
}

void DCCNetworkModule::showHotspotPage()
{
    HotspotPage *p = new HotspotPage();
    connect(p, &HotspotPage::requestNextPage, [ = ](ContentWidget * const w) {
        m_frameProxy->pushWidget(this, w, dccV20::FrameProxyInterface::PushType::CoverTop);

        connect(w, &ContentWidget::back, [ = ]() {
            m_frameProxy->popWidget(this);
        });
    });

    m_frameProxy->pushWidget(this, p);
}

void DCCNetworkModule::showDetailPage()
{
    NetworkDetailPage *p = new NetworkDetailPage;
    p->setVisible(false);

    m_frameProxy->pushWidget(this, p);
    p->setVisible(true);
}
