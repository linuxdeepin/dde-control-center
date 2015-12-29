#include <libdui/dbuttonlist.h>
#include <libdui/dconstants.h>

#include "genericlistitem.h"
#include "constants.h"

#include "addvpnpage.h"
#include "listwidgetcontainer.h"
#include "networkglobal.h"
#include "networkmainwidget.h"
#include "editlinecombobox.h"
#include "editlineinput.h"
#include "editlinemissingpackage.h"
#include "editlineswitchbutton.h"

AddVpnPage::AddVpnPage(const QString &dbusPath, QWidget *parent) :
    DVBoxWidget(parent),
    m_dbus(new DBusConnectionSession(dbusPath, this))
{
    init();
}

AddVpnPage::~AddVpnPage()
{
    m_dbus->Close();
}

void AddVpnPage::init()
{
    ListWidgetContainer *container_vpn_type = new ListWidgetContainer(tr("VPN Type"));
    DButtonList *list_vpn_type = new DButtonList;

    m_mapVpnNameToType["L2TP"] = ConnectionType::VpnL2tp;
    m_mapVpnNameToType["PPTP"] = ConnectionType::VpnPptp;
    m_mapVpnNameToType["OpenVPN"] = ConnectionType::VpnOpenvpn;
    m_mapVpnNameToType["OpenConnect"] = ConnectionType::VpnOpenconnect;
    m_mapVpnNameToType["VPNC"] = ConnectionType::VpnVpnc;

    list_vpn_type->addButtons(m_mapVpnNameToType.keys());
    list_vpn_type->checkButtonByIndex(0);
    list_vpn_type->setFixedSize(DCC::ModuleContentWidth, list_vpn_type->count() * DUI::BUTTON_HEIGHT);

    container_vpn_type->setBoxWidgetContentsMargins(0, 5, 0, 5);
    container_vpn_type->addWidget(list_vpn_type);
    container_vpn_type->setButtonsVisible(false);

    ListWidgetContainer *container_vpn_info = new ListWidgetContainer(tr("Basic Information"));

//    EditLineMissingPackage *missing_package = new EditLineMissingPackage("vs-vpn", "vk-vpn-missing-plugin",
//                                                                         m_dbus, tr("Need VPN Plugin"));

//    missing_package->setAlwaysUpdate(true);

    connect(list_vpn_type, &DButtonList::buttonChecked,
            this, [this](const QString &text) {
        //missing_package->setCurrentVpnName(text);
        m_dbus->SetKey("vs-vpn", "vk-vpn-type", "\"" + m_mapVpnNameToType[text] + "\"");
    });

    DVBoxWidget *boxWidget_vpn_info = new DVBoxWidget;

    //container_vpn_info->addWidget(missing_package);
    container_vpn_info->addWidget(boxWidget_vpn_info);
    container_vpn_info->setRightButtonText(tr("Add"));

    //connect(missing_package, &EditLineMissingPackage::visibleChanged, boxWidget_vpn_info, &DVBoxWidget::setHidden);

    boxWidget_vpn_info->layout()->setSpacing(5);
    boxWidget_vpn_info->addWidget(new EditLineInput("connection", "id", m_dbus, tr("Name")));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-l2tp", "gateway", m_dbus, tr("Gateway")));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-l2tp", "user", m_dbus, tr("Username")));
    boxWidget_vpn_info->addWidget(new EditLineComboBox("alias-vpn-l2tp", "password-flags",
                                                       m_dbus, tr("Ask for Pwd")));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-l2tp", "password", m_dbus,
                                                    tr("Password"), EditLineInput::Password));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openconnect", "gateway",
                                                    m_dbus, tr("Gateway")));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openconnect", "cacert", m_dbus,
                                                    tr("CA Cert"), EditLineInput::FileChooser));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openconnect", "proxy", m_dbus, tr("Proxy")));
    boxWidget_vpn_info->addWidget(new EditLineSwitchButton("alias-vpn-openconnect", "enable_csd_trojan",
                                                           m_dbus, tr("Allow Cisco Secure Desktop Trojan")));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openconnect", "csd_wrapper",
                                                    m_dbus, tr("CSD Script")));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openconnect", "usercert", m_dbus,
                                                    tr("User Cert"), EditLineInput::FileChooser));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openconnect", "userkey", m_dbus,
                                                    tr("Private Key"), EditLineInput::FileChooser));
    boxWidget_vpn_info->addWidget(new EditLineSwitchButton("alias-vpn-openconnect", "pem_passphrase_fsid",
                                                           m_dbus, tr("Use FSID for Key Passphrase")));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openvpn", "remote", m_dbus, tr("Gateway")));
    boxWidget_vpn_info->addWidget(new EditLineComboBox("alias-vpn-openvpn", "connection-type",
                                                       m_dbus, tr("Auth Type")));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openvpn", "username",
                                                    m_dbus, tr("Username")));
    boxWidget_vpn_info->addWidget(new EditLineComboBox("alias-vpn-openvpn", "password-flags",
                                                       m_dbus, tr("Ask for Pwd")));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openvpn", "password", m_dbus,
                                                    tr("Password"), EditLineInput::Password));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openvpn", "ca", m_dbus,
                                                    tr("CA Cert"), EditLineInput::FileChooser));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openvpn", "cert", m_dbus,
                                                    tr("User Cert"), EditLineInput::FileChooser));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openvpn", "key", m_dbus,
                                                    tr("Private Key"), EditLineInput::FileChooser));
    boxWidget_vpn_info->addWidget(new EditLineComboBox("alias-vpn-openvpn", "cert-pass-flags",
                                                       m_dbus, tr("Ask for Pwd")));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openvpn", "cert-pass", m_dbus,
                                                    tr("Private Pwd"), EditLineInput::Password));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openvpn", "static-key", m_dbus,
                                                    tr("Static Key"), EditLineInput::FileChooser));
    boxWidget_vpn_info->addWidget(new EditLineSwitchButton("alias-vpn-openvpn", "vk-static-key-direction",
                                                           m_dbus, tr("Customize Key Direction")));
    boxWidget_vpn_info->addWidget(new EditLineComboBox("alias-vpn-openvpn", "static-key-direction",
                                                       m_dbus, tr("Key Direction")));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openvpn", "remote-ip", m_dbus, tr("Remote IP")));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openvpn", "local-ip", m_dbus, tr("Local IP")));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-pptp", "gateway", m_dbus, tr("Gateway")));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-pptp", "user", m_dbus, tr("Username")));
    boxWidget_vpn_info->addWidget(new EditLineComboBox("alias-vpn-pptp", "password-flags",
                                                       m_dbus, tr("Ask for Pwd")));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-pptp", "password", m_dbus,
                                                    tr("Password"), EditLineInput::Password));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-vpnc", "IPSec gateway", m_dbus, tr("Gateway")));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-vpnc", "Xauth username", m_dbus, tr("Username")));
    boxWidget_vpn_info->addWidget(new EditLineComboBox("alias-vpn-vpnc", "Xauth password-flags",
                                                       m_dbus, tr("Ask for Pwd")));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-vpnc", "Xauth password", m_dbus,
                                                    tr("User Pwd"), EditLineInput::Password));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-vpnc", "IPSec ID", m_dbus, tr("Group Name")));
    boxWidget_vpn_info->addWidget(new EditLineComboBox("alias-vpn-vpnc", "IPSec secret-flags",
                                                       m_dbus, tr("Ask for Pwd")));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-vpnc", "IPSec secret", m_dbus,
                                                    tr("Group Pwd"), EditLineInput::Password));
    boxWidget_vpn_info->addWidget(new EditLineSwitchButton("alias-vpn-vpnc", "vk-hybrid-authmode",
                                                           m_dbus, tr("Use Hybrid Authentication")));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-vpnc", "CA-File", m_dbus,
                                                    tr("CA File"), EditLineInput::FileChooser));
    boxWidget_vpn_info->addWidget(new EditLineSwitchButton("alias-vpn-l2tp-ipsec", "ipsec-enabled",
                                                           m_dbus, tr("Enable IPsec")));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-l2tp-ipsec", "ipsec-group-name",
                                                    m_dbus, tr("Group Name")));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-l2tp-ipsec", "ipsec-gateway-id",
                                                    m_dbus, tr("Group ID")));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-l2tp-ipsec", "ipsec-psk", m_dbus,
                                                    tr("Pre-Shared Key"), EditLineInput::Password));
    boxWidget_vpn_info->addWidget(new EditLineSwitchButton("alias-vpn-l2tp-ppp", "vk-require-mppe",
                                                           m_dbus, tr("Use MPPE")));
    boxWidget_vpn_info->addWidget(new EditLineSwitchButton("alias-vpn-pptp-ppp", "vk-require-mppe",
                                                           m_dbus, tr("Use MPPE")));
    boxWidget_vpn_info->addWidget(new EditLineSwitchButton("connection", "vk-autoconnect",
                                                           m_dbus, tr("Automatically connect")));

    connect(container_vpn_info, &ListWidgetContainer::leftButtonClicked, this, [this] {
        m_dbus->Close();

        ScrollFrame *frame = DCCNetwork::parentNetworkMainWidget(this);

        if(frame)
            frame->popAllWidget();
    });

    connect(container_vpn_info, &ListWidgetContainer::rightButtonClicked, this, [this] {
        //checkKeysInPage()
        ScrollFrame *frame = DCCNetwork::parentNetworkMainWidget(this);

        if (m_dbus->Save()) {
            if(frame)
                frame->popAllWidget();
        } else {
            for(NetworkBaseEditLine *line : findChildren<NetworkBaseEditLine*>()) {
                line->checkKey();
            }
        }
    });

    addWidget(container_vpn_type);
    addWidget(container_vpn_info);
}
