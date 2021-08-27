/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
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

#include "vpnadvvpncsection.h"

#include <dspinbox.h>

#include <QComboBox>

using namespace dcc::widgets;
using namespace NetworkManager;

VpnAdvVPNCSection::VpnAdvVPNCSection(VpnSetting::Ptr vpnSetting, QFrame *parent)
    : AbstractSection(tr("VPN Advanced"), parent)
    , m_vpnSetting(vpnSetting)
    , m_dataMap(vpnSetting->data())
    , m_domain(new LineEditWidget(this))
    , m_vendorChooser(new ComboxWidget(this))
    , m_version(new LineEditWidget(this))
    , m_encryptionChooser(new ComboxWidget(this))
    , m_natTravModeChooser(new ComboxWidget(this))
    , m_ikeDHGroupChooser(new ComboxWidget(this))
    , m_forwordSecrecyChooser(new ComboxWidget(this))
    , m_localPort(new SpinBoxWidget(this))
    , m_disableDPD(new SwitchWidget(this))
    , m_currentVendor("cisco")
    , m_currentEncryption("secure")
    , m_currentNatTravMod("natt")
    , m_currentIkeDHGroup("dh2")
    , m_currentForwordSecrecy("server")
{
    initStrMaps();

    // init chooser current values
    for (auto it = VendorStrMap.cbegin(); it != VendorStrMap.cend(); ++it) {
        if (it->second == m_dataMap.value("Vendor")) {
            m_currentVendor = it->second;
            break;
        }
    }

    if (m_dataMap.value("Enable no encryption") == "yes")
        m_currentEncryption = "none";
    else if (m_dataMap.value("Enable Single DES") == "yes")
        m_currentEncryption = "weak";
    else
        m_currentEncryption = "secure";

    for (auto it = NATTravModeStrMap.cbegin(); it != NATTravModeStrMap.cend(); ++it) {
        if (it->second == m_dataMap.value("NAT Traversal Mode")) {
            m_currentNatTravMod = it->second;
            break;
        }
    }

    for (auto it = NATTravModeStrMap.cbegin(); it != NATTravModeStrMap.cend(); ++it) {
        if (it->second == m_dataMap.value("IKE DH Group")) {
            m_currentIkeDHGroup = it->second;
            break;
        }
    }

    for (auto it = ForwardSecrecyStrMap.cbegin(); it != ForwardSecrecyStrMap.cend(); ++it) {
        if (it->second == m_dataMap.value("Perfect Forward Secrecy")) {
            m_currentForwordSecrecy = it->second;
            break;
        }
    }

    initUI();
    initConnection();
}

VpnAdvVPNCSection::~VpnAdvVPNCSection()
{
}

bool VpnAdvVPNCSection::allInputValid()
{
    return true;
}

void VpnAdvVPNCSection::saveSettings()
{
    // retrieve the data map
    m_dataMap = m_vpnSetting->data();

    m_dataMap.insert("Domain", m_domain->text());
    m_dataMap.insert("Vendor", m_currentVendor);
    m_dataMap.insert("Application Version", m_version->text());
    if (m_currentEncryption == "none") {
        m_dataMap.remove("Enable Single DES");
        m_dataMap.insert("Enable no encryption", "yes");
    } else if (m_currentEncryption == "weak") {
        m_dataMap.remove("Enable no encryption");
        m_dataMap.insert("Enable Single DES", "yes");
    } else if (m_currentEncryption == "secure") {
        m_dataMap.remove("Enable no encryption");
        m_dataMap.remove("Enable Single DES");
    }

    m_dataMap.insert("NAT Traversal Mode", m_currentNatTravMod);
    m_dataMap.insert("IKE DH Group", m_currentIkeDHGroup);
    m_dataMap.insert("Perfect Forward Secrecy", m_currentForwordSecrecy);
    m_dataMap.insert("Local Port", QString::number(m_localPort->spinBox()->value()));
    if (m_disableDPD->checked())
        m_dataMap.insert("DPD idle timeout (our side)", "0");
    else
        m_dataMap.remove("DPD idle timeout (our side)");

    m_vpnSetting->setData(m_dataMap);
    m_vpnSetting->setInitialized(true);
}

void VpnAdvVPNCSection::initStrMaps()
{
    VendorStrMap = {
        { tr("Cisco (default)"), "cisco" },
        { tr("Netscreen"), "netscreen" },
    };
    EncryptionStrMap = {
        { tr("Secure (default)"), "secure" },
        { tr("Weak"), "weak" },
        { tr("None"), "none" },
    };
    NATTravModeStrMap = {
        { tr("NAT-T When Available (default)"), "natt" },
        { tr("NAT-T Always"), "force-natt" },
        { tr("Cisco UDP"), "cisco-udp" },
        { tr("Disabled"), "none" },
    };
    IKEDHGroupStrMap = {
        { tr("DH Group 1"), "dh1" },
        { tr("DH Group 2 (default)"), "dh2" },
        { tr("DH Group 5"), "dh5" },
    };
    ForwardSecrecyStrMap = {
        { tr("Server (default)"), "server" },
        { tr("None"), "nopfs" },
        { tr("DH Group 1"), "dh1" },
        { tr("DH Group 2"), "dh2" },
        { tr("DH Group 5"), "dh5" },
    };
}

