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

#include "vpnopenvpnsection.h"
#include "widgets/passwdeditwidget.h"
#include "widgets/switchwidget.h"

using namespace dcc::network;
using namespace dcc::widgets;
using namespace NetworkManager;

VpnOpenVPNSection::VpnOpenVPNSection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent)
    : AbstractSection(tr("VPN"), parent),
      m_vpnSetting(vpnSetting),
      m_gateway(new LineEditWidget(this)),
      m_authTypeChooser(new ComboBoxWidget(this)),
      m_caFile(new FileChooseWidget(this))
{
    m_dataMap = vpnSetting->data();
    m_secretMap = vpnSetting->secrets();

    initStrMaps();
    initUI();
    initConnection();

    onAuthTypeChanged(m_currentAuthType);
}

VpnOpenVPNSection::~VpnOpenVPNSection()
{
}

bool VpnOpenVPNSection::allInputValid()
{
    bool valid = true;

    if (m_gateway->text().isEmpty()) {
        valid = false;
        m_gateway->setIsErr(true);
    } else {
        m_gateway->setIsErr(false);
    }

    if (m_caFile->edit()->text().isEmpty()) {
        m_caFile->setIsErr(true);
    } else {
        m_caFile->setIsErr(false);
    }

    if (m_currentAuthType == "tls") {
        valid = tlsItemsInputValid();
    } else if (m_currentAuthType == "password") {
        valid = passwordItemsInputValid();
    } else if (m_currentAuthType == "password-tls") {
        valid = tlsItemsInputValid();
        valid = passwordItemsInputValid();
    } else if (m_currentAuthType == "static-key") {
        valid = staticKeyItemsInputValid();
    }

    return valid;
}

void VpnOpenVPNSection::saveSettings()
{
    // retrieve the data map
    m_dataMap = m_vpnSetting->data();
    m_secretMap = m_vpnSetting->secrets();

    m_dataMap.insert("remote", m_gateway->text());
    m_dataMap.insert("connection-type", m_currentAuthType);

    if (m_currentAuthType == "tls") {
        saveTlsItems();
    } else if (m_currentAuthType == "password") {
        savePasswordItems();
    } else if (m_currentAuthType == "password-tls") {
        saveTlsItems();
        savePasswordItems();
    } else if (m_currentAuthType == "static-key") {
        saveStaticKeyItems();
    }

    if (m_currentAuthType != "static-key") {
        m_dataMap.insert("ca", m_caFile->edit()->text());
    }

    m_vpnSetting->setData(m_dataMap);
    m_vpnSetting->setSecrets(m_secretMap);

    m_vpnSetting->setInitialized(true);
}

void VpnOpenVPNSection::initStrMaps()
{
    AuthTypeStrMap = {
        {tr("Certificates (TLS)"), "tls"},
        {tr("Password"), "password"},
        {tr("Certificates with Password (TLS)"), "password-tls"},
        {tr("Static Key"), "static-key"},
    };

    PasswordFlagsStrMap = {
        //{"Saved", NetworkManager::Setting::AgentOwned},
        {tr("Saved"), NetworkManager::Setting::SecretFlagType::None},
        {tr("Ask"), NetworkManager::Setting::SecretFlagType::NotSaved},
        {tr("Not Required"), NetworkManager::Setting::SecretFlagType::NotRequired}
    };
}

void VpnOpenVPNSection::initUI()
{
    m_gateway->setTitle(tr("Gateway"));
    m_gateway->setPlaceholderText(tr("Required"));
    m_gateway->setText(m_dataMap.value("remote"));

    m_currentAuthType = "tls";
    for (auto type : AuthTypeStrMap) {
        if (type == m_dataMap.value("connection-type")) {
            m_currentAuthType = type;
        }
        m_authTypeChooser->appendOption(AuthTypeStrMap.key(type), type);
    }
    m_authTypeChooser->setTitle(tr("Auth Type"));
    m_authTypeChooser->setCurrent(m_currentAuthType);

    m_caFile->setTitle(tr("CA Cert"));
    m_caFile->edit()->setText(m_dataMap.value("ca"));
    m_caFile->setVisible(false);

    appendItem(m_gateway);
    appendItem(m_authTypeChooser);
    appendItem(m_caFile);
}

