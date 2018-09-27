#include "vpnopenvpnsettings.h"
#include "../../sections/genericsection.h"
#include "../../sections/vpn/vpnopenvpnsection.h"
#include "../../sections/vpn/vpnadvopenvpnsection.h"
#include "../../sections/vpn/vpnsecopenvpnsection.h"
#include "../../sections/vpn/vpnproxysection.h"
#include "../../sections/ipvxsection.h"

using namespace dcc::widgets;
using namespace NetworkManager;

VpnOpenVPNSettings::VpnOpenVPNSettings(NetworkManager::ConnectionSettings::Ptr connSettings, QWidget *parent)
    : AbstractSettings(connSettings, parent)
{
    initSections();
}

VpnOpenVPNSettings::~VpnOpenVPNSettings()
{
}

void VpnOpenVPNSettings::initSections()
{
    NetworkManager::VpnSetting::Ptr vpnSetting = m_connSettings->setting(
            NetworkManager::Setting::SettingType::Vpn).staticCast<NetworkManager::VpnSetting>();

    if (!vpnSetting) {
        qDebug() << "vpn setting is invalid...";
        return;
    }

    GenericSection *genericSection = new GenericSection(m_connSettings);

    VpnOpenVPNSection *vpnOpenVPNSection = new VpnOpenVPNSection(vpnSetting);

    VpnAdvOpenVPNSection *vpnAdvOpenVPNSection = new VpnAdvOpenVPNSection(vpnSetting);

    VpnSecOpenVPNSection *vpnSecOpenVPNSection = new VpnSecOpenVPNSection(vpnSetting);

    VpnProxySection *vpnProxySection = new VpnProxySection(vpnSetting);

    IpvxSection *ipv4Section = new IpvxSection(
            m_connSettings->setting(Setting::SettingType::Ipv4).staticCast<NetworkManager::Ipv4Setting>());
    ipv4Section->setIpv4ConfigMethodEnable(NetworkManager::Ipv4Setting::ConfigMethod::Manual, false);
    ipv4Section->setNeverDefaultEnable(true);

    IpvxSection *ipv6Section = new IpvxSection(
            m_connSettings->setting(Setting::SettingType::Ipv6).staticCast<NetworkManager::Ipv6Setting>());
    ipv6Section->setIpv6ConfigMethodEnable(NetworkManager::Ipv6Setting::ConfigMethod::Manual, false);
    ipv6Section->setNeverDefaultEnable(true);

    connect(vpnOpenVPNSection, &VpnOpenVPNSection::requestPage, this, &VpnOpenVPNSettings::requestNextPage);
    connect(vpnAdvOpenVPNSection, &VpnAdvOpenVPNSection::requestPage, this, &VpnOpenVPNSettings::requestNextPage);
    connect(vpnSecOpenVPNSection, &VpnSecOpenVPNSection::requestPage, this, &VpnOpenVPNSettings::requestNextPage);
    connect(vpnProxySection, &VpnProxySection::requestPage, this, &VpnOpenVPNSettings::requestNextPage);
    connect(ipv4Section, &IpvxSection::requestPage, this, &VpnOpenVPNSettings::requestNextPage);
    connect(ipv6Section, &IpvxSection::requestPage, this, &VpnOpenVPNSettings::requestNextPage);

    m_sectionsLayout->addWidget(genericSection);
    m_sectionsLayout->addWidget(vpnOpenVPNSection);
    m_sectionsLayout->addWidget(vpnAdvOpenVPNSection);
    m_sectionsLayout->addWidget(vpnSecOpenVPNSection);
    m_sectionsLayout->addWidget(vpnProxySection);
    m_sectionsLayout->addWidget(ipv4Section);
    m_sectionsLayout->addWidget(ipv6Section);

    m_settingSections.append(genericSection);
    m_settingSections.append(vpnOpenVPNSection);
    m_settingSections.append(vpnAdvOpenVPNSection);
    m_settingSections.append(vpnSecOpenVPNSection);
    m_settingSections.append(vpnProxySection);
    m_settingSections.append(ipv4Section);
    m_settingSections.append(ipv6Section);
}
