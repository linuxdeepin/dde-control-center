#include "dccnetworkmodule.h"
#include "window/gsettingwatcher.h"
#include "networkmodulewidget.h"

#include <networkcontroller.h>
#include <networkdevicebase.h>
#include <wireddevice.h>
#include <wirelessdevice.h>

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
    GSettingWatcher::instance()->insertState("networkWireless");
    GSettingWatcher::instance()->insertState("networkWired");
    GSettingWatcher::instance()->insertState("networkDsl");
    GSettingWatcher::instance()->insertState("networkVpn");
    GSettingWatcher::instance()->insertState("systemProxy");
    GSettingWatcher::instance()->insertState("applicationProxy");
    GSettingWatcher::instance()->insertState("networkDetails");
    GSettingWatcher::instance()->insertState("personalHotspot");
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
    return QIcon();
}

QString DCCNetworkModule::translationPath() const
{
    return QString("");
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

void DCCNetworkModule::onDeviceChanged()
{
    m_hasAp = false;
    m_hasWired = false;
    m_hasWireless = false;

    QList<NetworkDeviceBase *> devices = NetworkController::instance()->devices();
    for (NetworkDeviceBase *dev : devices) {
        if (dev->deviceType() == DeviceType::Wired) {
            m_hasWired = true;
        }
        if (dev->deviceType() != DeviceType::Wireless)
            continue;
        m_hasWireless = true;
        if (static_cast<WirelessDevice*>(dev)->supportHotspot()) {
            m_hasAp = true;
        }
    }

    m_frameProxy->setRemoveableDeviceStatus(tr("Wired Network"), m_hasWired);
    m_frameProxy->setRemoveableDeviceStatus(tr("Wireless Network"), m_hasWireless);
    m_frameProxy->setRemoveableDeviceStatus(tr("Personal Hotspot"), m_hasAp);
}

void DCCNetworkModule::showWirelessEditPage(NetworkDeviceBase *dev, const QString &connUuid, const QString &apPath)
{
}