void VpnOpenVPNSection::initItems(const QString &type)
{
    if (type == "tls") {
        initTLSItems();
    } else if (type == "password") {
        initPasswordItems();
    } else if (type == "password-tls") {
        initPasswordItems();
        initTLSItems();
    } else if (type == "static-key") {
        initStaticKeyItems();
    } else {
        qDebug() << "invalid auth type:" << type;
    }
}

void VpnOpenVPNSection::initTLSItems()
{
    if (m_settingItemsMap.contains("tls")) {
        return;
    }

    FileChooseWidget *userCertFile = new FileChooseWidget(this);
    userCertFile->setTitle(tr("User Cert"));
    userCertFile->edit()->setText(m_dataMap.value("cert"));

    FileChooseWidget *priKeyFile = new FileChooseWidget(this);
    priKeyFile->setTitle(tr("Private Key"));
    priKeyFile->edit()->setText(m_dataMap.value("key"));

    ComboBoxWidget *certPasswordFlagsChooser = new ComboBoxWidget(this);
    certPasswordFlagsChooser->setTitle(tr("Pwd Options"));
    m_currentCertPasswordType = NetworkManager::Setting::SecretFlagType::None;
    for (auto flag : PasswordFlagsStrMap.values()) {
        if (flag == m_dataMap.value("cert-pass-flags").toInt()) {
            m_currentCertPasswordType = flag;
        }
        certPasswordFlagsChooser->appendOption(PasswordFlagsStrMap.key(flag), flag);
    }
    certPasswordFlagsChooser->setCurrent(m_currentCertPasswordType);

    PasswdEditWidget *priKeyPassword = new PasswdEditWidget(this);
    priKeyPassword->setTitle(tr("Private Pwd"));
    priKeyPassword->setText(m_secretMap.value("cert-pass"));

    connect(userCertFile->edit(), &QLineEdit::editingFinished, this, &VpnOpenVPNSection::allInputValid);
    connect(priKeyFile->edit(), &QLineEdit::editingFinished, this, &VpnOpenVPNSection::allInputValid);
    connect(priKeyPassword->textEdit(), &QLineEdit::editingFinished, this, &VpnOpenVPNSection::allInputValid);
    connect(certPasswordFlagsChooser, &ComboBoxWidget::requestPage, this, &VpnOpenVPNSection::requestNextPage);
    connect(certPasswordFlagsChooser, &ComboBoxWidget::dataChanged, this, [=](const QVariant &data) {
        m_currentCertPasswordType = data.value<NetworkManager::Setting::SecretFlagType>();
        priKeyPassword->setVisible(m_currentCertPasswordType == NetworkManager::Setting::SecretFlagType::None);
    });
    connect(userCertFile, &FileChooseWidget::requestFrameKeepAutoHide, this, &VpnOpenVPNSection::requestFrameAutoHide);
    connect(priKeyFile, &FileChooseWidget::requestFrameKeepAutoHide, this, &VpnOpenVPNSection::requestFrameAutoHide);

    QList<SettingsItem *> itemList;
    itemList << userCertFile << priKeyFile << certPasswordFlagsChooser << priKeyPassword;

    for (auto item : itemList) {
        appendItem(item);
    }

    m_settingItemsMap.insert("tls", itemList);
}

