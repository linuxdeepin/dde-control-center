// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "hotspotsettings.h"
#include "../sections/generichotspotsection.h"
#include "../sections/secrethotspotsection.h"
#include "../sections/wirelesssection.h"
#include "../connectioneditpage.h"

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

    WirelessSection *wirelessSection = new WirelessSection(m_connSettings, wirelessSetting, ConnectionEditPage::devicePath(), true);

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
