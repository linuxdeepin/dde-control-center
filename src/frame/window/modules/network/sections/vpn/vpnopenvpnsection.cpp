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
#include "widgets/switchwidget.h"

#include <DPasswordEdit>
#include <DApplicationHelper>

#include <QComboBox>

using namespace DCC_NAMESPACE::network;
using namespace dcc::widgets;
using namespace NetworkManager;

VpnOpenVPNSection::VpnOpenVPNSection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent)
    : AbstractSection(tr("VPN"), parent)
    , m_vpnSetting(vpnSetting)
    , m_gateway(new LineEditWidget(this))
    , m_authTypeChooser(new ComboxWidget(this))
    , m_caFile(new FileChooseWidget(this))
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

    m_authTypeChooser->setTitle(tr("Auth Type"));
    m_currentAuthType = "tls";
    QString curAuthOption = AuthTypeStrMap.at(0).first;
    for (auto it = AuthTypeStrMap.cbegin(); it != AuthTypeStrMap.cend(); ++it) {
        m_authTypeChooser->comboBox()->addItem(it->first, it->second);
        if (it->second == m_dataMap.value("connection-type")) {
            m_currentAuthType = it->second;
            curAuthOption = it->first;
        }
    }
    m_authTypeChooser->setCurrentText(curAuthOption);

    m_caFile->setTitle(tr("CA Cert"));
    m_caFile->edit()->setText(m_dataMap.value("ca"));
    m_caFile->setVisible(false);

    appendItem(m_gateway);
    appendItem(m_authTypeChooser);
    appendItem(m_caFile);

    m_gateway->textEdit()->installEventFilter(this);
    m_caFile->edit()->lineEdit()->installEventFilter(this);
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
    userCertFile->edit()->lineEdit()->installEventFilter(this);

    FileChooseWidget *priKeyFile = new FileChooseWidget(this);
    priKeyFile->setTitle(tr("Private Key"));
    priKeyFile->edit()->setText(m_dataMap.value("key"));
    priKeyFile->edit()->lineEdit()->installEventFilter(this);

    ComboxWidget *certPasswordFlagsChooser = new ComboxWidget(this);
    certPasswordFlagsChooser->setTitle(tr("Pwd Options"));
    m_currentCertPasswordType = NetworkManager::Setting::SecretFlagType::None;
    QString curCertPasswordOption = PasswordFlagsStrMap.at(0).first;
    for (auto it = PasswordFlagsStrMap.cbegin(); it != PasswordFlagsStrMap.cend(); ++it) {
        certPasswordFlagsChooser->comboBox()->addItem(it->first, it->second);
        if (it->second == m_dataMap.value("cert-pass-flags").toInt()) {
            m_currentCertPasswordType = it->second;
            curCertPasswordOption = it->first;
        }
    }
    certPasswordFlagsChooser->setCurrentText(curCertPasswordOption);

    LineEditWidget *priKeyPassword = new LineEditWidget(true, this);
    priKeyPassword->setTitle(tr("Private Pwd"));
    priKeyPassword->setText(m_secretMap.value("cert-pass"));
    priKeyPassword->setPlaceholderText(tr("Required"));
    priKeyPassword->textEdit()->installEventFilter(this);

    connect(certPasswordFlagsChooser, &ComboxWidget::dataChanged, this, [ = ](const QVariant &data) {
        m_currentCertPasswordType = data.value<NetworkManager::Setting::SecretFlagType>();
        priKeyPassword->setVisible(m_currentCertPasswordType == NetworkManager::Setting::SecretFlagType::None);
    });
    connect(userCertFile, &FileChooseWidget::requestFrameKeepAutoHide, this, &VpnOpenVPNSection::requestFrameAutoHide);
    connect(priKeyFile, &FileChooseWidget::requestFrameKeepAutoHide, this, &VpnOpenVPNSection::requestFrameAutoHide);
    connect(certPasswordFlagsChooser, &ComboxWidget::onIndexChanged, this, &VpnOpenVPNSection::editClicked);
    connect(userCertFile->edit()->lineEdit(), &QLineEdit::textChanged, this, &VpnOpenVPNSection::editClicked);
    connect(priKeyFile->edit()->lineEdit(), &QLineEdit::textChanged, this, &VpnOpenVPNSection::editClicked);

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
    userName->textEdit()->installEventFilter(this);

    ComboxWidget *passwordFlagsChooser = new ComboxWidget(this);
    passwordFlagsChooser->setTitle(tr("Pwd Options"));
    m_currentPasswordType = NetworkManager::Setting::SecretFlagType::None;
    QString curPasswordOption = PasswordFlagsStrMap.at(0).first;
    for (auto it = PasswordFlagsStrMap.cbegin(); it != PasswordFlagsStrMap.cend(); ++it) {
        passwordFlagsChooser->comboBox()->addItem(it->first, it->second);
        if (it->second == m_dataMap.value("password-flags").toInt()) {
            m_currentPasswordType = it->second;
            curPasswordOption = it->first;
        }
    }
    passwordFlagsChooser->setCurrentText(curPasswordOption);

    LineEditWidget *password = new LineEditWidget(true, this);
    password->setTitle(tr("Password"));
    password->setText(m_secretMap.value("password"));
    password->setPlaceholderText(tr("Required"));
    password->textEdit()->installEventFilter(this);
    if (!m_secretMap.value("password").isEmpty() && DGuiApplicationHelper::isTabletEnvironment()) {
        static_cast<DPasswordEdit *>(password->dTextEdit())->setEchoButtonIsVisible(false);
    }

    connect(passwordFlagsChooser, &ComboxWidget::dataChanged, this, [ = ](const QVariant &data) {
        m_currentPasswordType = data.value<NetworkManager::Setting::SecretFlagType>();
        password->setVisible(m_currentPasswordType == NetworkManager::Setting::SecretFlagType::None);
    });
    connect(passwordFlagsChooser, &ComboxWidget::onIndexChanged, this, &VpnOpenVPNSection::editClicked);

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
    staticKey->edit()->lineEdit()->installEventFilter(this);

    SwitchWidget *customizeKeyDirection = new SwitchWidget(this);
    customizeKeyDirection->setTitle(tr("Customize Key Direction"));
    customizeKeyDirection->setChecked(m_dataMap.keys().contains("static-key-direction"));

    ComboxWidget *keyDirectionChooser = new ComboxWidget(this);
    keyDirectionChooser->setTitle(tr("Key Direction"));
    keyDirectionChooser->comboBox()->addItem("0", "0");
    keyDirectionChooser->comboBox()->addItem("1", "1");
    keyDirectionChooser->setCurrentText(m_dataMap.value("static-key-direction", "0"));
    keyDirectionChooser->setVisible(customizeKeyDirection->checked());

    LineEditWidget *remoteIp = new LineEditWidget(this);
    remoteIp->setTitle(tr("Remote IP"));
    remoteIp->setPlaceholderText(tr("Required"));
    remoteIp->setText(m_dataMap.value("remote-ip"));
    remoteIp->textEdit()->installEventFilter(this);

    LineEditWidget *localIp = new LineEditWidget(this);
    localIp->setTitle(tr("Local IP"));
    localIp->setPlaceholderText(tr("Required"));
    localIp->setText(m_dataMap.value("local-ip"));
    localIp->textEdit()->installEventFilter(this);

    connect(customizeKeyDirection, &SwitchWidget::checkedChanged,
            keyDirectionChooser, &ComboxWidget::setVisible);
    connect(keyDirectionChooser, &ComboxWidget::dataChanged, this, [ = ](const QVariant &data) {
        m_currentKeyDirection = data.toString();
    });
    connect(staticKey, &FileChooseWidget::requestFrameKeepAutoHide, this, &VpnOpenVPNSection::requestFrameAutoHide);
    connect(customizeKeyDirection, &SwitchWidget::checkedChanged, this, &VpnOpenVPNSection::editClicked);
    connect(keyDirectionChooser, &ComboxWidget::onIndexChanged, this, &VpnOpenVPNSection::editClicked);
    connect(staticKey->edit()->lineEdit(), &QLineEdit::textChanged, this, &VpnOpenVPNSection::editClicked);

    QList<SettingsItem *> itemList;
    itemList << staticKey << customizeKeyDirection << keyDirectionChooser << remoteIp << localIp;

    for (auto item : itemList) {
        appendItem(item);
    }

    m_settingItemsMap.insert("static-key", itemList);
}

