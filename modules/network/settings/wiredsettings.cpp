#include "wiredsettings.h"
#include "../sections/genericsection.h"
#include "../sections/secretwiredsection.h"
#include "../sections/ipvxsection.h"
#include "../sections/ethernetsection.h"

using namespace dcc::widgets;
using namespace NetworkManager;

WiredSettings::WiredSettings(NetworkManager::ConnectionSettings::Ptr connSettings, QWidget *parent)
    : AbstractSettings(connSettings, parent)
{
    initSections();
}

WiredSettings::~WiredSettings()
{
}

void WiredSettings::initSections()
{
    GenericSection *genericSection = new GenericSection(m_connSettings);
    Secret8021xSection *secretSection = new SecretWiredSection(
            m_connSettings->setting(Setting::Security8021x).staticCast<NetworkManager::Security8021xSetting>());
    IpvxSection *ipv4Section = new IpvxSection(
            m_connSettings->setting(Setting::Ipv4).staticCast<NetworkManager::Ipv4Setting>());
    IpvxSection *ipv6Section = new IpvxSection(
            m_connSettings->setting(Setting::Ipv6).staticCast<NetworkManager::Ipv6Setting>());
    EthernetSection *etherNetSection = new EthernetSection(
            m_connSettings->setting(Setting::Wired).staticCast<NetworkManager::WiredSetting>());

    connect(secretSection, &Secret8021xSection::requestPage, this, &WiredSettings::requestNextPage);
    connect(ipv4Section, &IpvxSection::requestPage, this, &WiredSettings::requestNextPage);
    connect(ipv6Section, &IpvxSection::requestPage, this, &WiredSettings::requestNextPage);
    connect(etherNetSection, &EthernetSection::requestPage, this, &WiredSettings::requestNextPage);

    m_sectionsLayout->addWidget(genericSection);
    m_sectionsLayout->addWidget(secretSection);
    m_sectionsLayout->addWidget(ipv4Section);
    m_sectionsLayout->addWidget(ipv6Section);
    m_sectionsLayout->addWidget(etherNetSection);

    m_settingSections.append(genericSection);
    m_settingSections.append(secretSection);
    m_settingSections.append(ipv4Section);
    m_settingSections.append(ipv6Section);
    m_settingSections.append(etherNetSection);
}
