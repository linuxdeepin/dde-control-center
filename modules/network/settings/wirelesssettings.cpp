#include "wirelesssettings.h"
#include "../sections/genericsection.h"
#include "../sections/secretwirelesssection.h"
#include "../sections/ipvxsection.h"
#include "../sections/wirelesssection.h"

using namespace dcc::widgets;
using namespace NetworkManager;

WirelessSettings::WirelessSettings(NetworkManager::ConnectionSettings::Ptr connSettings, QWidget *parent)
    : AbstractSettings(connSettings, parent)
{
    initSections();
}

WirelessSettings::~WirelessSettings()
{
}

void WirelessSettings::initSections()
{
    GenericSection *genericSection = new GenericSection(m_connSettings);

    SecretWirelessSection *secretSection = new SecretWirelessSection(
            m_connSettings->setting(Setting::SettingType::WirelessSecurity).staticCast<NetworkManager::WirelessSecuritySetting>(),
            m_connSettings->setting(Setting::SettingType::Security8021x).staticCast<NetworkManager::Security8021xSetting>());

    IpvxSection *ipv4Section = new IpvxSection(
            m_connSettings->setting(Setting::SettingType::Ipv4).staticCast<NetworkManager::Ipv4Setting>());

    IpvxSection *ipv6Section = new IpvxSection(
            m_connSettings->setting(Setting::SettingType::Ipv6).staticCast<NetworkManager::Ipv6Setting>());
    
    WirelessSection *wirelessSection = new WirelessSection(
            m_connSettings->setting(Setting::SettingType::Wireless).staticCast<NetworkManager::WirelessSetting>());

    connect(wirelessSection, &WirelessSection::requestPage, this, &WirelessSettings::requestNextPage);
    connect(secretSection, &Secret8021xSection::requestPage, this, &WirelessSettings::requestNextPage);
    connect(ipv4Section, &IpvxSection::requestPage, this, &WirelessSettings::requestNextPage);
    connect(ipv6Section, &IpvxSection::requestPage, this, &WirelessSettings::requestNextPage);

    m_sectionsLayout->addWidget(genericSection);
    m_sectionsLayout->addWidget(secretSection);
    m_sectionsLayout->addWidget(ipv4Section);
    m_sectionsLayout->addWidget(ipv6Section);
    m_sectionsLayout->addWidget(wirelessSection);

    m_settingSections.append(genericSection);
    m_settingSections.append(secretSection);
    m_settingSections.append(ipv4Section);
    m_settingSections.append(ipv6Section);
    m_settingSections.append(wirelessSection);
}
