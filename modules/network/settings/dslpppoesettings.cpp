#include "dslpppoesettings.h"
#include "../sections/genericsection.h"
#include "../sections/pppoesection.h"
#include "../sections/ipvxsection.h"
#include "../sections/pppsection.h"
#include "../sections/ethernetsection.h"

using namespace dcc::widgets;
using namespace NetworkManager;

DslPppoeSettings::DslPppoeSettings(NetworkManager::ConnectionSettings::Ptr connSettings, QWidget *parent)
    : AbstractSettings(connSettings, parent)
{
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
    //IpvxSection *ipv6Section = new IpvxSection(
            //m_connSettings->setting(Setting::Ipv6).staticCast<NetworkManager::Ipv6Setting>());
    EthernetSection *etherNetSection = new EthernetSection(
            m_connSettings->setting(Setting::Wired).staticCast<NetworkManager::WiredSetting>());
    PPPSection *pppSection = new PPPSection(
            m_connSettings->setting(Setting::Ppp).staticCast<NetworkManager::PppSetting>());

    connect(ipv4Section, &IpvxSection::requestPage, this, &DslPppoeSettings::requestNextPage);
    //connect(ipv6Section, &IpvxSection::requestPage, this, &DslPppoeSettings::requestNextPage);
    connect(etherNetSection, &EthernetSection::requestPage, this, &DslPppoeSettings::requestNextPage);

    m_sectionsLayout->addWidget(genericSection);
    m_sectionsLayout->addWidget(pppoeSection);
    m_sectionsLayout->addWidget(ipv4Section);
    //m_sectionsLayout->addWidget(ipv6Section);
    m_sectionsLayout->addWidget(etherNetSection);
    m_sectionsLayout->addWidget(pppSection);

    m_settingSections.append(genericSection);
    m_settingSections.append(pppoeSection);
    m_settingSections.append(ipv4Section);
    //m_settingSections.append(ipv6Section);
    m_settingSections.append(etherNetSection);
    m_settingSections.append(pppSection);
}
