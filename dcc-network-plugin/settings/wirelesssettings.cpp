// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "wirelesssettings.h"
#include "../sections/genericsection.h"
#include "../sections/secretwirelesssection.h"
#include "../sections/multiipvxsection.h"
#include "../sections/dnssection.h"
#include "../sections/wirelesssection.h"
#include "../window/gsettingwatcher.h"
#include "../connectioneditpage.h"

#include <widgets/switchwidget.h>
#include <widgets/contentwidget.h>

using namespace NetworkManager;

WirelessSettings::WirelessSettings(ConnectionSettings::Ptr connSettings, ParametersContainer::Ptr parameters, QWidget *parent)
    : AbstractSettings(connSettings, parent)
    , m_parameter(parameters)
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
        m_connSettings->setting(Setting::SettingType::WirelessSecurity).staticCast<WirelessSecuritySetting>(),
        m_connSettings->setting(Setting::SettingType::Security8021x).staticCast<Security8021xSetting>(), m_parameter);

    MultiIpvxSection *ipv4Section = new MultiIpvxSection(m_connSettings->setting(Setting::SettingType::Ipv4).staticCast<Ipv4Setting>());
    MultiIpvxSection *ipv6Section = new MultiIpvxSection(m_connSettings->setting(Setting::SettingType::Ipv6).staticCast<Ipv6Setting>());

    DNSSection *dnsSection = new DNSSection(m_connSettings);

    WirelessSection *wirelessSection = new WirelessSection(m_connSettings, m_connSettings->setting(Setting::SettingType::Wireless).staticCast<WirelessSetting>(), ConnectionEditPage::devicePath());
    // we need enable ssid text edit since it is a hidden wifi edit page if ssid is empty
    if (!wirelessSection->ssid().isEmpty())
        wirelessSection->setSsidEditable(false);

    // 不用解绑，需要监控的控件有限，不会无限增长
    GSettingWatcher::instance()->bind("wirelessAutoConnect", genericSection->autoConnItem());
    GSettingWatcher::instance()->bind("wirelessSecurity", secretSection);
    GSettingWatcher::instance()->bind("wirelessIpv4", ipv4Section);
    GSettingWatcher::instance()->bind("wirelessIpv6", ipv6Section);
    GSettingWatcher::instance()->bind("wirelessWlan", wirelessSection);

    connect(genericSection, &GenericSection::editClicked, this, &WirelessSettings::anyEditClicked);
    connect(secretSection, &Secret8021xSection::editClicked, this, &WirelessSettings::anyEditClicked);
    connect(ipv4Section, &MultiIpvxSection::editClicked, this, &WirelessSettings::anyEditClicked);
    connect(ipv6Section, &MultiIpvxSection::editClicked, this, &WirelessSettings::anyEditClicked);
    connect(dnsSection, &DNSSection::editClicked, this, &WirelessSettings::anyEditClicked);
    connect(wirelessSection, &WirelessSection::editClicked, this, &WirelessSettings::anyEditClicked);

    connect(wirelessSection, &WirelessSection::requestNextPage, this, &WirelessSettings::requestNextPage);
    connect(secretSection, &Secret8021xSection::requestNextPage, this, &WirelessSettings::requestNextPage);
    connect(ipv4Section, &MultiIpvxSection::requestNextPage, this, &WirelessSettings::requestNextPage);
    connect(ipv6Section, &MultiIpvxSection::requestNextPage, this, &WirelessSettings::requestNextPage);
    connect(dnsSection, &DNSSection::requestNextPage, this, &WirelessSettings::requestNextPage);

    connect(wirelessSection, &WirelessSection::requestFrameAutoHide, this, &WirelessSettings::requestFrameAutoHide);
    connect(secretSection, &Secret8021xSection::requestFrameAutoHide, this, &WirelessSettings::requestFrameAutoHide);
    connect(ipv4Section, &MultiIpvxSection::requestFrameAutoHide, this, &WirelessSettings::requestFrameAutoHide);
    connect(ipv6Section, &MultiIpvxSection::requestFrameAutoHide, this, &WirelessSettings::requestFrameAutoHide);
    connect(dnsSection, &DNSSection::requestFrameAutoHide, this, &WirelessSettings::requestFrameAutoHide);

    connect(wirelessSection, &WirelessSection::ssidChanged, genericSection, &GenericSection::setConnectionName);

    m_sectionsLayout->addWidget(genericSection);
    m_sectionsLayout->addWidget(secretSection);
    m_sectionsLayout->addWidget(ipv4Section);
    m_sectionsLayout->addWidget(ipv6Section);
    m_sectionsLayout->addWidget(dnsSection);
    m_sectionsLayout->addWidget(wirelessSection);
    m_sectionsLayout->addStretch();

    m_settingSections.append(genericSection);
    m_settingSections.append(secretSection);
    m_settingSections.append(ipv4Section);
    m_settingSections.append(ipv6Section);
    m_settingSections.append(dnsSection);
    m_settingSections.append(wirelessSection);
}

bool WirelessSettings::clearInterfaceName()
{
    WirelessSetting::Ptr wirelessSetting = m_connSettings->setting(Setting::SettingType::Wireless).staticCast<WirelessSetting>();
    return wirelessSetting->macAddress().isEmpty();
}
