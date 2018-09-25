#include "connectionvpneditpage.h"
#include "settings/vpn/vpnl2tpsettings.h"
#include "settings/vpn/vpnpptpsettings.h"
#include "settings/vpn/vpnvpncsettings.h"
#include "settings/vpn/vpnopenvpnsettings.h"
#include "settings/vpn/vpnstrongswansettings.h"
#include "settings/vpn/vpnopenconnectsettings.h"

#include <networkmanagerqt/vpnsetting.h>

#include <QDebug>

using namespace dcc::network;
using namespace dcc::widgets;
using namespace NetworkManager;

ConnectionVpnEditPage::ConnectionVpnEditPage(const QString &connUuid, QWidget *parent)
    : ConnectionEditPageNew(ConnectionEditPageNew::ConnectionType::VpnConnection, QString(), connUuid, parent)
{
}

ConnectionVpnEditPage::~ConnectionVpnEditPage()
{
}

void ConnectionVpnEditPage::initSettingsWidget()
{
    if (!m_connection) {
        qDebug() << "Connection of base class is invalid... maybe initSettingsWidgetByType should be used.";
        return;
    }

    if (!m_connectionSettings) {
        qDebug() << "ConnectionSettings of base class is invalid..." << m_connectionSettings;
        return;
    }

    const QString &serviceType = m_connectionSettings->setting(NetworkManager::Setting::SettingType::Vpn)
        .staticCast<NetworkManager::VpnSetting>()->serviceType();

    qDebug() << "using existing vpn connection, connection type:" << serviceType;

    if (serviceType == ServiceTypeL2TP) {
        initSettingsWidgetByType(ConnectionVpnEditPage::VpnType::L2TP);
    } else if (serviceType == ServiceTypePPTP) {
        initSettingsWidgetByType(ConnectionVpnEditPage::VpnType::PPTP);
    } else if (serviceType == ServiceTypeVPNC) {
        initSettingsWidgetByType(ConnectionVpnEditPage::VpnType::VPNC);
    } else if (serviceType == ServiceTypeOpenVPN) {
        initSettingsWidgetByType(ConnectionVpnEditPage::VpnType::OPENVPN);
    } else if (serviceType == ServiceTypeStrongSwan) {
        initSettingsWidgetByType(ConnectionVpnEditPage::VpnType::STRONGSWAN);
    } else if (serviceType == ServiceTypeOpenConnect) {
        initSettingsWidgetByType(ConnectionVpnEditPage::VpnType::OPENCONNECT);
    } else {
        qDebug() << "Unsupport vpn service type...";
        return;
    }
}

void ConnectionVpnEditPage::initSettingsWidgetByType(ConnectionVpnEditPage::VpnType vpnType)
{
    if (!m_connectionSettings) {
        qDebug() << "ConnectionSettings of base class is invalid..." << m_connectionSettings;
        return;
    }

    if (m_connectionSettings->id().isEmpty()) {
        resetConnectionIdByType(vpnType);
    }

    NetworkManager::VpnSetting::Ptr vpnSetting = m_connectionSettings->setting(
            NetworkManager::Setting::SettingType::Vpn).staticCast<NetworkManager::VpnSetting>();

    switch (vpnType) {
        case VpnType::L2TP: {
            vpnSetting->setServiceType(ServiceTypeL2TP);
            m_settingsWidget = new VpnL2tpSettings(m_connectionSettings, this);
            break;
        }
        case VpnType::PPTP: {
            vpnSetting->setServiceType(ServiceTypePPTP);
            m_settingsWidget = new VpnPPTPSettings(m_connectionSettings, this);
            break;
        }
        case VpnType::VPNC: {
            vpnSetting->setServiceType(ServiceTypeVPNC);
            m_settingsWidget = new VpnVPNCSettings(m_connectionSettings, this);
            break;
        }
        case VpnType::OPENVPN: {
            vpnSetting->setServiceType(ServiceTypeOpenVPN);
            m_settingsWidget = new VpnOpenVPNSettings(m_connectionSettings, this);
            break;
        }
        case VpnType::STRONGSWAN: {
            vpnSetting->setServiceType(ServiceTypeStrongSwan);
            m_settingsWidget = new VpnStrongSwanSettings(m_connectionSettings, this);
            break;
        }
        case VpnType::OPENCONNECT: {
            vpnSetting->setServiceType(ServiceTypeOpenConnect);
            m_settingsWidget = new VpnOpenConnectSettings(m_connectionSettings, this);
            break;
        }
        default:
            break;
    }

    connect(m_settingsWidget, &AbstractSettings::requestNextPage, this, &ConnectionVpnEditPage::requestNextPage);

    m_settingsLayout->addWidget(m_settingsWidget);
}

void ConnectionVpnEditPage::resetConnectionIdByType(ConnectionVpnEditPage::VpnType vpnType)
{
    QString connName;
    switch (vpnType) {
        case VpnType::L2TP: {
            connName = tr("VPN L2TP %1");
            break;
        }
        case VpnType::PPTP: {
            connName = tr("VPN PPTP %1");
            break;
        }
        case VpnType::VPNC: {
            connName = tr("VPN VPNC %1");
            break;
        }
        case VpnType::OPENVPN: {
            connName = tr("VPN OpenVPN %1");
            break;
        }
        case VpnType::STRONGSWAN: {
            connName = tr("VPN StrongSwan %1");
            break;
        }
        case VpnType::OPENCONNECT: {
            connName = tr("VPN OpenConnect %1");
            break;
        }
        default:
            break;
    }
    m_connectionSettings->setId(connName.arg(connectionSuffixNum(connName)));
}
