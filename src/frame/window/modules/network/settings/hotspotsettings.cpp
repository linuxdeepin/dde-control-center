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

#include "hotspotsettings.h"
#include "../sections/generichotspotsection.h"
#include "../sections/secrethotspotsection.h"
#include "../sections/wirelesssection.h"
#include "../sections/cbandsection.h"

using namespace DCC_NAMESPACE::network;
using namespace NetworkManager;

HotspotSettings::HotspotSettings(NetworkManager::ConnectionSettings::Ptr connSettings, QWidget *parent)
    : AbstractSettings(connSettings, parent)
{
    initSections();
}

HotspotSettings::~HotspotSettings()
{
}

void HotspotSettings::initSections()
{
    NetworkManager::WirelessSetting::Ptr wirelessSetting =
        m_connSettings->setting(Setting::SettingType::Wireless).staticCast<NetworkManager::WirelessSetting>();
    wirelessSetting->setMode(NetworkManager::WirelessSetting::NetworkMode::Ap);

    GenericHotspotSection *genericSection = new GenericHotspotSection(m_connSettings);

    SecretHotspotSection *secretHotspotSection = new SecretHotspotSection(
        m_connSettings->setting(Setting::SettingType::WirelessSecurity)
        .staticCast<NetworkManager::WirelessSecuritySetting>());

    WirelessSection *wirelessSection = new WirelessSection(wirelessSetting, true);
    CbandSection *bandSection = new CbandSection(wirelessSetting, nullptr);
    m_sectionsLayout->addWidget(genericSection);
    m_sectionsLayout->addWidget(secretHotspotSection);
    m_sectionsLayout->addWidget(bandSection);
    m_sectionsLayout->addWidget(wirelessSection);

    m_settingSections.append(genericSection);
    m_settingSections.append(secretHotspotSection);
    m_settingSections.append(wirelessSection);
    m_settingSections.append(bandSection);
}

bool HotspotSettings::clearInterfaceName()
{
    NetworkManager::WirelessSetting::Ptr wirelessSetting = m_connSettings->setting(Setting::SettingType::Wireless).staticCast<NetworkManager::WirelessSetting>();
    return wirelessSetting->macAddress().isEmpty();
}