void VpnAdvVPNCSection::initUI()
{
    m_domain->setTitle(tr("Domain"));
    m_domain->setText(m_dataMap.value("Domain"));

    m_vendorChooser->setTitle(tr("Vendor"));
    QString curVendorOption = VendorStrMap.at(0).first;
    for (auto it = VendorStrMap.cbegin(); it != VendorStrMap.cend(); ++it) {
        m_vendorChooser->comboBox()->addItem(it->first, it->second);
        if (m_currentVendor == it->second)
            curVendorOption = it->first;
    }

    m_vendorChooser->setCurrentText(curVendorOption);

    m_version->setTitle(tr("Version"));
    m_version->setText(m_dataMap.value("Application Version"));

    m_encryptionChooser->setTitle(tr("Encryption"));
    QString curEncryptionOption = EncryptionStrMap.at(0).first;
    for (auto it = EncryptionStrMap.cbegin(); it != EncryptionStrMap.cend(); ++it) {
        m_encryptionChooser->comboBox()->addItem(it->first, it->second);
        if (it->second == m_currentEncryption)
            curEncryptionOption = it->first;
    }

    m_encryptionChooser->setCurrentText(curEncryptionOption);

    m_natTravModeChooser->setTitle(tr("NAT Traversal Mode"));
    QString curNatTravOption = NATTravModeStrMap.at(0).first;
    for (auto it = NATTravModeStrMap.cbegin(); it != NATTravModeStrMap.cend(); ++it) {
        m_natTravModeChooser->comboBox()->addItem(it->first, it->second);
        if (it->second == m_currentNatTravMod)
            curNatTravOption = it->first;
    }

    m_natTravModeChooser->setCurrentText(curNatTravOption);

    m_ikeDHGroupChooser->setTitle(tr("IKE DH Group"));
    QString curIKEOption = IKEDHGroupStrMap.at(0).first;
    for (auto it = IKEDHGroupStrMap.cbegin(); it != IKEDHGroupStrMap.cend(); ++it) {
        m_ikeDHGroupChooser->comboBox()->addItem(it->first, it->second);
        if (it->second == m_currentIkeDHGroup)
            curIKEOption = it->first;
    }

    m_ikeDHGroupChooser->setCurrentText(curIKEOption);

    m_forwordSecrecyChooser->setTitle(tr("Forward Secrecy"));
    QString curForwardSecrecyOption = ForwardSecrecyStrMap.at(0).first;
    for (auto it = IKEDHGroupStrMap.cbegin(); it != IKEDHGroupStrMap.cend(); ++it) {
        m_forwordSecrecyChooser->comboBox()->addItem(it->first, it->second);
        if (it->second == m_currentForwordSecrecy)
            curForwardSecrecyOption = it->first;
    }

    m_forwordSecrecyChooser->setCurrentText(curForwardSecrecyOption);

    m_localPort->setTitle(tr("Local Port"));
    m_localPort->spinBox()->setMinimum(0);
    m_localPort->spinBox()->setMaximum(65535);
    m_localPort->spinBox()->setValue(m_dataMap.value("Local Port").toInt());

    m_disableDPD->setTitle(tr("Disable Dead Peer Detection"));
    m_disableDPD->setChecked(m_dataMap.value("DPD idle timeout (our side)") == "0");

    appendItem(m_domain);
    appendItem(m_vendorChooser);
    appendItem(m_version);
    appendItem(m_encryptionChooser);
    appendItem(m_natTravModeChooser);
    appendItem(m_ikeDHGroupChooser);
    appendItem(m_forwordSecrecyChooser);
    appendItem(m_localPort);
    appendItem(m_disableDPD);

    m_domain->textEdit()->installEventFilter(this);
    m_version->textEdit()->installEventFilter(this);
    m_localPort->spinBox()->installEventFilter(this);
}

void VpnAdvVPNCSection::initConnection()
{
    connect(m_vendorChooser, &ComboxWidget::dataChanged, this, [ = ](const QVariant &data) {
        m_currentVendor = data.value<QString>();
    });

    connect(m_encryptionChooser, &ComboxWidget::dataChanged, this, [ = ](const QVariant &data) {
        m_currentEncryption = data.value<QString>();
    });

    connect(m_natTravModeChooser, &ComboxWidget::dataChanged, this, [ = ](const QVariant &data) {
        m_currentNatTravMod = data.value<QString>();
    });

    connect(m_ikeDHGroupChooser, &ComboxWidget::dataChanged, this, [ = ](const QVariant &data) {
        m_currentIkeDHGroup = data.value<QString>();
    });

    connect(m_forwordSecrecyChooser, &ComboxWidget::dataChanged, this, [ = ](const QVariant &data) {
        m_currentForwordSecrecy = data.value<QString>();
    });

    connect(m_vendorChooser, &ComboxWidget::onIndexChanged, this, &VpnAdvVPNCSection::editClicked);
    connect(m_encryptionChooser, &ComboxWidget::onIndexChanged, this, &VpnAdvVPNCSection::editClicked);
    connect(m_natTravModeChooser, &ComboxWidget::onIndexChanged, this, &VpnAdvVPNCSection::editClicked);
    connect(m_ikeDHGroupChooser, &ComboxWidget::onIndexChanged, this, &VpnAdvVPNCSection::editClicked);
    connect(m_forwordSecrecyChooser, &ComboxWidget::onIndexChanged, this, &VpnAdvVPNCSection::editClicked);
    connect(m_disableDPD, &SwitchWidget::checkedChanged, this, &VpnAdvVPNCSection::editClicked);
    connect(m_localPort->spinBox(), static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &VpnAdvVPNCSection::editClicked);
}

bool VpnAdvVPNCSection::eventFilter(QObject *watched, QEvent *event)
{
    // 实现鼠标点击编辑框，确定按钮激活，统一网络模块处理，捕捉FocusIn消息
    if (event->type() == QEvent::FocusIn) {
        if (dynamic_cast<QLineEdit *>(watched) || dynamic_cast<QSpinBox *>(watched))
            Q_EMIT editClicked();
    }

    return QWidget::eventFilter(watched, event);
}
