#include "hotspotsettings.h"
#include "../sections/generichotspotsection.h"
#include "../sections/secrethotspotsection.h"
#include "../sections/wirelesssection.h"

using namespace dcc::widgets;
using namespace NetworkManager;

HotspotSettings::HotspotSettings(NetworkManager::ConnectionSettings::Ptr connSettings, QWidget *parent)
    : AbstractSettings(connSettings, parent)
{
    initSections();
}

HotspotSettings::~HotspotSettings()
{
}

void HotspotSettings::initSections()
{
    NetworkManager::WirelessSetting::Ptr wirelessSetting =
        m_connSettings->setting(Setting::SettingType::Wireless).staticCast<NetworkManager::WirelessSetting>();
    wirelessSetting->setMode(NetworkManager::WirelessSetting::NetworkMode::Ap);

    GenericHotspotSection *genericSection = new GenericHotspotSection(m_connSettings);

    SecretHotspotSection *secretHotspotSection = new SecretHotspotSection(
        m_connSettings->setting(Setting::SettingType::WirelessSecurity)
        .staticCast<NetworkManager::WirelessSecuritySetting>());
    
    WirelessSection *wirelessSection = new WirelessSection(wirelessSetting);

    connect(secretHotspotSection, &SecretHotspotSection::requestPage, this, &HotspotSettings::requestNextPage);
    connect(wirelessSection, &WirelessSection::requestPage, this, &HotspotSettings::requestNextPage);

    m_sectionsLayout->addWidget(genericSection);
    m_sectionsLayout->addWidget(secretHotspotSection);
    m_sectionsLayout->addWidget(wirelessSection);

    m_settingSections.append(genericSection);
    m_settingSections.append(secretHotspotSection);
    m_settingSections.append(wirelessSection);
}
