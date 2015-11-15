#include <QFormLayout>
#include <QSpinBox>
#include <QDebug>

#include <libdui/dheaderline.h>
#include <libdui/dtextbutton.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/dlineedit.h>
#include <libdui/dcombobox.h>
#include <libdui/dpasswordedit.h>
#include <libdui/dconstants.h>
#include <libdui/dthememanager.h>
#include <libdui/libdui_global.h>

#include "constants.h"

#include "connecttohiddenappage.h"
#include "networkbaseeditline.h"
#include "editlinecombobox.h"
#include "editlineinput.h"

DUI_USE_NAMESPACE

ConnectToHiddenApPage::ConnectToHiddenApPage(DBusConnectionSession *dbus, QWidget *parent) :
    ListWidgetContainer(tr("Connect to hidden access point"), parent),
    m_dbus(dbus)
{
    init();
}

void ConnectToHiddenApPage::init()
{
    NetworkBaseEditLine *line80211WirelessSsid = new EditLineInput("802-11-wireless", "ssid",
                                                                   m_dbus, tr("SSID"));
    NetworkBaseEditLine *line80211WirelessSecurityVkKeyMgmt = new EditLineComboBox("802-11-wireless-security",
                                                                                   "vk-key-mgmt",
                                                                                   m_dbus,
                                                                                   tr("Security"));
    NetworkBaseEditLine *line8021xPhase2Auth = new EditLineComboBox("802-1x", "phase2-auth",
                                                                    m_dbus, tr("Inner Auth"));
    NetworkBaseEditLine *line8021xPasswordFlags = new EditLineInput("802-1x", "password-flags",
                                                                    m_dbus, tr("Ask for Pwd"));

    line8021xPhase2Auth->setAlwaysUpdate(true);
    line8021xPasswordFlags->setAlwaysUpdate(true);

    addWidget(line80211WirelessSsid);
    addWidget(line80211WirelessSecurityVkKeyMgmt);
    addWidget(new EditLineInput("802-11-wireless-security",
                                                       "wep-key0", m_dbus,
                                                       tr("Key"), EditLineInput::Password));
    addWidget(new EditLineInput("802-11-wireless-security",
                                                       "psk", m_dbus,
                                                       tr("Password"), EditLineInput::Password));
    addWidget(new EditLineComboBox("802-1x", "vk-eap",
                                                          m_dbus, tr("EAP Auth")));
    addWidget(new EditLineInput("802-1x", "identity",
                                                       m_dbus, tr("Identity")));
    addWidget(new EditLineInput("802-1x", "anonymous-identity",
                                                       m_dbus, tr("Anonymous ID")));
    addWidget(new EditLineInput("802-1x", "vk-ca-cert", m_dbus,
                                                       tr("CA Cert"), EditLineInput::FileChooser));
    addWidget(new EditLineInput("802-1x", "vk-client-cert", m_dbus,
                                                       tr("User Cert"), EditLineInput::FileChooser));
    addWidget(new EditLineComboBox("802-1x", "phase1-fast-provisioning",
                                                          m_dbus, tr("Provisioning")));
    addWidget(new EditLineInput("802-1x", "vk-pac-file", m_dbus,
                                                       tr("PAC file"), EditLineInput::FileChooser));
    addWidget(new EditLineComboBox("802-1x", "phase1-peapver",
                                                          m_dbus, tr("PEAP Version")));
    addWidget(line8021xPhase2Auth);
    addWidget(new EditLineInput("802-1x", "vk-private-key", m_dbus,
                                                       tr("Private Key"), EditLineInput::FileChooser));
    addWidget(line8021xPasswordFlags);
    addWidget(new EditLineInput("802-1x", "password", m_dbus,
                                                       tr("Password"), EditLineInput::Password));
    addWidget(new EditLineComboBox("802-1x", "private-key-password-flags",
                                                          m_dbus, tr("Ask for Pwd")));
    addWidget(new EditLineInput("802-1x", "private-key-password",
                                                       m_dbus, tr("Private Pwd"),
                                                       EditLineInput::Password));

    if(line80211WirelessSecurityVkKeyMgmt->dbusKey() == "none") {
        line80211WirelessSecurityVkKeyMgmt->setDBusKey(QJsonValue("wpa-psk"));
    }

    connect(this, &ConnectToHiddenApPage::leftButtonClicked, this, &ConnectToHiddenApPage::cancel);
    connect(this, &ConnectToHiddenApPage::rightButtonClicked, this, [this, line80211WirelessSsid] {
        m_dbus->SetKey("connection", "id",
                                        m_dbus->GetKey(line80211WirelessSsid->section(),
                                                                        line80211WirelessSsid->key()));
        //checkKeysInPage()
        if (m_dbus->Save()) {
            emit confirm();
        } else {
            qDebug() << m_dbus->errors();
        }
    });
    connect(this, &ConnectToHiddenApPage::leftButtonClicked, m_dbus, &DBusConnectionSession::Close);
}

