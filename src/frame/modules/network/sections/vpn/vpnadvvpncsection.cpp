/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
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

using namespace dcc::network;
using namespace dcc::widgets;
using namespace NetworkManager;

VpnAdvVPNCSection::VpnAdvVPNCSection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent)
    : AbstractSection(tr("VPN Advanced"), parent),
      m_vpnSetting(vpnSetting),
      m_domain(new LineEditWidget(this)),
      m_vendorChooser(new ComboBoxWidget(this)),
      m_version(new LineEditWidget(this)),
      m_encryptionChooser(new ComboBoxWidget(this)),
      m_natTravModeChooser(new ComboBoxWidget(this)),
      m_ikeDHGroupChooser(new ComboBoxWidget(this)),
      m_forwordSecrecyChooser(new ComboBoxWidget(this)),
      m_localPort(new SpinBoxWidget(this)),
      m_disableDPD(new SwitchWidget(this)),
      m_currentVendor("cisco"),
      m_currentEncryption("secure"),
      m_currentNatTravMod("natt"),
      m_currentIkeDHGroup("dh2"),
      m_currentForwordSecrecy("server")
{
    m_dataMap = vpnSetting->data();

    initStrMaps();

    // init chooser current values
    for (auto vendor : VendorStrMap.values()) {
        if (m_dataMap.value("Vendor") == vendor) {
            m_currentVendor = vendor;
            break;
        }
    }

    if (m_dataMap.value("Enable no encryption") == "yes") {
        m_currentEncryption = "none";
    } else if (m_dataMap.value("Enable Single DES") == "yes") {
        m_currentEncryption = "weak";
    } else {
        m_currentEncryption = "secure";
    }

    for (auto mode : NATTravModeStrMap.values()) {
        if (m_dataMap.value("NAT Traversal Mode") == mode) {
            m_currentNatTravMod = mode;
            break;
        }
    }

    for (auto ike : IKEDHGroupStrMap.values()) {
        if (m_dataMap.value("IKE DH Group") == ike) {
            m_currentIkeDHGroup = ike;
            break;
        }
    }

    for (auto secrecy : ForwardSecrecyStrMap.values()) {
        if (m_dataMap.value("Perfect Forward Secrecy") == secrecy) {
            m_currentForwordSecrecy = secrecy;
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
    } else {
        qDebug() << "invalid current encryption";
    }
    m_dataMap.insert("NAT Traversal Mode", m_currentNatTravMod);
    m_dataMap.insert("IKE DH Group", m_currentIkeDHGroup);
    m_dataMap.insert("Perfect Forward Secrecy", m_currentForwordSecrecy);
    m_dataMap.insert("Local Port", QString::number(m_localPort->spinBox()->value()));
    if (m_disableDPD->checked()) {
        m_dataMap.insert("DPD idle timeout (our side)", "0");
    } else {
        m_dataMap.remove("DPD idle timeout (our side)");
    }

    m_vpnSetting->setData(m_dataMap);

    m_vpnSetting->setInitialized(true);
}

void VpnAdvVPNCSection::initStrMaps()
{
    VendorStrMap = {
        {tr("Cisco (default)"), "cisco"},
        {tr("Netscreen"), "netscreen"},
    };
    EncryptionStrMap = {
        {tr("Secure (default)"), "secure"},
        {tr("Weak"), "weak"},
        {tr("None"), "none"},
    };
    NATTravModeStrMap = {
        {tr("NAT-T When Available (default)"), "natt"},
        {tr("NAT-T Always"), "force-natt"},
        {tr("Cisco UDP"), "cisco-udp"},
        {tr("Disabled"), "none"},
    };
    IKEDHGroupStrMap = {
        {tr("DH Group 1"), "dh1"},
        {tr("DH Group 2 (default)"), "dh2"},
        {tr("DH Group 5"), "dh5"},
    };
    ForwardSecrecyStrMap = {
        {tr("Server (default)"), "server"},
        {tr("None"), "nopfs"},
        {tr("DH Group 1"), "dh1"},
        {tr("DH Group 2"), "dh2"},
        {tr("DH Group 5"), "dh5"},
    };
}

void VpnAdvVPNCSection::initUI()
{
    m_domain->setTitle(tr("Domain"));
    m_domain->setText(m_dataMap.value("Domain"));

    m_vendorChooser->setTitle(tr("Vendor"));
    for (auto vendor : VendorStrMap.values()) {
        m_vendorChooser->appendOption(VendorStrMap.key(vendor), vendor);
    }
    m_vendorChooser->setCurrent(m_currentVendor);

    m_version->setTitle(tr("Version"));
    m_version->setText(m_dataMap.value("Application Version"));

    m_encryptionChooser->setTitle(tr("Encryption"));
    for (auto encryption : EncryptionStrMap.values()) {
        m_encryptionChooser->appendOption(EncryptionStrMap.key(encryption), encryption);
    }
    m_encryptionChooser->setCurrent(m_currentEncryption);

    m_natTravModeChooser->setTitle(tr("NAT Traversal Mode"));
    for (auto mode : NATTravModeStrMap.values()) {
        m_natTravModeChooser->appendOption(NATTravModeStrMap.key(mode), mode);
    }
    m_natTravModeChooser->setCurrent(m_currentNatTravMod);

    m_ikeDHGroupChooser->setTitle(tr("IKE DH Group"));
    for (auto ike : IKEDHGroupStrMap.values()) {
        m_ikeDHGroupChooser->appendOption(IKEDHGroupStrMap.key(ike), ike);
    }
    m_ikeDHGroupChooser->setCurrent(m_currentIkeDHGroup);

    m_forwordSecrecyChooser->setTitle(tr("Forward Secrecy"));
    for (auto secrecy : ForwardSecrecyStrMap.values()) {
        m_forwordSecrecyChooser->appendOption(ForwardSecrecyStrMap.key(secrecy), secrecy);
    }
    m_forwordSecrecyChooser->setCurrent(m_currentForwordSecrecy);

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
}

void VpnAdvVPNCSection::initConnection()
{
    connect(m_vendorChooser, &ComboBoxWidget::requestPage, this, &VpnAdvVPNCSection::requestNextPage);
    connect(m_vendorChooser, &ComboBoxWidget::dataChanged, this, [=](const QVariant &data) {
        m_currentVendor = data.value<QString>();
    });

    connect(m_encryptionChooser, &ComboBoxWidget::requestPage, this, &VpnAdvVPNCSection::requestNextPage);
    connect(m_encryptionChooser, &ComboBoxWidget::dataChanged, this, [=](const QVariant &data) {
        m_currentEncryption = data.value<QString>();
    });

    connect(m_natTravModeChooser, &ComboBoxWidget::requestPage, this, &VpnAdvVPNCSection::requestNextPage);
    connect(m_natTravModeChooser, &ComboBoxWidget::dataChanged, this, [=](const QVariant &data) {
        m_currentNatTravMod = data.value<QString>();
    });

    connect(m_ikeDHGroupChooser, &ComboBoxWidget::requestPage, this, &VpnAdvVPNCSection::requestNextPage);
    connect(m_ikeDHGroupChooser, &ComboBoxWidget::dataChanged, this, [=](const QVariant &data) {
        m_currentIkeDHGroup = data.value<QString>();
    });

    connect(m_forwordSecrecyChooser, &ComboBoxWidget::requestPage, this, &VpnAdvVPNCSection::requestNextPage);
    connect(m_forwordSecrecyChooser, &ComboBoxWidget::dataChanged, this, [=](const QVariant &data) {
        m_currentForwordSecrecy = data.value<QString>();
    });
}
