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

#include "vpnsstpproxysection.h"

#include <dspinbox.h>

using namespace dcc::network;
using namespace dcc::widgets;
using namespace NetworkManager;

VpnSstpProxySection::VpnSstpProxySection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent)
    : AbstractSection(tr("VPN Proxy"), parent),
      m_vpnSetting(vpnSetting),
    m_server(new LineEditWidget(this)),
    m_port(new SpinBoxWidget(this)),
    m_userName(new LineEditWidget(this)),
    m_password(new PasswdEditWidget(this))
{
    m_dataMap = vpnSetting->data();
    m_secretMap = vpnSetting->secrets();

    initUI();
    initConnection();
}

VpnSstpProxySection::~VpnSstpProxySection()
{
}

bool VpnSstpProxySection::allInputValid()
{
    bool valid = true;

    QString server = m_server->text();
    int port = m_port->spinBox()->value();

    valid = (server.isEmpty() == (port == 0));

    if (!valid) {
        m_server->setIsErr(server.isEmpty());
        m_port->setIsErr(port == 0);
    } else {
        m_server->setIsErr(false);
        m_port->setIsErr(false);
    }

    return valid;
}

void VpnSstpProxySection::saveSettings()
{
    // retrieve the data map
    m_dataMap = m_vpnSetting->data();
    m_secretMap = m_vpnSetting->secrets();

    if (m_server->text().isEmpty() || m_port->spinBox()->value() == 0) {
        m_dataMap.remove("proxy-server");
        m_dataMap.remove("proxy-port");
        m_dataMap.remove("proxy-user");
        m_secretMap.remove("proxy-password");
    } else {
        m_dataMap.insert("proxy-server", m_server->text());
        m_dataMap.insert("proxy-port", QString::number(m_port->spinBox()->value()));
    }

    if (m_userName->text().isEmpty()) {
        m_dataMap.remove("proxy-user");
        m_secretMap.remove("proxy-password");
    } else {
        m_dataMap.insert("proxy-user", m_userName->text());
        if (m_password->text().isEmpty()) {
            m_secretMap.remove("proxy-password");
        } else {
            m_secretMap.insert("proxy-password", m_password->text());
        }
    }

    m_vpnSetting->setData(m_dataMap);
    m_vpnSetting->setSecrets(m_secretMap);

    m_vpnSetting->setInitialized(true);
}

void VpnSstpProxySection::initUI()
{
    m_server->setTitle(tr("Server IP"));
    m_server->setText(m_dataMap.value("proxy-server"));

    m_port->setTitle(tr("Port"));
    m_port->spinBox()->setMinimum(0);
    m_port->spinBox()->setMaximum(65535);
    m_port->spinBox()->setValue(m_dataMap.value("proxy-port").toInt());

    m_userName->setTitle(tr("Username"));
    m_userName->setText(m_dataMap.value("proxy-user"));

    m_password->setTitle(tr("Password"));
    m_password->setText(m_secretMap.value("proxy-password"));

    appendItem(m_server);
    appendItem(m_port);
    appendItem(m_userName);
    appendItem(m_password);
}

void VpnSstpProxySection::initConnection()
{
    connect(m_server->textEdit(), &QLineEdit::editingFinished, this, &VpnSstpProxySection::allInputValid);
    connect(m_port->spinBox(), static_cast<void (DSpinBox::*) (int)>(&DSpinBox::valueChanged), this, &VpnSstpProxySection::allInputValid);
//    connect(m_userName->textEdit(), &QLineEdit::editingFinished, this, &VpnSstpProxySection::allInputValid);
//    connect(m_password->textEdit(), &QLineEdit::editingFinished, this, &VpnSstpProxySection::allInputValid);
}
