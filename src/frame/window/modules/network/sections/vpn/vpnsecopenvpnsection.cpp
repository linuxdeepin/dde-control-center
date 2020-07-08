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

#include "vpnsecopenvpnsection.h"

#include <QComboBox>

using namespace DCC_NAMESPACE::network;
using namespace dcc::widgets;
using namespace NetworkManager;

VpnSecOpenVPNSection::VpnSecOpenVPNSection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent)
    : AbstractSection(tr("VPN Security"), parent)
    , m_vpnSetting(vpnSetting)
    , m_dataMap(vpnSetting->data())
    , m_cipherChooser(new ComboxWidget(this))
    , m_hmacChooser(new ComboxWidget(this))
{
    initStrMaps();
    initUI();
    initConnection();
}

VpnSecOpenVPNSection::~VpnSecOpenVPNSection()
{
}

bool VpnSecOpenVPNSection::allInputValid()
{
    return true;
}

void VpnSecOpenVPNSection::saveSettings()
{
    // retrieve the data map
    m_dataMap = m_vpnSetting->data();

    if (m_currentCipher == "default") {
        m_dataMap.remove("cipher");
    } else {
        m_dataMap.insert("cipher", m_currentCipher);
    }

    if (m_currentHMAC == "default") {
        m_dataMap.remove("auth");
    } else {
        m_dataMap.insert("auth", m_currentHMAC);
    }

    m_vpnSetting->setData(m_dataMap);

    m_vpnSetting->setInitialized(true);
}

void VpnSecOpenVPNSection::initStrMaps()
{
    CipherStrMap = {
        {tr("Default"), "default"},
        {tr("None"), "none"},
        {"DES-CBC", "DES-CBC"},
        {"RC2-CBC", "RC2-CBC"},
        {"DES-EDE-CBC", "DES-EDE-CBC"},
        {"DES-EDE3-CBC", "DES-EDE3-CBC"},
        {"DESX-CBC", "DESX-CBC"},
        {"BF-CBC", "BF-CBC"},
        {"RC2-40-CBC", "RC2-40-CBC"},
        {"CAST5-CBC", "CAST5-CBC"},
        {"RC2-64-CBC", "RC2-64-CBC"},
        {"AES-128-CBC", "AES-128-CBC"},
        {"AES-192-CBC", "AES-192-CBC"},
        {"AES-256-CBC", "AES-256-CBC"},
        {"CAMELLIA-128-CBC", "CAMELLIA-128-CBC"},
        {"CAMELLIA-192-CBC", "CAMELLIA-192-CBC"},
        {"CAMELLIA-256-CBC", "CAMELLIA-256-CBC"},
        {"SEED-CBC", "SEED-CBC"}
    };

    HMACStrMap = {
        {tr("Default"), "default"},
        {tr("None"), "none"},
        {"RSA MD-4", "RSA-MD4"},
        {"MD-5", "MD5"},
        {"SHA-1", "SHA1"},
        {"SHA-224", "SHA224"},
        {"SHA-256", "SHA256"},
        {"SHA-384", "SHA384"},
        {"SHA-512", "SHA512"},
        {"RIPEMD-160", "RIPEMD160"},
    };
}

void VpnSecOpenVPNSection::initUI()
{
    m_cipherChooser->setTitle(tr("Cipher"));
    m_currentCipher = "default";
    QString curCipherOption = CipherStrMap.at(0).first;
    for (auto it = CipherStrMap.cbegin(); it != CipherStrMap.cend(); ++it) {
        m_cipherChooser->comboBox()->addItem(it->first, it->second);
        if (it->second == m_dataMap.value("cipher")) {
            m_currentCipher = it->second;
            curCipherOption = it->first;
        }
    }
    m_cipherChooser->setCurrentText(curCipherOption);

    m_hmacChooser->setTitle(tr("HMAC Auth"));
    m_currentHMAC = "default";
    QString curHMACOption = HMACStrMap.at(0).first;
    for (auto it = HMACStrMap.cbegin(); it != HMACStrMap.cend(); ++it) {
        m_hmacChooser->comboBox()->addItem(it->first, it->second);
        if (it->second == m_dataMap.value("auth")) {
            m_currentHMAC = it->second;
            curHMACOption = it->first;
        }
    }
    m_hmacChooser->setCurrentText(curHMACOption);

    appendItem(m_cipherChooser);
    appendItem(m_hmacChooser);
}

void VpnSecOpenVPNSection::initConnection()
{
    connect(m_cipherChooser, &ComboxWidget::onSelectChanged, this, [ = ](const QString &dataSelected) {
        for (auto it = CipherStrMap.cbegin(); it != CipherStrMap.cend(); ++it) {
            if (it->first == dataSelected) {
                m_currentCipher = it->second;
                break;
            }
        }
    });
    connect(m_hmacChooser, &ComboxWidget::onSelectChanged, this, [ = ](const QString &dataSelected) {
        for (auto it = HMACStrMap.cbegin(); it != HMACStrMap.cend(); ++it) {
            if (it->first == dataSelected) {
                m_currentHMAC = it->second;
                break;
            }
        }
    });
}