void VpnOpenVPNSection::initConnection()
{
    connect(m_authTypeChooser, &ComboxWidget::dataChanged, this, [ = ](const QVariant &data) {
        qDebug() << "dataChanged is :" << data.toString();
        onAuthTypeChanged(data.toString());
    });
    connect(m_caFile, &FileChooseWidget::requestFrameKeepAutoHide, this, &VpnOpenVPNSection::requestFrameAutoHide);

    connect(m_authTypeChooser, &ComboxWidget::onIndexChanged, this, &VpnOpenVPNSection::editClicked);
    connect(m_caFile->edit()->lineEdit(), &QLineEdit::textChanged, this, &VpnOpenVPNSection::editClicked);
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
        ComboxWidget *tlsPasswordFlagsChooser = static_cast<ComboxWidget *>(itemsList.at(2));
        Q_EMIT tlsPasswordFlagsChooser->dataChanged(m_currentCertPasswordType);
    } else if (itemsType == "password") {
        ComboxWidget *passwordFlagsChooser = static_cast<ComboxWidget *>(itemsList.at(1));
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
    LineEditWidget *priKeyPassword = static_cast<LineEditWidget *>(itemsList.at(3));

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
    LineEditWidget *password = static_cast<LineEditWidget *>(itemsList.at(2));

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
    LineEditWidget *priKeyPassword = static_cast<LineEditWidget *>(itemsList.at(3));

    m_dataMap.insert("cert", userCertFile->edit()->text());
    m_dataMap.insert("key", priKeyFile->edit()->text());
    m_dataMap.insert("cert-pass-flags", QString::number(m_currentCertPasswordType));

    m_secretMap.insert("cert-pass", priKeyPassword->text());
}

void VpnOpenVPNSection::savePasswordItems()
{
    const QList<SettingsItem *> &itemsList = m_settingItemsMap.value("password");
    LineEditWidget *userName = static_cast<LineEditWidget *>(itemsList.at(0));
    LineEditWidget *password = static_cast<LineEditWidget *>(itemsList.at(2));

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

bool VpnOpenVPNSection::eventFilter(QObject *watched, QEvent *event)
{
    // 实现鼠标点击编辑框，确定按钮激活，统一网络模块处理，捕捉FocusIn消息
    if (event->type() == QEvent::FocusIn) {
        if (dynamic_cast<QLineEdit *>(watched)) {
            Q_EMIT editClicked();
        }
    }
    return QWidget::eventFilter(watched, event);
}