void VpnOpenVPNSection::initPasswordItems()
{
    if (m_settingItemsMap.contains("password")) {
        return;
    }

    LineEditWidget *userName = new LineEditWidget(this);
    userName->setTitle(tr("Username"));
    userName->setText(m_dataMap.value("username"));
    userName->setPlaceholderText(tr("Required"));

    ComboBoxWidget *passwordFlagsChooser = new ComboBoxWidget(this);
    passwordFlagsChooser->setTitle(tr("Pwd Options"));
    m_currentPasswordType = NetworkManager::Setting::SecretFlagType::None;
    for (auto flag : PasswordFlagsStrMap.values()) {
        if (flag == m_dataMap.value("password-flags").toInt()) {
            m_currentPasswordType = flag;
        }
        passwordFlagsChooser->appendOption(PasswordFlagsStrMap.key(flag), flag);
    }
    passwordFlagsChooser->setCurrent(m_currentPasswordType);

    PasswdEditWidget *password = new PasswdEditWidget(this);
    password->setTitle(tr("Password"));
    password->setText(m_secretMap.value("password"));
    password->setPlaceholderText(tr("Required"));

    connect(userName->textEdit(), &QLineEdit::editingFinished, this, &VpnOpenVPNSection::allInputValid);
    connect(password->textEdit(), &QLineEdit::editingFinished, this, &VpnOpenVPNSection::allInputValid);
    connect(passwordFlagsChooser, &ComboBoxWidget::requestPage, this, &VpnOpenVPNSection::requestNextPage);
    connect(passwordFlagsChooser, &ComboBoxWidget::dataChanged, this, [=](const QVariant &data) {
        m_currentPasswordType = data.value<NetworkManager::Setting::SecretFlagType>();
        password->setVisible(m_currentPasswordType == NetworkManager::Setting::SecretFlagType::None);
    });

    QList<SettingsItem *> itemList;
    itemList << userName << passwordFlagsChooser << password;

    insertItem(3, password);
    insertItem(3, passwordFlagsChooser);
    insertItem(3, userName);

    m_settingItemsMap.insert("password", itemList);
}

void VpnOpenVPNSection::initStaticKeyItems()
{
    if (m_settingItemsMap.contains("static-key")) {
        return;
    }

    FileChooseWidget *staticKey = new FileChooseWidget(this);
    staticKey->setTitle(tr("Static Key"));
    staticKey->edit()->setText(m_dataMap.value("static-key"));

    SwitchWidget *customizeKeyDirection = new SwitchWidget(this);
    customizeKeyDirection->setTitle(tr("Customize Key Direction"));
    customizeKeyDirection->setChecked(m_dataMap.keys().contains("static-key-direction"));

    ComboBoxWidget *keyDirectionChooser = new ComboBoxWidget(this);
    keyDirectionChooser->setTitle(tr("Key Direction"));
    keyDirectionChooser->appendOption("0", "0");
    keyDirectionChooser->appendOption("1", "1");
    keyDirectionChooser->setCurrent(m_dataMap.value("static-key-direction", "0"));
    keyDirectionChooser->setVisible(customizeKeyDirection->checked());

    LineEditWidget *remoteIp = new LineEditWidget(this);
    remoteIp->setTitle(tr("Remote IP"));
    remoteIp->setPlaceholderText(tr("Required"));
    remoteIp->setText(m_dataMap.value("remote-ip"));

    LineEditWidget *localIp = new LineEditWidget(this);
    localIp->setTitle(tr("Local IP"));
    localIp->setPlaceholderText(tr("Required"));
    localIp->setText(m_dataMap.value("local-ip"));

    connect(staticKey->edit(), &QLineEdit::textChanged, this, &VpnOpenVPNSection::allInputValid);
    connect(remoteIp->textEdit(), &QLineEdit::textChanged, this, &VpnOpenVPNSection::allInputValid);
    connect(localIp->textEdit(), &QLineEdit::textChanged, this, &VpnOpenVPNSection::allInputValid);
    connect(customizeKeyDirection, &SwitchWidget::checkedChanged,
            keyDirectionChooser, &ComboBoxWidget::setVisible);
    connect(keyDirectionChooser, &ComboBoxWidget::requestPage, this, &VpnOpenVPNSection::requestNextPage);
    connect(keyDirectionChooser, &ComboBoxWidget::dataChanged, this, [=](const QVariant &data) {
        m_currentKeyDirection = data.toString();
    });
    connect(staticKey, &FileChooseWidget::requestFrameKeepAutoHide, this, &VpnOpenVPNSection::requestFrameAutoHide);

    QList<SettingsItem *> itemList;
    itemList << staticKey << customizeKeyDirection << keyDirectionChooser << remoteIp << localIp;

    for (auto item : itemList) {
        appendItem(item);
    }

    m_settingItemsMap.insert("static-key", itemList);
}

