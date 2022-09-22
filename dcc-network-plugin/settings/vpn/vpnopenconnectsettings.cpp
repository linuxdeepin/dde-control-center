// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "vpnopenconnectsettings.h"
#include "../../sections/genericsection.h"
#include "../../sections/vpn/vpnopenconnectsection.h"
#include "../../sections/ipvxsection.h"
#include "../../sections/dnssection.h"

#include <QVBoxLayout>

#include <widgets/contentwidget.h>

using namespace NetworkManager;

VpnOpenConnectSettings::VpnOpenConnectSettings(ConnectionSettings::Ptr connSettings, QWidget *parent)
    : AbstractSettings(connSettings, parent)
{
    initSections();
}

VpnOpenConnectSettings::~VpnOpenConnectSettings()
{
}

void VpnOpenConnectSettings::initSections()
{
    VpnSetting::Ptr vpnSetting = m_connSettings->setting(Setting::SettingType::Vpn).staticCast<VpnSetting>();

    if (!vpnSetting)
        return;

    GenericSection *genericSection = new GenericSection(m_connSettings);
    genericSection->setConnectionType(ConnectionSettings::Vpn);

    VpnOpenConnectSection *vpnOpenConnectSection = new VpnOpenConnectSection(vpnSetting);

    IpvxSection *ipv4Section = new IpvxSection(m_connSettings->setting(Setting::SettingType::Ipv4).staticCast<Ipv4Setting>());
    ipv4Section->setIpv4ConfigMethodEnable(Ipv4Setting::ConfigMethod::Manual, false);
    ipv4Section->setNeverDefaultEnable(true);

    IpvxSection *ipv6Section = new IpvxSection(m_connSettings->setting(Setting::SettingType::Ipv6).staticCast<Ipv6Setting>());
    ipv6Section->setIpv6ConfigMethodEnable(Ipv6Setting::ConfigMethod::Manual, false);
    ipv6Section->setNeverDefaultEnable(true);
    DNSSection *dnsSection = new DNSSection(m_connSettings);

    connect(genericSection, &GenericSection::editClicked, this, &VpnOpenConnectSettings::anyEditClicked);
    connect(vpnOpenConnectSection, &VpnOpenConnectSection::editClicked, this, &VpnOpenConnectSettings::anyEditClicked);
    connect(ipv4Section, &IpvxSection::editClicked, this, &VpnOpenConnectSettings::anyEditClicked);
    connect(ipv6Section, &IpvxSection::editClicked, this, &VpnOpenConnectSettings::anyEditClicked);
    connect(dnsSection, &DNSSection::editClicked, this, &VpnOpenConnectSettings::anyEditClicked);

    connect(ipv4Section, &IpvxSection::requestNextPage, this, &VpnOpenConnectSettings::requestNextPage);
    connect(ipv6Section, &IpvxSection::requestNextPage, this, &VpnOpenConnectSettings::requestNextPage);
    connect(dnsSection, &DNSSection::requestNextPage, this, &VpnOpenConnectSettings::requestNextPage);

    connect(vpnOpenConnectSection, &VpnOpenConnectSection::requestFrameAutoHide, this, &VpnOpenConnectSettings::requestFrameAutoHide);
    connect(ipv4Section, &IpvxSection::requestFrameAutoHide, this, &VpnOpenConnectSettings::requestFrameAutoHide);
    connect(ipv6Section, &IpvxSection::requestFrameAutoHide, this, &VpnOpenConnectSettings::requestFrameAutoHide);
    connect(dnsSection, &DNSSection::requestFrameAutoHide, this, &VpnOpenConnectSettings::requestFrameAutoHide);

    m_sectionsLayout->addWidget(genericSection);
    m_sectionsLayout->addWidget(vpnOpenConnectSection);
    m_sectionsLayout->addWidget(ipv4Section);
    m_sectionsLayout->addWidget(ipv6Section);
    m_sectionsLayout->addWidget(dnsSection);

    m_settingSections.append(genericSection);
    m_settingSections.append(vpnOpenConnectSection);
    m_settingSections.append(ipv4Section);
    m_settingSections.append(ipv6Section);
    m_settingSections.append(dnsSection);
}
