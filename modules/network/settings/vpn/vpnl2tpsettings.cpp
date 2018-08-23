#include "vpnl2tpsettings.h"
#include "../../sections/genericsection.h"
#include "../../sections/vpn/vpnsection.h"
#include "../../sections/vpn/vpnpppsection.h"
#include "../../sections/vpn/vpnipsecsection.h"
#include "../../sections/ipvxsection.h"

using namespace dcc::widgets;
using namespace NetworkManager;

VpnL2tpSettings::VpnL2tpSettings(NetworkManager::ConnectionSettings::Ptr connSettings, QWidget *parent)
    : AbstractSettings(connSettings, parent)
{
    initSections();
}

VpnL2tpSettings::~VpnL2tpSettings()
{
}

void VpnL2tpSettings::initSections()
{
    NetworkManager::VpnSetting::Ptr vpnSetting = m_connSettings->setting(
            NetworkManager::Setting::SettingType::Vpn).staticCast<NetworkManager::VpnSetting>();

    if (!vpnSetting) {
        qDebug() << "vpn setting is invalid...";
        return;
    }

    GenericSection *genericSection = new GenericSection(m_connSettings);

    VpnSection *vpnSection = new VpnSection(vpnSetting);

    VpnPPPSection *vpnPPPSection = new VpnPPPSection(vpnSetting);
    QStringList supportOptions = {
        "refuse-eap", "refuse-pap", "refuse-chap", "refuse-mschap", "refuse-mschapv2",
        "nobsdcomp", "nodeflate", "no-vj-comp", "nopcomp", "noaccomp", "lcp-echo-interval"
    };
    vpnPPPSection->setSupportOptions(supportOptions);

    VpnIpsecSection *vpnIpsecSection = new VpnIpsecSection(vpnSetting);

    IpvxSection *ipv4Section = new IpvxSection(
            m_connSettings->setting(Setting::SettingType::Ipv4).staticCast<NetworkManager::Ipv4Setting>());
    ipv4Section->setIpv4ConfigMethodEnable(NetworkManager::Ipv4Setting::ConfigMethod::Manual, false);
    ipv4Section->setNeverDefaultEnable(true);

    //IpvxSection *ipv6Section = new IpvxSection(
            //m_connSettings->setting(Setting::SettingType::Ipv6).staticCast<NetworkManager::Ipv6Setting>());

    connect(vpnSection, &VpnSection::requestPage, this, &VpnL2tpSettings::requestNextPage);
    connect(vpnPPPSection, &VpnPPPSection::requestPage, this, &VpnL2tpSettings::requestNextPage);
    connect(vpnIpsecSection, &VpnIpsecSection::requestPage, this, &VpnL2tpSettings::requestNextPage);
    connect(ipv4Section, &IpvxSection::requestPage, this, &VpnL2tpSettings::requestNextPage);
    //connect(ipv6Section, &IpvxSection::requestPage, this, &VpnL2tpSettings::requestNextPage);

    m_sectionsLayout->addWidget(genericSection);
    m_sectionsLayout->addWidget(vpnSection);
    m_sectionsLayout->addWidget(vpnPPPSection);
    m_sectionsLayout->addWidget(vpnIpsecSection);
    m_sectionsLayout->addWidget(ipv4Section);
    //m_sectionsLayout->addWidget(ipv6Section);

    m_settingSections.append(genericSection);
    m_settingSections.append(vpnSection);
    m_settingSections.append(vpnPPPSection);
    m_settingSections.append(vpnIpsecSection);
    m_settingSections.append(ipv4Section);
    //m_settingSections.append(ipv6Section);
}
