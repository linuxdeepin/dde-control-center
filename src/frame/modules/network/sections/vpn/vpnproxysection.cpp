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

#include "vpnproxysection.h"

#include <dspinbox.h>

using namespace dcc::network;
using namespace dcc::widgets;
using namespace NetworkManager;

VpnProxySection::VpnProxySection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent)
    : AbstractSection(tr("VPN Proxy"), parent),
      m_vpnSetting(vpnSetting),
    m_proxyTypeChooser(new ComboBoxWidget(this)),
    m_server(new LineEditWidget(this)),
    m_port(new SpinBoxWidget(this)),
    m_retry(new SwitchWidget(this)),
    m_userName(new LineEditWidget(this)),
    m_password(new PasswdEditWidget(this))
{
    m_dataMap = vpnSetting->data();
    m_secretMap = vpnSetting->secrets();

    initStrMaps();
    initUI();
    initConnection();

    onProxyTypeChanged(m_currentProxyType);
}

VpnProxySection::~VpnProxySection()
{
}

bool VpnProxySection::allInputValid()
{
    bool valid = true;

    if (m_currentProxyType == "none") {
        return true;
    }

    if (m_server->text().isEmpty()) {
        valid = false;
        m_server->setIsErr(true);
    } else {
        m_server->setIsErr(false);
    }

    if (m_currentProxyType == "http") {
        if (m_userName->text().isEmpty()) {
            valid = false;
            m_userName->setIsErr(true);
        } else {
            m_userName->setIsErr(false);
        }

        if (m_password->text().isEmpty()) {
            valid = false;
            m_password->setIsErr(true);
        } else {
            m_password->setIsErr(false);
        }
    }

    return valid;
}

void VpnProxySection::saveSettings()
{
    // retrieve the data map
    m_dataMap = m_vpnSetting->data();
    m_secretMap = m_vpnSetting->secrets();

    if (m_currentProxyType == "none") {
        m_dataMap.remove("proxy-type");
        m_dataMap.remove("proxy-server");
        m_dataMap.remove("proxy-port");
        m_dataMap.remove("proxy-retry");
        m_dataMap.remove("http-proxy-username");
        m_dataMap.remove("http-proxy-password-flags");
        m_secretMap.remove("http-proxy-password");
    }

    if (m_currentProxyType == "http" || m_currentProxyType == "socks") {
        m_dataMap.insert("proxy-type", m_currentProxyType);
        m_dataMap.insert("proxy-server", m_server->text());
        m_dataMap.insert("proxy-port", QString::number(m_port->spinBox()->value()));

        if (m_retry->checked()) {
            m_dataMap.insert("proxy-retry", "yes");
        } else {
            m_dataMap.remove("proxy-retry");
        }

        if (m_currentProxyType == "http") {
            m_dataMap.insert("http-proxy-username", m_userName->text());
            m_dataMap.insert("http-proxy-password-flags", "0");
            m_secretMap.insert("http-proxy-password", m_password->text());
        } else {
            m_dataMap.remove("http-proxy-username");
            m_dataMap.remove("http-proxy-password-flags");
            m_secretMap.remove("http-proxy-password");
        }
    }

    m_vpnSetting->setData(m_dataMap);
    m_vpnSetting->setSecrets(m_secretMap);

    m_vpnSetting->setInitialized(true);
}

void VpnProxySection::initStrMaps()
{
    ProxyTypeStrMap = {
        {tr("Not Required"), "none"},
        {tr("HTTP"), "http"},
        {tr("SOCKS"), "socks"},
    };
}

void VpnProxySection::initUI()
{
    m_proxyTypeChooser->setTitle(tr("Proxy Type"));
    m_currentProxyType = "none";
    for (auto value : ProxyTypeStrMap.values()) {
        if (value == m_dataMap.value("proxy-type")) {
            m_currentProxyType = value;
        }
        m_proxyTypeChooser->appendOption(ProxyTypeStrMap.key(value), value);
    }
    m_proxyTypeChooser->setCurrent(m_currentProxyType);

    m_server->setTitle(tr("Server IP"));
    m_server->setPlaceholderText(tr("Required"));
    m_server->setText(m_dataMap.value("proxy-server"));

    m_port->setTitle(tr("Port"));
    m_port->spinBox()->setMinimum(0);
    m_port->spinBox()->setMaximum(65535);
    m_port->spinBox()->setValue(m_dataMap.value("proxy-port").toInt());

    m_retry->setTitle(tr("Retry Indefinitely When Failed"));
    m_retry->setChecked(m_dataMap.value("proxy-retry") == "yes");

    m_userName->setTitle(tr("Username"));
    m_userName->setPlaceholderText(tr("Required"));
    m_userName->setText(m_dataMap.value("http-proxy-username"));

    m_password->setTitle(tr("Password"));
    m_password->setPlaceholderText(tr("Required"));
    m_password->setText(m_secretMap.value("http-proxy-password"));

    appendItem(m_proxyTypeChooser);
    appendItem(m_server);
    appendItem(m_port);
    appendItem(m_retry);
    appendItem(m_userName);
    appendItem(m_password);
}

void VpnProxySection::initConnection()
{
    connect(m_proxyTypeChooser, &ComboBoxWidget::requestPage, this, &VpnProxySection::requestNextPage);
    connect(m_proxyTypeChooser, &ComboBoxWidget::dataChanged, this, [=](const QVariant &data) {
        onProxyTypeChanged(data.toString());
    });

    connect(m_server->textEdit(), &QLineEdit::editingFinished, this, &VpnProxySection::allInputValid);
    connect(m_userName->textEdit(), &QLineEdit::editingFinished, this, &VpnProxySection::allInputValid);
    connect(m_password->textEdit(), &QLineEdit::editingFinished, this, &VpnProxySection::allInputValid);
}

void VpnProxySection::onProxyTypeChanged(const QString &type)
{
    m_currentProxyType = type;

    m_server->setVisible(m_currentProxyType != "none");
    m_port->setVisible(m_currentProxyType != "none");
    m_retry->setVisible(m_currentProxyType != "none");

    m_userName->setVisible(m_currentProxyType == "http");
    m_password->setVisible(m_currentProxyType == "http");
}
