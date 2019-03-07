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

#include "vpnstrongswansettings.h"
#include "../../sections/genericsection.h"
#include "../../sections/vpn/vpnstrongswansection.h"
#include "../../sections/ipvxsection.h"

using namespace dcc::network;
using namespace NetworkManager;

VpnStrongSwanSettings::VpnStrongSwanSettings(NetworkManager::ConnectionSettings::Ptr connSettings, QWidget *parent)
    : AbstractSettings(connSettings, parent)
{
    initSections();
}

VpnStrongSwanSettings::~VpnStrongSwanSettings()
{
}

void VpnStrongSwanSettings::initSections()
{
    NetworkManager::VpnSetting::Ptr vpnSetting = m_connSettings->setting(
            NetworkManager::Setting::SettingType::Vpn).staticCast<NetworkManager::VpnSetting>();

    if (!vpnSetting) {
        qDebug() << "vpn setting is invalid...";
        return;
    }

    GenericSection *genericSection = new GenericSection(m_connSettings);

    VpnStrongSwanSection *vpnStrongSwanSection = new VpnStrongSwanSection(vpnSetting);

    IpvxSection *ipv4Section = new IpvxSection(
            m_connSettings->setting(Setting::SettingType::Ipv4).staticCast<NetworkManager::Ipv4Setting>());
    ipv4Section->setIpv4ConfigMethodEnable(NetworkManager::Ipv4Setting::ConfigMethod::Manual, false);
    ipv4Section->setNeverDefaultEnable(true);
    //IpvxSection *ipv6Section = new IpvxSection(
            //m_connSettings->setting(Setting::SettingType::Ipv6).staticCast<NetworkManager::Ipv6Setting>());

    connect(vpnStrongSwanSection, &VpnStrongSwanSection::requestNextPage,
            this, &VpnStrongSwanSettings::requestNextPage);
    connect(ipv4Section, &IpvxSection::requestNextPage, this, &VpnStrongSwanSettings::requestNextPage);
    //connect(ipv6Section, &IpvxSection::requestNextPage, this, &VpnStrongSwanSettings::requestNextPage);

    connect(vpnStrongSwanSection, &VpnStrongSwanSection::requestFrameAutoHide,
            this, &VpnStrongSwanSettings::requestFrameAutoHide);
    connect(ipv4Section, &IpvxSection::requestFrameAutoHide, this, &VpnStrongSwanSettings::requestFrameAutoHide);
    //connect(ipv6Section, &IpvxSection::requestFrameAutoHide, this, &VpnStrongSwanSettings::requestFrameAutoHide);

    m_sectionsLayout->addWidget(genericSection);
    m_sectionsLayout->addWidget(vpnStrongSwanSection);
    m_sectionsLayout->addWidget(ipv4Section);
    //m_sectionsLayout->addWidget(ipv6Section);

    m_settingSections.append(genericSection);
    m_settingSections.append(vpnStrongSwanSection);
    m_settingSections.append(ipv4Section);
    //m_settingSections.append(ipv6Section);
}
