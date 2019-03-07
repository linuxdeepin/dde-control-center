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

#include "vpnvpncsettings.h"
#include "../../sections/genericsection.h"
#include "../../sections/vpn/vpnvpncsection.h"
#include "../../sections/vpn/vpnadvvpncsection.h"
#include "../../sections/ipvxsection.h"

using namespace dcc::network;
using namespace NetworkManager;

VpnVPNCSettings::VpnVPNCSettings(NetworkManager::ConnectionSettings::Ptr connSettings, QWidget *parent)
    : AbstractSettings(connSettings, parent)
{
    initSections();
}

VpnVPNCSettings::~VpnVPNCSettings()
{
}

void VpnVPNCSettings::initSections()
{
    NetworkManager::VpnSetting::Ptr vpnSetting = m_connSettings->setting(
            NetworkManager::Setting::SettingType::Vpn).staticCast<NetworkManager::VpnSetting>();

    if (!vpnSetting) {
        qDebug() << "vpn setting is invalid...";
        return;
    }

    GenericSection *genericSection = new GenericSection(m_connSettings);
    VpnVPNCSection *vpnVPNCSection = new VpnVPNCSection(vpnSetting);
    VpnAdvVPNCSection *vpnAdvVPNCSection = new VpnAdvVPNCSection(vpnSetting);
    IpvxSection *ipv4Section = new IpvxSection(
            m_connSettings->setting(Setting::SettingType::Ipv4).staticCast<NetworkManager::Ipv4Setting>());
    ipv4Section->setIpv4ConfigMethodEnable(NetworkManager::Ipv4Setting::ConfigMethod::Manual, false);
    ipv4Section->setNeverDefaultEnable(true);
    //IpvxSection *ipv6Section = new IpvxSection(
            //m_connSettings->setting(Setting::SettingType::Ipv6).staticCast<NetworkManager::Ipv6Setting>());

    connect(vpnVPNCSection, &VpnVPNCSection::requestNextPage, this, &VpnVPNCSettings::requestNextPage);
    connect(vpnAdvVPNCSection, &VpnAdvVPNCSection::requestNextPage, this, &VpnVPNCSettings::requestNextPage);
    connect(ipv4Section, &IpvxSection::requestNextPage, this, &VpnVPNCSettings::requestNextPage);
    //connect(ipv6Section, &IpvxSection::requestNextPage, this, &VpnVPNCSettings::requestNextPage);

    connect(vpnVPNCSection, &VpnVPNCSection::requestFrameAutoHide, this, &VpnVPNCSettings::requestFrameAutoHide);
    connect(vpnAdvVPNCSection, &VpnAdvVPNCSection::requestFrameAutoHide, this, &VpnVPNCSettings::requestFrameAutoHide);
    connect(ipv4Section, &IpvxSection::requestFrameAutoHide, this, &VpnVPNCSettings::requestFrameAutoHide);
    //connect(ipv6Section, &IpvxSection::requestFrameAutoHide, this, &VpnVPNCSettings::requestFrameAutoHide);

    m_sectionsLayout->addWidget(genericSection);
    m_sectionsLayout->addWidget(vpnVPNCSection);
    m_sectionsLayout->addWidget(vpnAdvVPNCSection);
    m_sectionsLayout->addWidget(ipv4Section);
    //m_sectionsLayout->addWidget(ipv6Section);

    m_settingSections.append(genericSection);
    m_settingSections.append(vpnVPNCSection);
    m_settingSections.append(vpnAdvVPNCSection);
    m_settingSections.append(ipv4Section);
    //m_settingSections.append(ipv6Section);
}
