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

#include "secrethotspotsection.h"
#include "widgets/contentwidget.h"
#include "widgets/comboxwidget.h"
#include "widgets/lineeditwidget.h"

#include <networkmanagerqt/utils.h>

#include <QComboBox>

using namespace dcc::widgets;
using namespace NetworkManager;

static const QList<WirelessSecuritySetting::KeyMgmt> KeyMgmtList {
    WirelessSecuritySetting::KeyMgmt::WpaNone,
    //WirelessSecuritySetting::KeyMgmt::Wep,
    WirelessSecuritySetting::KeyMgmt::WpaPsk,
};

SecretHotspotSection::SecretHotspotSection(WirelessSecuritySetting::Ptr wsSeting, QFrame *parent)
    : AbstractSection(parent)
    , m_keyMgmtChooser(new ComboxWidget(this))
    , m_passwdEdit(new LineEditWidget(true, this))
    , m_currentKeyMgmt(WirelessSecuritySetting::KeyMgmt::WpaNone)
    , m_wsSetting(wsSeting)
{
    initStrMaps();

    // init KeyMgmt
    const WirelessSecuritySetting::KeyMgmt &keyMgmt = m_wsSetting->keyMgmt();
    m_currentKeyMgmt = (keyMgmt == WirelessSecuritySetting::KeyMgmt::Unknown) ?
                       WirelessSecuritySetting::KeyMgmt::WpaNone : keyMgmt;

    initUI();
    initConnection();

    onKeyMgmtChanged(m_currentKeyMgmt);
}

SecretHotspotSection::~SecretHotspotSection()
{
}

bool SecretHotspotSection::allInputValid()
{
    bool valid = true;

    if (m_currentKeyMgmt == WirelessSecuritySetting::KeyMgmt::Wep) {
        valid = wepKeyIsValid(m_passwdEdit->text(), WirelessSecuritySetting::WepKeyType::Passphrase);
        m_passwdEdit->setIsErr(!valid);
        if (!valid && !m_passwdEdit->text().isEmpty())
            m_passwdEdit->showAlertMessage(tr("Invalid password"));
    }

    if (m_currentKeyMgmt == WirelessSecuritySetting::KeyMgmt::WpaPsk) {
        valid = wpaPskIsValid(m_passwdEdit->text());
        m_passwdEdit->setIsErr(!valid);
        if (!valid && !m_passwdEdit->text().isEmpty())
            m_passwdEdit->showAlertMessage(tr("Invalid password"));
    }

    return valid;
}

void SecretHotspotSection::saveSettings()
{
    if (m_currentKeyMgmt == WirelessSecuritySetting::KeyMgmt::WpaNone) {
        m_wsSetting->setInitialized(false);
        return;
    }

    m_wsSetting->setKeyMgmt(m_currentKeyMgmt);

    if (m_currentKeyMgmt == WirelessSecuritySetting::KeyMgmt::Wep) {
        m_wsSetting->setAuthAlg(WirelessSecuritySetting::AuthAlg::Open);
        m_wsSetting->setWepKeyType(WirelessSecuritySetting::WepKeyType::Passphrase);
        m_wsSetting->setWepKey0(m_passwdEdit->text());
        m_wsSetting->setPsk("");
    } else if (m_currentKeyMgmt == WirelessSecuritySetting::KeyMgmt::WpaPsk) {
        m_wsSetting->setPsk(m_passwdEdit->text());
        m_wsSetting->setPskFlags(NetworkManager::Setting::AgentOwned);
        m_wsSetting->setProto(QList<NetworkManager::WirelessSecuritySetting::WpaProtocolVersion>{NetworkManager::WirelessSecuritySetting::Wpa,NetworkManager::WirelessSecuritySetting::Rsn});
        m_wsSetting->setGroup(QList<NetworkManager::WirelessSecuritySetting::WpaEncryptionCapabilities>{NetworkManager::WirelessSecuritySetting::Ccmp});
        m_wsSetting->setPairwise(QList<NetworkManager::WirelessSecuritySetting::WpaEncryptionCapabilities>{NetworkManager::WirelessSecuritySetting::Ccmp});
    }

    m_wsSetting->setInitialized(true);
}

void SecretHotspotSection::initStrMaps()
{
    KeyMgmtStrMap = {
        { tr("None"), WirelessSecuritySetting::KeyMgmt::WpaNone },
        { tr("WEP"), WirelessSecuritySetting::KeyMgmt::Wep },
        { tr("WPA/WPA2 Personal"), WirelessSecuritySetting::KeyMgmt::WpaPsk },
    };
}

void SecretHotspotSection::initUI()
{
    QComboBox *cb = m_keyMgmtChooser->comboBox();
    m_keyMgmtChooser->setTitle(tr("Security"));
    for (auto keyMgmt : KeyMgmtList)
        cb->addItem(KeyMgmtStrMap.key(keyMgmt), keyMgmt);

    cb->setCurrentIndex(cb->findData(m_currentKeyMgmt));

    m_passwdEdit->setPlaceholderText(tr("Required"));

    appendItem(m_keyMgmtChooser);
    appendItem(m_passwdEdit);

    m_passwdEdit->textEdit()->installEventFilter(this);
}

void SecretHotspotSection::initConnection()
{
    connect(m_keyMgmtChooser->comboBox(), static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [ this ] {
        onKeyMgmtChanged(this->m_keyMgmtChooser->comboBox()->currentData().value<WirelessSecuritySetting::KeyMgmt>());
    });

    connect(m_passwdEdit->textEdit(), &QLineEdit::editingFinished, this, &SecretHotspotSection::saveUserInputPassword);
    connect(m_keyMgmtChooser, &ComboxWidget::onIndexChanged, this, &SecretHotspotSection::editClicked);
}

void SecretHotspotSection::onKeyMgmtChanged(WirelessSecuritySetting::KeyMgmt keyMgmt)
{
    if (m_currentKeyMgmt != keyMgmt)
        m_currentKeyMgmt = keyMgmt;

    switch (m_currentKeyMgmt) {
    case WirelessSecuritySetting::KeyMgmt::WpaNone: {
        m_passwdEdit->setVisible(false);
        break;
    }

    case WirelessSecuritySetting::KeyMgmt::Wep: {
        m_passwdEdit->setText(m_wsSetting->wepKey0());
        m_passwdEdit->setTitle(tr("Key"));
        m_passwdEdit->setVisible(true);
        break;
    }

    case WirelessSecuritySetting::KeyMgmt::WpaPsk: {
        m_passwdEdit->setText(m_wsSetting->psk());
        m_passwdEdit->setTitle(tr("Password"));
        m_passwdEdit->setVisible(true);
        break;
    }

    default:
        break;
    }

    if (m_userInputPasswordMap.contains(m_currentKeyMgmt))
        m_passwdEdit->setText(m_userInputPasswordMap.value(m_currentKeyMgmt));
}

void SecretHotspotSection::saveUserInputPassword()
{
    m_userInputPasswordMap.insert(m_currentKeyMgmt, m_passwdEdit->text());
}

bool SecretHotspotSection::eventFilter(QObject *watched, QEvent *event)
{
    // 实现鼠标点击编辑框，确定按钮激活，统一网络模块处理，捕捉FocusIn消息
    if (event->type() == QEvent::FocusIn) {
        if (dynamic_cast<QLineEdit *>(watched))
            Q_EMIT editClicked();
    }

    return QWidget::eventFilter(watched, event);
}
