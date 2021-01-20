/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     listenerri <listenerri@gmail.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "vpnopenconnectsettings.h"
#include "../../sections/genericsection.h"
#include "../../sections/vpn/vpnopenconnectsection.h"
#include "../../sections/ipvxsection.h"
#include "../../sections/dnssection.h"

using namespace DCC_NAMESPACE::network;
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
    NetworkManager::VpnSetting::Ptr vpnSetting = m_connSettings->setting(
            NetworkManager::Setting::SettingType::Vpn).staticCast<NetworkManager::VpnSetting>();

    if (!vpnSetting) {
        qDebug() << "vpn setting is invalid...";
        return;
    }

    GenericSection *genericSection = new GenericSection(m_connSettings);
    genericSection->setConnectionType(NetworkManager::ConnectionSettings::Vpn);

    VpnOpenConnectSection *vpnOpenConnectSection = new VpnOpenConnectSection(vpnSetting);

    IpvxSection *ipv4Section = new IpvxSection(
            m_connSettings->setting(Setting::SettingType::Ipv4).staticCast<NetworkManager::Ipv4Setting>());
    ipv4Section->setIpv4ConfigMethodEnable(NetworkManager::Ipv4Setting::ConfigMethod::Manual, false);
    ipv4Section->setNeverDefaultEnable(true);

    IpvxSection *ipv6Section = new IpvxSection(
            m_connSettings->setting(Setting::SettingType::Ipv6).staticCast<NetworkManager::Ipv6Setting>());
    ipv6Section->setIpv6ConfigMethodEnable(NetworkManager::Ipv6Setting::ConfigMethod::Manual, false);
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
