#include "vpnopenconnectsettings.h"
#include "../../sections/genericsection.h"
#include "../../sections/ipvxsection.h"

using namespace dcc::widgets;
using namespace NetworkManager;

VpnOpenConnectSettings::VpnOpenConnectSettings(NetworkManager::ConnectionSettings::Ptr connSettings, QWidget *parent)
    : AbstractSettings(connSettings, parent)
{
    initSections();
}

VpnOpenConnectSettings::~VpnOpenConnectSettings()
{
}

void VpnOpenConnectSettings::initSections()
{
    //NetworkManager::VpnSetting::Ptr vpnSetting = m_connSettings->setting(
            //NetworkManager::Setting::SettingType::Vpn).staticCast<NetworkManager::VpnSetting>();

    //if (!vpnSetting) {
        //qDebug() << "vpn setting is invalid...";
        //return;
    //}

    GenericSection *genericSection = new GenericSection(m_connSettings);
    IpvxSection *ipv4Section = new IpvxSection(
            m_connSettings->setting(Setting::SettingType::Ipv4).staticCast<NetworkManager::Ipv4Setting>());
    ipv4Section->setIpv4ConfigMethodEnable(NetworkManager::Ipv4Setting::ConfigMethod::Manual, false);
    ipv4Section->setNeverDefaultEnable(true);
    //IpvxSection *ipv6Section = new IpvxSection(
            //m_connSettings->setting(Setting::SettingType::Ipv6).staticCast<NetworkManager::Ipv6Setting>());

    connect(ipv4Section, &IpvxSection::requestPage, this, &VpnOpenConnectSettings::requestNextPage);
    //connect(ipv6Section, &IpvxSection::requestPage, this, &VpnOpenConnectSettings::requestNextPage);

    m_sectionsLayout->addWidget(genericSection);
    m_sectionsLayout->addWidget(ipv4Section);
    //m_sectionsLayout->addWidget(ipv6Section);

    m_settingSections.append(genericSection);
    m_settingSections.append(ipv4Section);
    //m_settingSections.append(ipv6Section);
}
