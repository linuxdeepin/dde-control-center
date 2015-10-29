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

#include "connecttohiddenapwidget.h"
#include "networkbaseeditline.h"

DUI_USE_NAMESPACE

ConnectToHiddenApWidget::ConnectToHiddenApWidget(DBusConnectionSession *dbus, QWidget *parent) :
    DVBoxWidget(parent),
    m_dbusConnectionSession(dbus)
{
    D_THEME_INIT_WIDGET(ConnectToHiddenApWidget)

    init();
}

NetworkBaseEditLine *ConnectToHiddenApWidget::getLineEdit(const QString &section, const QString &key,
                                 const QString &title, bool isPasswordEdit)
{
    QWidget *line_edit;
    if(isPasswordEdit)
        line_edit = new DPasswordEdit;
    else
        line_edit = new DLineEdit;

    NetworkBaseEditLine *base_edit = new NetworkBaseEditLine(section, key,
                                                             m_dbusConnectionSession,
                                                             title, line_edit);

    return base_edit;
}

NetworkBaseEditLine *ConnectToHiddenApWidget::getCombobox(const QString &section,
                                 const QString &key, const QString &title)
{
    DComboBox *combo_box = new DComboBox;

    NetworkBaseEditLine *base_edit = new NetworkBaseEditLine(section, key,
                                                             m_dbusConnectionSession,
                                                             title, combo_box);

    auto updateComboData = [base_edit, combo_box]{
        const QStringList &list = base_edit->getAvailableValuesText();

        combo_box->clear();
        combo_box->addItems(list);

        int index = base_edit->getAvailableValuesIndex();

        if(index >= 0)
            combo_box->setCurrentIndex(index);
    };

    connect(base_edit, &NetworkBaseEditLine::widgetShown, base_edit, updateComboData);
    connect(base_edit, &NetworkBaseEditLine::cacheValueChanged, base_edit, updateComboData);
    connect(combo_box, static_cast<void (DComboBox::*)(int)>(&DComboBox::activated),
            base_edit, [base_edit, combo_box](int index) {
        const QList<QJsonValue> &list = base_edit->getAvailableValuesValue();

        if(index >= 0) {
            base_edit->setDBusKey(list[index]);
        }
    });

    return base_edit;
}

NetworkBaseEditLine *ConnectToHiddenApWidget::getFileChooser(const QString &section,
                                    const QString &key, const QString &title)
{
    NetworkBaseEditLine *base_edit = new NetworkBaseEditLine(section, key,
                                                             m_dbusConnectionSession,
                                                             title, new DLineEdit);

    return base_edit;
}

void ConnectToHiddenApWidget::init()
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

    NetworkBaseEditLine *line80211WirelessSecurityVkKeyMgmt
            = getCombobox("802-11-wireless-security", "vk-key-mgmt", tr("Security"));
    NetworkBaseEditLine *line8021xPhase2Auth = getCombobox("802-1x", "phase2-auth", tr("Inner Auth"));
    NetworkBaseEditLine *line8021xPasswordFlags = getLineEdit("802-1x", "password-flags", tr("Ask for Pwd"));

    line8021xPhase2Auth->setAlwaysUpdate(true);
    line8021xPasswordFlags->setAlwaysUpdate(true);

    main_widget->setObjectName("MainVBoxWidget");
    main_widget->setStyleSheet(styleSheet());
    main_widget->layout()->setContentsMargins(15, 5, 15, 5);
    main_widget->layout()->setSpacing(5);
    main_widget->layout()->addWidget(getLineEdit("802-11-wireless", "ssid", tr("SSID")));
    main_widget->layout()->addWidget(line80211WirelessSecurityVkKeyMgmt);
    main_widget->layout()->addWidget(getLineEdit("802-11-wireless-security",
                                                 "wep-key0", tr("Key"), true));
    main_widget->layout()->addWidget(getLineEdit("802-11-wireless-security",
                                                 "psk", tr("Password"), true));
    main_widget->layout()->addWidget(getCombobox("802-1x", "vk-eap", tr("EAP Auth")));
    main_widget->layout()->addWidget(getLineEdit("802-1x", "identity", tr("Identity")));
    main_widget->layout()->addWidget(getLineEdit("802-1x", "anonymous-identity", tr("Anonymous ID")));
    main_widget->layout()->addWidget(getFileChooser("802-1x", "vk-ca-cert", tr("CA Cert")));
    main_widget->layout()->addWidget(getFileChooser("802-1x", "vk-client-cert", tr("User Cert")));
    main_widget->layout()->addWidget(getCombobox("802-1x", "phase1-fast-provisioning", tr("Provisioning")));
    main_widget->layout()->addWidget(getFileChooser("802-1x", "vk-pac-file", tr("PAC file")));
    main_widget->layout()->addWidget(getCombobox("802-1x", "phase1-peapver", tr("PEAP Version")));
    main_widget->layout()->addWidget(line8021xPhase2Auth);
    main_widget->layout()->addWidget(getFileChooser("802-1x", "vk-private-key", tr("Private Key")));
    main_widget->layout()->addWidget(line8021xPasswordFlags);
    main_widget->layout()->addWidget(getLineEdit("802-1x", "password", tr("Password"), true));
    main_widget->layout()->addWidget(getCombobox("802-1x", "private-key-password-flags", tr("Ask for Pwd")));
    main_widget->layout()->addWidget(getLineEdit("802-1x", "private-key-password", tr("Private Pwd"), true));

    if(line80211WirelessSecurityVkKeyMgmt->dbusKey() == "none") {
        line80211WirelessSecurityVkKeyMgmt->setDBusKey("wpa-psk");
    }

    addWidget(title);
    addWidget(new DSeparatorHorizontal);
    addWidget(main_widget);
    addWidget(new DSeparatorHorizontal);
    layout()->addSpacing(10);
    layout()->addLayout(button_layout);

    connect(button_cancel, &DTextButton::clicked, this, &ConnectToHiddenApWidget::cancel);
    connect(button_connect, &DTextButton::clicked, this, &ConnectToHiddenApWidget::confirm);
    connect(button_cancel, &DTextButton::clicked, m_dbusConnectionSession, &DBusConnectionSession::Close);
}