void VpnOpenVPNSection::initConnection()
{
    connect(m_authTypeChooser, &ComboBoxWidget::dataChanged, this, [=](const QVariant &data) {
        onAuthTypeChanged(data.toString());
    });
    connect(m_authTypeChooser, &ComboBoxWidget::requestPage, this, &VpnOpenVPNSection::requestNextPage);

    connect(m_caFile, &FileChooseWidget::requestFrameKeepAutoHide, this, &VpnOpenVPNSection::requestFrameAutoHide);
}

void VpnOpenVPNSection::onAuthTypeChanged(const QString &type)
{
    if (!m_settingItemsMap.keys().contains(m_currentAuthType)) {
        initItems(m_currentAuthType);
    }

    if (!m_settingItemsMap.keys().contains(type)) {
        initItems(type);
    }

    if (m_currentAuthType == "password-tls") {
        setItemsVisible("tls", false);
        setItemsVisible("password", false);
    } else {
        setItemsVisible(m_currentAuthType, false);
    }

    m_currentAuthType = type;
    Q_EMIT authTypeChanged(m_currentAuthType);

    if (m_currentAuthType == "password-tls") {
        setItemsVisible("tls", true);
        setItemsVisible("password", true);
    } else {
        setItemsVisible(m_currentAuthType, true);
    }

    m_caFile->setVisible(m_currentAuthType != "static-key");
}

void VpnOpenVPNSection::setItemsVisible(const QString &itemsType, const bool visible)
{
    const QList<SettingsItem *> &itemsList = m_settingItemsMap.value(itemsType);
    for (auto item : itemsList) {
        item->setVisible(visible);
    }

    if (!visible) {
        return;
    }

    // refresh special items visivle
    if (itemsType == "tls") {
        ComboBoxWidget *tlsPasswordFlagsChooser = static_cast<ComboBoxWidget *>(itemsList.at(2));
        Q_EMIT tlsPasswordFlagsChooser->dataChanged(m_currentCertPasswordType);
    } else if (itemsType == "password") {
        ComboBoxWidget *passwordFlagsChooser = static_cast<ComboBoxWidget *>(itemsList.at(1));
        Q_EMIT passwordFlagsChooser->dataChanged(m_currentPasswordType);
    } else if (itemsType == "static-key") {
        SwitchWidget *customizeKeyDirection = static_cast<SwitchWidget *>(itemsList.at(1));
        Q_EMIT customizeKeyDirection->checkedChanged(customizeKeyDirection->checked());
    }
}

bool VpnOpenVPNSection::tlsItemsInputValid()
{
    bool valid = true;

    const QList<SettingsItem *> &itemsList = m_settingItemsMap.value("tls");
    FileChooseWidget *userCertFile = static_cast<FileChooseWidget *>(itemsList.at(0));
    FileChooseWidget *priKeyFile = static_cast<FileChooseWidget *>(itemsList.at(1));
    PasswdEditWidget *priKeyPassword = static_cast<PasswdEditWidget *>(itemsList.at(3));

    if (userCertFile->edit()->text().isEmpty()) {
        valid = false;
        userCertFile->setIsErr(true);
    } else {
        userCertFile->setIsErr(false);
    }

    if (priKeyFile->edit()->text().isEmpty()) {
        valid = false;
        priKeyFile->setIsErr(true);
    } else {
        priKeyFile->setIsErr(false);
    }

    if (m_currentCertPasswordType == NetworkManager::Setting::SecretFlagType::None) {
        if (priKeyPassword->text().isEmpty()) {
            valid = false;
            priKeyPassword->setIsErr(true);
        }
    } else {
        priKeyPassword->setIsErr(false);
    }

    return valid;
}

