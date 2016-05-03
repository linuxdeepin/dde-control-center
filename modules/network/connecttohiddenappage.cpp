/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QFormLayout>
#include <QSpinBox>
#include <QDebug>

#include <dheaderline.h>
#include <dtextbutton.h>
#include <dseparatorhorizontal.h>
#include <dlineedit.h>
#include <dcombobox.h>
#include <dpasswordedit.h>
#include <dconstants.h>
#include <dthememanager.h>
#include <dwidget_global.h>

#include "constants.h"

#include "connecttohiddenappage.h"
#include "networkbaseeditline.h"
#include "editlinecombobox.h"
#include "editlineinput.h"

DWIDGET_USE_NAMESPACE

ConnectToHiddenApPage::ConnectToHiddenApPage(DBusConnectionSession *dbus, QWidget *parent) :
    ListWidgetContainer(tr("Connect to hidden access point"), parent),
    m_dbus(dbus)
{
    init();
}

ConnectToHiddenApPage::~ConnectToHiddenApPage()
{
    m_dbus->Close();
}

void ConnectToHiddenApPage::init()
{
    NetworkBaseEditLine *line80211WirelessSsid = new EditLineInput("802-11-wireless", "ssid",
                                                                   m_dbus);
    NetworkBaseEditLine *line80211WirelessSecurityVkKeyMgmt = new EditLineComboBox("802-11-wireless-security",
                                                                                   "vk-key-mgmt",
                                                                                   m_dbus);
    NetworkBaseEditLine *line8021xPhase2Auth = new EditLineComboBox("802-1x", "phase2-auth",
                                                                    m_dbus);
    NetworkBaseEditLine *line8021xPasswordFlags = new EditLineInput("802-1x", "password-flags",
                                                                    m_dbus);

    line8021xPhase2Auth->setAlwaysUpdate(true);
    line8021xPasswordFlags->setAlwaysUpdate(true);

    addWidget(line80211WirelessSsid);
    addWidget(line80211WirelessSecurityVkKeyMgmt);
    addWidget(new EditLineInput("802-11-wireless-security",
                                "wep-key0", m_dbus,
                                EditLineInput::Password));
    addWidget(new EditLineInput("802-11-wireless-security",
                                "psk", m_dbus,
                                EditLineInput::Password));
    addWidget(new EditLineComboBox("802-1x", "vk-eap", m_dbus));
    addWidget(new EditLineInput("802-1x", "identity", m_dbus));
    addWidget(new EditLineInput("802-1x", "anonymous-identity", m_dbus));
    addWidget(new EditLineInput("802-1x", "vk-ca-cert", m_dbus,
                                EditLineInput::FileChooser));
    addWidget(new EditLineInput("802-1x", "vk-client-cert", m_dbus,
                                EditLineInput::FileChooser));
    addWidget(new EditLineComboBox("802-1x", "phase1-fast-provisioning", m_dbus));
    addWidget(new EditLineInput("802-1x", "vk-pac-file", m_dbus,
                                EditLineInput::FileChooser));
    addWidget(new EditLineComboBox("802-1x", "phase1-peapver", m_dbus));
    addWidget(line8021xPhase2Auth);
    addWidget(new EditLineInput("802-1x", "vk-private-key", m_dbus,
                                EditLineInput::FileChooser));
    addWidget(line8021xPasswordFlags);
    addWidget(new EditLineInput("802-1x", "password", m_dbus,
                                EditLineInput::Password));
    addWidget(new EditLineComboBox("802-1x", "private-key-password-flags", m_dbus));
    addWidget(new EditLineInput("802-1x", "private-key-password",
                                m_dbus, EditLineInput::Password));

    if(line80211WirelessSecurityVkKeyMgmt->dbusKey() == "none") {
        line80211WirelessSecurityVkKeyMgmt->setDBusKey(QJsonValue("wpa-psk"));
    }

    connect(this, &ConnectToHiddenApPage::leftButtonClicked,
            this, &ConnectToHiddenApPage::cancel);
    connect(this, &ConnectToHiddenApPage::rightButtonClicked,
            this, [this, line80211WirelessSsid] {
        m_dbus->SetKey("connection", "id",
                       m_dbus->GetKey(line80211WirelessSsid->section(),
                       line80211WirelessSsid->key()));
        //checkKeysInPage()
        if (m_dbus->Save()) {
            emit confirm();
        } else {
            for(NetworkBaseEditLine *line : findChildren<NetworkBaseEditLine*>()) {
                line->checkKey();
            }
        }
    });
    connect(this, &ConnectToHiddenApPage::leftButtonClicked,
            m_dbus, &DBusConnectionSession::Close);
}

