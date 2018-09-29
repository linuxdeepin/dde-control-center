#include "connectionhotspoteditpage.h"
#include "settings/hotspotsettings.h"

#include <networkmanagerqt/settings.h>
#include <networkmanagerqt/device.h>
#include <networkmanagerqt/ipv4setting.h>

#include <QDebug>

using namespace dcc::network;
using namespace dcc::widgets;
using namespace NetworkManager;

ConnectionHotspotEditPage::ConnectionHotspotEditPage(const QString &devPath, const QString &connUuid, QWidget *parent)
    : ConnectionEditPageNew(ConnectionEditPageNew::ConnectionType::WirelessConnection, devPath, connUuid, parent)
{
    m_devPath = devPath;
}

ConnectionHotspotEditPage::~ConnectionHotspotEditPage()
{
}

void ConnectionHotspotEditPage::initSettingsWidget()
{
    // some special configurations for hotspot
    m_connectionSettings->setId("hotspot");
    NetworkManager::Device::Ptr device = findNetworkInterface(m_devPath);
    if (device) {
        m_connectionSettings->setInterfaceName(device->interfaceName());
    }
    NetworkManager::Ipv4Setting::Ptr ipv4Setting =
        m_connectionSettings->setting(NetworkManager::Setting::SettingType::Ipv4)
        .staticCast<NetworkManager::Ipv4Setting>();
    ipv4Setting->setMethod(NetworkManager::Ipv4Setting::ConfigMethod::Shared);
    ipv4Setting->setInitialized(true);

    m_settingsWidget = new HotspotSettings(m_connectionSettings, this);

    connect(m_settingsWidget, &AbstractSettings::requestNextPage, this, &ConnectionEditPageNew::requestNextPage);

    m_settingsLayout->addWidget(m_settingsWidget);
}
