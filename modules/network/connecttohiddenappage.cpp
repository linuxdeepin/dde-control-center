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

#include "constants.h"

#include "connecttohiddenappage.h"
#include "networkbaseeditline.h"
#include "editlinecombobox.h"
#include "editlineinput.h"

DUI_USE_NAMESPACE

ConnectToHiddenApPage::ConnectToHiddenApPage(DBusConnectionSession *dbus, QWidget *parent) :
    DVBoxWidget(parent),
    m_dbus(dbus)
{
    D_THEME_INIT_WIDGET(ConnectToHiddenApPage)

    init();
}

void ConnectToHiddenApPage::init()
{
    DHeaderLine *title = new DHeaderLine;

    title->setTitle("Connect to hidden access point");
    title->setFixedWidth(DCC::ModuleContentWidth);

    QHBoxLayout *button_layout = new QHBoxLayout;
    DTextButton *button_cancel = new DTextButton(tr("Cancel"));
    DTextButton *button_connect = new DTextButton(tr("Connect"));

    button_layout->setMargin(0);
    button_layout->setSpacing(5);
    button_layout->addStretch(1);
    button_layout->addWidget(button_cancel);
    button_layout->addWidget(button_connect);
    button_layout->addSpacing(15);

    DVBoxWidget *main_widget = new DVBoxWidget;

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

    main_widget->setObjectName("MainVBoxWidget");
    main_widget->setStyleSheet(styleSheet());
    main_widget->layout()->setContentsMargins(15, 5, 15, 5);
    main_widget->layout()->setSpacing(5);
    main_widget->layout()->addWidget(line80211WirelessSsid);
    main_widget->layout()->addWidget(line80211WirelessSecurityVkKeyMgmt);
    main_widget->layout()->addWidget(new EditLineInput("802-11-wireless-security",
                                                       "wep-key0", m_dbus,
                                                       tr("Key"), BaseLineEditType::Password));
    main_widget->layout()->addWidget(new EditLineInput("802-11-wireless-security",
                                                       "psk", m_dbus,
                                                       tr("Password"), BaseLineEditType::Password));
    main_widget->layout()->addWidget(new EditLineComboBox("802-1x", "vk-eap",
                                                          m_dbus, tr("EAP Auth")));
    main_widget->layout()->addWidget(new EditLineInput("802-1x", "identity",
                                                       m_dbus, tr("Identity")));
    main_widget->layout()->addWidget(new EditLineInput("802-1x", "anonymous-identity",
                                                       m_dbus, tr("Anonymous ID")));
    main_widget->layout()->addWidget(new EditLineInput("802-1x", "vk-ca-cert", m_dbus,
                                                       tr("CA Cert"), BaseLineEditType::FileChooser));
    main_widget->layout()->addWidget(new EditLineInput("802-1x", "vk-client-cert", m_dbus,
                                                       tr("User Cert"), BaseLineEditType::FileChooser));
    main_widget->layout()->addWidget(new EditLineComboBox("802-1x", "phase1-fast-provisioning",
                                                          m_dbus, tr("Provisioning")));
    main_widget->layout()->addWidget(new EditLineInput("802-1x", "vk-pac-file", m_dbus,
                                                       tr("PAC file"), BaseLineEditType::FileChooser));
    main_widget->layout()->addWidget(new EditLineComboBox("802-1x", "phase1-peapver",
                                                          m_dbus, tr("PEAP Version")));
    main_widget->layout()->addWidget(line8021xPhase2Auth);
    main_widget->layout()->addWidget(new EditLineInput("802-1x", "vk-private-key", m_dbus,
                                                       tr("Private Key"), BaseLineEditType::FileChooser));
    main_widget->layout()->addWidget(line8021xPasswordFlags);
    main_widget->layout()->addWidget(new EditLineInput("802-1x", "password", m_dbus,
                                                       tr("Password"), BaseLineEditType::Password));
    main_widget->layout()->addWidget(new EditLineComboBox("802-1x", "private-key-password-flags",
                                                          m_dbus, tr("Ask for Pwd")));
    main_widget->layout()->addWidget(new EditLineInput("802-1x", "private-key-password",
                                                       m_dbus, tr("Private Pwd"),
                                                       BaseLineEditType::Password));

    if(line80211WirelessSecurityVkKeyMgmt->dbusKey() == "none") {
        line80211WirelessSecurityVkKeyMgmt->setDBusKey(QJsonValue("wpa-psk"));
    }

    addWidget(title);
    addWidget(new DSeparatorHorizontal);
    addWidget(main_widget);
    addWidget(new DSeparatorHorizontal);
    layout()->addSpacing(10);
    layout()->addLayout(button_layout);

    connect(button_cancel, &DTextButton::clicked, this, &ConnectToHiddenApPage::cancel);
    connect(button_connect, &DTextButton::clicked, this, [this, line80211WirelessSsid] {
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
    connect(button_cancel, &DTextButton::clicked, m_dbus, &DBusConnectionSession::Close);
}