bool VpnOpenVPNSection::passwordItemsInputValid()
{
    bool valid = true;

    const QList<SettingsItem *> &itemsList = m_settingItemsMap.value("password");
    LineEditWidget *userName = static_cast<LineEditWidget *>(itemsList.at(0));
    PasswdEditWidget *password = static_cast<PasswdEditWidget *>(itemsList.at(2));

    if (userName->text().isEmpty()) {
        valid = false;
        userName->setIsErr(true);
    } else {
        userName->setIsErr(false);
    }

    if (m_currentPasswordType == NetworkManager::Setting::SecretFlagType::None) {
        if (password->text().isEmpty()) {
            valid = false;
            password->setIsErr(true);
        }
    } else {
        password->setIsErr(false);
    }

    return valid;
}

bool VpnOpenVPNSection::staticKeyItemsInputValid()
{
    bool valid = true;

    const QList<SettingsItem *> &itemsList = m_settingItemsMap.value("static-key");
    FileChooseWidget *staticKey = static_cast<FileChooseWidget *>(itemsList.at(0));
    LineEditWidget *remoteIp = static_cast<LineEditWidget *>(itemsList.at(3));
    LineEditWidget *localIp = static_cast<LineEditWidget *>(itemsList.at(4));

    if (staticKey->edit()->text().isEmpty()) {
        valid = false;
        staticKey->setIsErr(true);
    } else {
        staticKey->setIsErr(false);
    }

    if (remoteIp->text().isEmpty()) {
        valid = false;
        remoteIp->setIsErr(true);
    } else {
        remoteIp->setIsErr(false);
    }

    if (localIp->text().isEmpty()) {
        valid = false;
        localIp->setIsErr(true);
    } else {
        localIp->setIsErr(false);
    }

    return valid;
}

void VpnOpenVPNSection::saveTlsItems()
{
    const QList<SettingsItem *> &itemsList = m_settingItemsMap.value("tls");
    FileChooseWidget *userCertFile = static_cast<FileChooseWidget *>(itemsList.at(0));
    FileChooseWidget *priKeyFile = static_cast<FileChooseWidget *>(itemsList.at(1));
    PasswdEditWidget *priKeyPassword = static_cast<PasswdEditWidget *>(itemsList.at(3));

    m_dataMap.insert("cert", userCertFile->edit()->text());
    m_dataMap.insert("key", priKeyFile->edit()->text());
    m_dataMap.insert("cert-pass-flags", QString::number(m_currentCertPasswordType));

    m_secretMap.insert("cert-pass", priKeyPassword->text());
}

void VpnOpenVPNSection::savePasswordItems()
{
    const QList<SettingsItem *> &itemsList = m_settingItemsMap.value("password");
    LineEditWidget *userName = static_cast<LineEditWidget *>(itemsList.at(0));
    PasswdEditWidget *password = static_cast<PasswdEditWidget *>(itemsList.at(2));

    m_dataMap.insert("username", userName->text());
    m_dataMap.insert("password-flags", QString::number(m_currentPasswordType));

    m_secretMap.insert("password", password->text());
}

void VpnOpenVPNSection::saveStaticKeyItems()
{
    const QList<SettingsItem *> &itemsList = m_settingItemsMap.value("static-key");
    FileChooseWidget *staticKey = static_cast<FileChooseWidget *>(itemsList.at(0));
    SwitchWidget *customizeKeyDirection = static_cast<SwitchWidget *>(itemsList.at(1));
    LineEditWidget *remoteIp = static_cast<LineEditWidget *>(itemsList.at(3));
    LineEditWidget *localIp = static_cast<LineEditWidget *>(itemsList.at(4));

    m_dataMap.insert("static-key", staticKey->edit()->text());
    if (customizeKeyDirection->checked()) {
        m_dataMap.insert("static-key-direction", m_currentKeyDirection);
    } else {
        m_dataMap.remove("static-key-direction");
    }
    m_dataMap.insert("remote-ip", remoteIp->text());
    m_dataMap.insert("local-ip", localIp->text());
}
