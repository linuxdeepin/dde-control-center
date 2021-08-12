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

#include "dslpppoesettings.h"
#include "../sections/genericsection.h"
#include "../sections/pppoesection.h"
#include "../sections/ipvxsection.h"
#include "../sections/dnssection.h"
#include "../sections/pppsection.h"
#include "../sections/ethernetsection.h"
#include "../connectioneditpage.h"

using namespace DCC_NAMESPACE::network;
using namespace NetworkManager;

DslPppoeSettings::DslPppoeSettings(NetworkManager::ConnectionSettings::Ptr connSettings, QString devPath, QWidget *parent)
    : AbstractSettings(connSettings, parent)
    , m_devicePath(devPath)
    , m_parent(parent)
{
    setAccessibleName("DslPppoeSettings");
    initSections();
}

DslPppoeSettings::~DslPppoeSettings()
{
}

void DslPppoeSettings::initSections()
{
    GenericSection *genericSection = new GenericSection(m_connSettings);
    PPPOESection *pppoeSection = new PPPOESection(
        m_connSettings->setting(Setting::Pppoe).staticCast<NetworkManager::PppoeSetting>());
    IpvxSection *ipv4Section = new IpvxSection(
        m_connSettings->setting(Setting::Ipv4).staticCast<NetworkManager::Ipv4Setting>());
    DNSSection *dnsSection = new DNSSection(m_connSettings, false);
    m_etherNetSection = new EthernetSection(
        m_connSettings->setting(Setting::Wired).staticCast<NetworkManager::WiredSetting>(), m_devicePath);
    PPPSection *pppSection = new PPPSection(
        m_connSettings->setting(Setting::Ppp).staticCast<NetworkManager::PppSetting>());

    connect(genericSection, &GenericSection::editClicked, this, &DslPppoeSettings::anyEditClicked);
    connect(pppoeSection, &EthernetSection::editClicked, this, &DslPppoeSettings::anyEditClicked);
    connect(ipv4Section, &IpvxSection::editClicked, this, &DslPppoeSettings::anyEditClicked);
    connect(dnsSection, &DNSSection::editClicked, this, &DslPppoeSettings::anyEditClicked);
    connect(m_etherNetSection, &EthernetSection::editClicked, this, &DslPppoeSettings::anyEditClicked);
    connect(pppSection, &IpvxSection::editClicked, this, &DslPppoeSettings::anyEditClicked);
    connect(dnsSection, &DNSSection::editClicked, this, &DslPppoeSettings::anyEditClicked);

    connect(ipv4Section, &IpvxSection::requestNextPage, this, &DslPppoeSettings::requestNextPage);
    connect(dnsSection, &DNSSection::requestNextPage, this, &DslPppoeSettings::requestNextPage);
    connect(m_etherNetSection, &EthernetSection::requestNextPage, this, &DslPppoeSettings::requestNextPage);

    m_sectionsLayout->addWidget(genericSection);
    m_sectionsLayout->addWidget(pppoeSection);
    m_sectionsLayout->addWidget(ipv4Section);
    m_sectionsLayout->addWidget(dnsSection);
    m_sectionsLayout->addWidget(m_etherNetSection);
    m_sectionsLayout->addWidget(pppSection);

    m_settingSections.append(genericSection);
    m_settingSections.append(pppoeSection);
    m_settingSections.append(ipv4Section);
    m_settingSections.append(dnsSection);
    m_settingSections.append(m_etherNetSection);
    m_settingSections.append(pppSection);
}

bool DslPppoeSettings::clearInterfaceName()
{
    ConnectionEditPage *page = dynamic_cast<ConnectionEditPage *>(m_parent);
    if (page) page->setDevicePath(m_etherNetSection->devicePath());
    NetworkManager::WiredSetting::Ptr wiredSetting = m_connSettings->setting(Setting::Wired).staticCast<NetworkManager::WiredSetting>();
    return wiredSetting->macAddress().isEmpty();
}
