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

#include "wirelesssettings.h"
#include "../sections/genericsection.h"
#include "../sections/secretwirelesssection.h"
#include "../sections/ipvxsection.h"
#include "../sections/wirelesssection.h"

using namespace dcc::network;
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
    genericSection->setConnectionNameEditable(false);

    SecretWirelessSection *secretSection = new SecretWirelessSection(
            m_connSettings->setting(Setting::SettingType::WirelessSecurity).staticCast<NetworkManager::WirelessSecuritySetting>(),
            m_connSettings->setting(Setting::SettingType::Security8021x).staticCast<NetworkManager::Security8021xSetting>());

    IpvxSection *ipv4Section = new IpvxSection(
            m_connSettings->setting(Setting::SettingType::Ipv4).staticCast<NetworkManager::Ipv4Setting>());

    IpvxSection *ipv6Section = new IpvxSection(
            m_connSettings->setting(Setting::SettingType::Ipv6).staticCast<NetworkManager::Ipv6Setting>());
    
    WirelessSection *wirelessSection = new WirelessSection(
            m_connSettings->setting(Setting::SettingType::Wireless).staticCast<NetworkManager::WirelessSetting>());
    // we need enable ssid text edit since it is a hidden wifi edit page if ssid is empty
    if (!wirelessSection->ssid().isEmpty()) {
        wirelessSection->setSsidEditable(false);
    }

    connect(wirelessSection, &WirelessSection::requestNextPage, this, &WirelessSettings::requestNextPage);
    connect(secretSection, &Secret8021xSection::requestNextPage, this, &WirelessSettings::requestNextPage);
    connect(ipv4Section, &IpvxSection::requestNextPage, this, &WirelessSettings::requestNextPage);
    connect(ipv6Section, &IpvxSection::requestNextPage, this, &WirelessSettings::requestNextPage);

    connect(wirelessSection, &WirelessSection::requestFrameAutoHide, this, &WirelessSettings::requestFrameAutoHide);
    connect(secretSection, &Secret8021xSection::requestFrameAutoHide, this, &WirelessSettings::requestFrameAutoHide);
    connect(ipv4Section, &IpvxSection::requestFrameAutoHide, this, &WirelessSettings::requestFrameAutoHide);
    connect(ipv6Section, &IpvxSection::requestFrameAutoHide, this, &WirelessSettings::requestFrameAutoHide);

    connect(wirelessSection, &WirelessSection::ssidChanged, genericSection, &GenericSection::setConnectionName);

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

bool WirelessSettings::clearInterfaceName()
{
    NetworkManager::WirelessSetting::Ptr wirelessSetting = m_connSettings->setting(Setting::SettingType::Wireless).staticCast<NetworkManager::WirelessSetting>();
    return wirelessSetting->macAddress().isEmpty();
}
