/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
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

using namespace NetworkManager;

HotspotSettings::HotspotSettings(ConnectionSettings::Ptr connSettings, QWidget *parent)
    : AbstractSettings(connSettings, parent)
{
    initSections();
}

HotspotSettings::~HotspotSettings()
{
}

void HotspotSettings::initSections()
{
    WirelessSetting::Ptr wirelessSetting = m_connSettings->setting(Setting::SettingType::Wireless).staticCast<WirelessSetting>();
    wirelessSetting->setInitialized(true);
    wirelessSetting->setMode(WirelessSetting::NetworkMode::Ap);
    wirelessSetting->setSecurity("802-11-wireless-security");

    GenericHotspotSection *genericSection = new GenericHotspotSection(m_connSettings);

    SecretHotspotSection *secretHotspotSection = new SecretHotspotSection(
        m_connSettings->setting(Setting::SettingType::WirelessSecurity)
        .staticCast<WirelessSecuritySetting>());

    WirelessSection *wirelessSection = new WirelessSection(wirelessSetting, true);

    connect(genericSection, &GenericHotspotSection::editClicked, this, &HotspotSettings::anyEditClicked);
    connect(secretHotspotSection, &GenericHotspotSection::editClicked, this, &HotspotSettings::anyEditClicked);
    connect(wirelessSection, &GenericHotspotSection::editClicked, this, &HotspotSettings::anyEditClicked);

    m_sectionsLayout->addWidget(genericSection);
    m_sectionsLayout->addWidget(secretHotspotSection);
    m_sectionsLayout->addWidget(wirelessSection);

    m_settingSections.append(genericSection);
    m_settingSections.append(secretHotspotSection);
    m_settingSections.append(wirelessSection);
}

bool HotspotSettings::clearInterfaceName()
{
    WirelessSetting::Ptr wirelessSetting = m_connSettings->setting(Setting::SettingType::Wireless).staticCast<WirelessSetting>();
    return wirelessSetting->macAddress().isEmpty();
}
