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

#include "vpntlssection.h"

using namespace dcc::network;
using namespace dcc::widgets;
using namespace NetworkManager;

VpnTLSSection::VpnTLSSection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent)
    : AbstractSection(tr("VPN TLS Authentication"), parent),
      m_vpnSetting(vpnSetting),
    m_remote(new LineEditWidget(this)),
    m_remoteCertTypeChooser(new ComboBoxWidget(this)),
    m_caCert(new FileChooseWidget(this)),
    m_customKeyDirection(new SwitchWidget(this)),
    m_keyDirectionChooser(new ComboBoxWidget(this))
{
    m_dataMap = vpnSetting->data();

    initStrMaps();
    initUI();
    initConnection();
}

VpnTLSSection::~VpnTLSSection()
{
}

bool VpnTLSSection::allInputValid()
{
    return true;
}

void VpnTLSSection::saveSettings()
{
    // retrieve the data map
    m_dataMap = m_vpnSetting->data();

    m_dataMap.insert("tls-remote", m_remote->text());
    if (m_currentRemoteCertType == "default") {
        m_dataMap.remove("remote-cert-tls");
    } else {
        m_dataMap.insert("remote-cert-tls", m_currentRemoteCertType);
    }
    m_dataMap.insert("ta", m_caCert->edit()->text());
    if (m_customKeyDirection->checked()) {
        m_dataMap.insert("ta-dir", m_currentKeyDirection);
    } else {
        m_dataMap.remove("ta-dir");
    }

    m_vpnSetting->setData(m_dataMap);

    m_vpnSetting->setInitialized(true);
}

void VpnTLSSection::initStrMaps()
{
    RemoteCertTypeStrMap = {
        {tr("Default"), "default"},
        {tr("Client"), "client"},
        {tr("Server"), "server"},
    };
}

void VpnTLSSection::initUI()
{
    m_remote->setTitle(tr("Subject Match"));
    m_remote->setText(m_dataMap.value("tls-remote"));

    m_remoteCertTypeChooser->setTitle(tr("Remote Cert Type"));
    m_currentRemoteCertType = "default";
    for (auto value : RemoteCertTypeStrMap.values()) {
        if (value == m_dataMap.value("remote-cert-tls")) {
            m_currentRemoteCertType = value;
        }
        m_remoteCertTypeChooser->appendOption(RemoteCertTypeStrMap.key(value), value);
    }
    m_remoteCertTypeChooser->setCurrent(m_currentRemoteCertType);

    m_caCert->setTitle(tr("Key File"));
    m_caCert->edit()->setText(m_dataMap.value("ta"));

    m_customKeyDirection->setTitle(tr("Customize Key Direction"));
    m_customKeyDirection->setChecked(m_dataMap.keys().contains("ta-dir"));

    m_keyDirectionChooser->setTitle(tr("Key Direction"));
    m_keyDirectionChooser->appendOption("0", "0");
    m_keyDirectionChooser->appendOption("1", "1");
    m_currentKeyDirection = m_dataMap.value("ta-dir", "0");
    m_keyDirectionChooser->setCurrent(m_currentKeyDirection);
    m_keyDirectionChooser->setVisible(m_customKeyDirection->checked());

    appendItem(m_remote);
    appendItem(m_remoteCertTypeChooser);
    appendItem(m_caCert);
    appendItem(m_customKeyDirection);
    appendItem(m_keyDirectionChooser);
}

void VpnTLSSection::initConnection()
{
    connect(m_remoteCertTypeChooser, &ComboBoxWidget::requestPage, this, &VpnTLSSection::requestNextPage);
    connect(m_remoteCertTypeChooser, &ComboBoxWidget::dataChanged, this, [=](const QVariant &data) {
        m_currentRemoteCertType = data.toString();
    });

    connect(m_customKeyDirection, &SwitchWidget::checkedChanged, m_keyDirectionChooser, &ComboBoxWidget::setVisible);

    connect(m_keyDirectionChooser, &ComboBoxWidget::requestPage, this, &VpnTLSSection::requestNextPage);
    connect(m_keyDirectionChooser, &ComboBoxWidget::dataChanged, this, [=](const QVariant &data) {
        m_currentKeyDirection = data.toString();
    });

    connect(m_caCert, &FileChooseWidget::requestFrameKeepAutoHide, this, &VpnTLSSection::requestFrameAutoHide);
}
