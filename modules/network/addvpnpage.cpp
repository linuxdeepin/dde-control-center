/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

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
    m_mapVpnNameToType["StrongSwan"] = ConnectionType::VpnStrongSwan;

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
    boxWidget_vpn_info->addWidget(new EditLineInput("connection", "id", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-l2tp", "gateway", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-l2tp", "user", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineComboBox("alias-vpn-l2tp", "password-flags", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-l2tp", "password", m_dbus,
                                                    EditLineInput::Password));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openconnect", "gateway", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openconnect", "cacert", m_dbus,
                                                    EditLineInput::FileChooser));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openconnect", "proxy", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineSwitchButton("alias-vpn-openconnect",
                                                           "enable_csd_trojan", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openconnect",
                                                    "csd_wrapper", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openconnect", "usercert", m_dbus,
                                                    EditLineInput::FileChooser));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openconnect", "userkey", m_dbus,
                                                    EditLineInput::FileChooser));
    boxWidget_vpn_info->addWidget(new EditLineSwitchButton("alias-vpn-openconnect",
                                                           "pem_passphrase_fsid", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openvpn", "remote", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineComboBox("alias-vpn-openvpn",
                                                       "connection-type", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openvpn", "username", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineComboBox("alias-vpn-openvpn",
                                                       "password-flags", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openvpn", "password", m_dbus,
                                                    EditLineInput::Password));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openvpn", "ca", m_dbus,
                                                    EditLineInput::FileChooser));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openvpn", "cert", m_dbus,
                                                    EditLineInput::FileChooser));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openvpn", "key", m_dbus,
                                                    EditLineInput::FileChooser));
    boxWidget_vpn_info->addWidget(new EditLineComboBox("alias-vpn-openvpn",
                                                       "cert-pass-flags", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openvpn", "cert-pass", m_dbus,
                                                    EditLineInput::Password));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openvpn", "static-key", m_dbus,
                                                    EditLineInput::FileChooser));
    boxWidget_vpn_info->addWidget(new EditLineSwitchButton("alias-vpn-openvpn",
                                                           "vk-static-key-direction", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineComboBox("alias-vpn-openvpn",
                                                       "static-key-direction", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openvpn", "remote-ip", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-openvpn", "local-ip", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-pptp", "gateway", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-pptp", "user", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineComboBox("alias-vpn-pptp",
                                                       "password-flags", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-pptp", "password", m_dbus,
                                                    EditLineInput::Password));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-vpnc", "IPSec gateway", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-vpnc", "Xauth username", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineComboBox("alias-vpn-vpnc",
                                                       "Xauth password-flags", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-vpnc", "Xauth password", m_dbus,
                                                    EditLineInput::Password));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-vpnc", "IPSec ID", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineComboBox("alias-vpn-vpnc",
                                                       "IPSec secret-flags", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-vpnc", "IPSec secret", m_dbus,
                                                    EditLineInput::Password));
    boxWidget_vpn_info->addWidget(new EditLineSwitchButton("alias-vpn-vpnc",
                                                           "vk-hybrid-authmode", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-vpnc", "CA-File", m_dbus,
                                                    EditLineInput::FileChooser));
    boxWidget_vpn_info->addWidget(new EditLineSwitchButton("alias-vpn-l2tp-ipsec",
                                                           "ipsec-enabled", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-l2tp-ipsec",
                                                    "ipsec-group-name", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-l2tp-ipsec",
                                                    "ipsec-gateway-id", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-l2tp-ipsec", "ipsec-psk", m_dbus,
                                                    EditLineInput::Password));
    boxWidget_vpn_info->addWidget(new EditLineSwitchButton("alias-vpn-l2tp-ppp",
                                                           "vk-require-mppe", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineSwitchButton("alias-vpn-pptp-ppp",
                                                           "vk-require-mppe", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-strongswan", "address", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-strongswan", "certificate",
                                                    m_dbus, EditLineInput::FileChooser));
    boxWidget_vpn_info->addWidget(new EditLineComboBox("alias-vpn-strongswan", "method", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-strongswan", "user",
                                                    m_dbus, EditLineInput::Normal, true));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-strongswan", "usercert", m_dbus,
                                                    EditLineInput::FileChooser, true));
    boxWidget_vpn_info->addWidget(new EditLineInput("alias-vpn-strongswan", "userkey", m_dbus,
                                                    EditLineInput::FileChooser, true));
    boxWidget_vpn_info->addWidget(new EditLineSwitchButton("alias-vpn-strongswan", "virtual", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineSwitchButton("alias-vpn-strongswan", "encap", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineSwitchButton("alias-vpn-strongswan", "ipcomp", m_dbus));
    boxWidget_vpn_info->addWidget(new EditLineSwitchButton("connection", "vk-autoconnect", m_dbus));

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
