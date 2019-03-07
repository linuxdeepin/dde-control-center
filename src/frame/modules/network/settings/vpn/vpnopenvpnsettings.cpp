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

#include "vpnopenvpnsettings.h"
#include "../../sections/genericsection.h"
#include "../../sections/vpn/vpnopenvpnsection.h"
#include "../../sections/vpn/vpnadvopenvpnsection.h"
#include "../../sections/vpn/vpnsecopenvpnsection.h"
#include "../../sections/vpn/vpnproxysection.h"
#include "../../sections/vpn/vpntlssection.h"
#include "../../sections/ipvxsection.h"

using namespace dcc::network;
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

    VpnTLSSection *vpnTLSSection = new VpnTLSSection(vpnSetting);
    if (vpnOpenVPNSection->authType() == "static-key") {
        vpnTLSSection->setVisible(false);
    }

    connect(vpnOpenVPNSection, &VpnOpenVPNSection::requestNextPage, this, &VpnOpenVPNSettings::requestNextPage);
    connect(vpnOpenVPNSection, &VpnOpenVPNSection::authTypeChanged, this, [=](const QString &type) {
        vpnTLSSection->setVisible(type != "static-key");
    });
    connect(vpnAdvOpenVPNSection, &VpnAdvOpenVPNSection::requestNextPage, this, &VpnOpenVPNSettings::requestNextPage);
    connect(vpnSecOpenVPNSection, &VpnSecOpenVPNSection::requestNextPage, this, &VpnOpenVPNSettings::requestNextPage);
    connect(vpnProxySection, &VpnProxySection::requestNextPage, this, &VpnOpenVPNSettings::requestNextPage);
    connect(ipv4Section, &IpvxSection::requestNextPage, this, &VpnOpenVPNSettings::requestNextPage);
    connect(ipv6Section, &IpvxSection::requestNextPage, this, &VpnOpenVPNSettings::requestNextPage);
    connect(vpnTLSSection, &VpnTLSSection::requestNextPage, this, &VpnOpenVPNSettings::requestNextPage);

    connect(vpnOpenVPNSection, &VpnOpenVPNSection::requestFrameAutoHide, this, &VpnOpenVPNSettings::requestFrameAutoHide);
    connect(vpnAdvOpenVPNSection, &VpnAdvOpenVPNSection::requestFrameAutoHide, this, &VpnOpenVPNSettings::requestFrameAutoHide);
    connect(vpnSecOpenVPNSection, &VpnSecOpenVPNSection::requestFrameAutoHide, this, &VpnOpenVPNSettings::requestFrameAutoHide);
    connect(vpnProxySection, &VpnProxySection::requestFrameAutoHide, this, &VpnOpenVPNSettings::requestFrameAutoHide);
    connect(ipv4Section, &IpvxSection::requestFrameAutoHide, this, &VpnOpenVPNSettings::requestFrameAutoHide);
    connect(ipv6Section, &IpvxSection::requestFrameAutoHide, this, &VpnOpenVPNSettings::requestFrameAutoHide);
    connect(vpnTLSSection, &VpnTLSSection::requestFrameAutoHide, this, &VpnOpenVPNSettings::requestFrameAutoHide);

    m_sectionsLayout->addWidget(genericSection);
    m_sectionsLayout->addWidget(vpnOpenVPNSection);
    m_sectionsLayout->addWidget(vpnAdvOpenVPNSection);
    m_sectionsLayout->addWidget(vpnSecOpenVPNSection);
    m_sectionsLayout->addWidget(vpnProxySection);
    m_sectionsLayout->addWidget(ipv4Section);
    m_sectionsLayout->addWidget(ipv6Section);
    m_sectionsLayout->addWidget(vpnTLSSection);

    m_settingSections.append(genericSection);
    m_settingSections.append(vpnOpenVPNSection);
    m_settingSections.append(vpnAdvOpenVPNSection);
    m_settingSections.append(vpnSecOpenVPNSection);
    m_settingSections.append(vpnProxySection);
    m_settingSections.append(ipv4Section);
    m_settingSections.append(ipv6Section);
    m_settingSections.append(vpnTLSSection);
}
