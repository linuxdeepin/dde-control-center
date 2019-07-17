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

#include "secret8021xsection.h"
#include "widgets/filechoosewidget.h"
#include "widgets/contentwidget.h"

using namespace dcc::network;
using namespace NetworkManager;
using namespace dcc::widgets;

Secret8021xSection::Secret8021xSection(NetworkManager::Security8021xSetting::Ptr sSetting, QFrame *parent)
    : AbstractSection(tr("Security"), parent),
      m_eapMethmodChooser(new ComboBoxWidget(this)),
      m_passwordFlagsChooser(new ComboBoxWidget(this)),
      m_identity(new LineEditWidget(this)),
      m_password(new PasswdEditWidget(this)),
      m_enableWatcher(nullptr),
      m_currentPasswordType(NetworkManager::Setting::None),
      m_secretSetting(sSetting)
{
    initStrMaps();

    // init eapMethod
    const QList<NetworkManager::Security8021xSetting::EapMethod> &eapMethods = m_secretSetting->eapMethods();
    m_currentEapMethod = eapMethods.isEmpty() ? NetworkManager::Security8021xSetting::EapMethodTls : eapMethods.first();

    // init password type
    NetworkManager::Setting::SecretFlags passwordFlags = m_secretSetting->passwordFlags();
    for (auto i : PasswordFlagsStrMap.values()) {
        if (passwordFlags.testFlag(i)) {
            m_currentPasswordType = i;
            break;
        }
    }
}

Secret8021xSection::~Secret8021xSection()
{
}

bool Secret8021xSection::allInputValid()
{
    bool valid = true;

    if (m_enableWatcher->secretEnabled()) {
        valid = commonItemsInpuValid();

        // md5, leap has no special item
        switch (m_currentEapMethod) {
            case NetworkManager::Security8021xSetting::EapMethodTls: {
                if (!tlsItemsInputValid()) {
                    valid = false;
                }
                break;
            }
            case NetworkManager::Security8021xSetting::EapMethodFast: {
                if (!fastItemsInputValid()) {
                    valid = false;
                }
                break;
            }
            case NetworkManager::Security8021xSetting::EapMethodTtls: {
                if (!ttlsItemsInputValid()) {
                    valid = false;
                }
                break;
            }
            case NetworkManager::Security8021xSetting::EapMethodPeap: {
                if (!peapItemsInputValid()) {
                    valid = false;
                }
                break;
            }
            default:
                break;
        }
    }

    return valid;
}

void Secret8021xSection::saveSettings()
{
    if (!m_enableWatcher->secretEnabled()) {
        m_secretSetting->setEapMethods(QList<NetworkManager::Security8021xSetting::EapMethod>());
        m_secretSetting->setInitialized(false);
        return;
    }

    saveCommonItems();

    switch (m_currentEapMethod) {
        case NetworkManager::Security8021xSetting::EapMethodTls:
            saveTlsItems();
            break;
        case NetworkManager::Security8021xSetting::EapMethodFast:
            saveFastItems();
            break;
        case NetworkManager::Security8021xSetting::EapMethodTtls:
            saveTtlsItems();
            break;
        case NetworkManager::Security8021xSetting::EapMethodPeap:
            savePeapItems();
            break;
        default:
            break;
    }

    m_secretSetting->setInitialized(true);
}

void Secret8021xSection::init(Secret8021xEnableWatcher *watcher,
        QList<NetworkManager::Security8021xSetting::EapMethod> eapMethodsSupportList)
{
    if (m_enableWatcher) {
        qDebug() << "Secret8021x enable watcher has been initialized";
        return;
    }

    m_enableWatcher = watcher;
    m_eapMethodsWantedList = eapMethodsSupportList;

    initUI();
    initConnection();

    onSecretEnableChanged(m_enableWatcher->secretEnabled());
    onEapMethodChanged(m_currentEapMethod);
    onPasswordFlagsChanged(m_currentPasswordType);
}

void Secret8021xSection::initStrMaps()
{
    EapMethodStrMap = {
        {tr("TLS"), NetworkManager::Security8021xSetting::EapMethodTls},
        {tr("MD5"), NetworkManager::Security8021xSetting::EapMethodMd5},
        {tr("LEAP"), NetworkManager::Security8021xSetting::EapMethodLeap},
        {tr("FAST"), NetworkManager::Security8021xSetting::EapMethodFast},
        {tr("Tunneled TLS"), NetworkManager::Security8021xSetting::EapMethodTtls},
        {tr("Protected EAP"), NetworkManager::Security8021xSetting::EapMethodPeap}
    };

    PasswordFlagsStrMap = {
        //{tr("Saved"), NetworkManager::Setting::AgentOwned},
        {tr("Saved"), NetworkManager::Setting::None},
        {tr("Ask"), NetworkManager::Setting::NotSaved},
        {tr("Not Required"), NetworkManager::Setting::NotRequired}
    };

    FastrProvisioningStrMap = {
        {tr("Disabled"), NetworkManager::Security8021xSetting::FastProvisioningDisabled},
        {tr("Anonymous"), NetworkManager::Security8021xSetting::FastProvisioningAllowUnauthenticated},
        {tr("Authenticated"), NetworkManager::Security8021xSetting::FastProvisioningAllowAuthenticated},
        {tr("Both"), NetworkManager::Security8021xSetting::FastProvisioningAllowBoth}
    };

    AuthMethodStrMapFast = {
        {"GTC", NetworkManager::Security8021xSetting::AuthMethodGtc},
        {"MSCHAPV2", NetworkManager::Security8021xSetting::AuthMethodMschapv2}
    };

    AuthMethodStrMapTtls = {
        {"PAP", NetworkManager::Security8021xSetting::AuthMethodPap},
        {"MSCHAP", NetworkManager::Security8021xSetting::AuthMethodMschap},
        {"MSCHAPV2", NetworkManager::Security8021xSetting::AuthMethodMschapv2},
        {"CHAP", NetworkManager::Security8021xSetting::AuthMethodChap}
    };

    PeapVersionStrMap = {
        {tr("Automatic"), NetworkManager::Security8021xSetting::PeapVersionUnknown},
        {tr("Version 0"), NetworkManager::Security8021xSetting::PeapVersionZero},
        {tr("Version 1"), NetworkManager::Security8021xSetting::PeapVersionOne}
    };

    AuthMethodStrMapPeap = {
        {"GTC", NetworkManager::Security8021xSetting::AuthMethodGtc},
        {"MD5", NetworkManager::Security8021xSetting::AuthMethodMd5},
        {"MSCHAPV2", NetworkManager::Security8021xSetting::AuthMethodMschapv2}
    };
}

void Secret8021xSection::initUI()
{
    m_eapMethmodChooser->setTitle(tr("EAP Auth"));
    for (auto eapMethodWanted : m_eapMethodsWantedList) {
        const QString &eapMethodStr = EapMethodStrMap.key(eapMethodWanted);
        if (eapMethodStr.isEmpty()) {
            qDebug() << "EAP method do not supported, removing it:" << eapMethodWanted;
            m_eapMethodsWantedList.removeAll(eapMethodWanted);
            continue;
        }
        m_eapMethmodChooser->appendOption(eapMethodStr, eapMethodWanted);
    }
    m_eapMethmodChooser->setCurrent(m_currentEapMethod);

    m_identity->setTitle(tr("Identity"));
    m_identity->setText(m_secretSetting->identity());
    m_identity->setPlaceholderText(tr("Required"));

    m_passwordFlagsChooser->setTitle(tr("Pwd Options"));
    for (const QString &key : PasswordFlagsStrMap.keys()) {
        m_passwordFlagsChooser->appendOption(key, PasswordFlagsStrMap.value(key));
    }
    m_passwordFlagsChooser->setCurrent(m_currentPasswordType);

    m_password->setPlaceholderText(tr("Required"));
    if (m_currentEapMethod == NetworkManager::Security8021xSetting::EapMethodTls) {
        m_password->setText(m_secretSetting->privateKeyPassword());
    } else {
        m_password->setText(m_secretSetting->password());
    }

    appendItem(m_eapMethmodChooser);
    appendItem(m_identity);
    appendItem(m_passwordFlagsChooser);
    appendItem(m_password);
}

void Secret8021xSection::initConnection()
{
    connect(m_identity->textEdit(), &QLineEdit::editingFinished, this, &Secret8021xSection::allInputValid);
    connect(m_identity->textEdit(), &QLineEdit::editingFinished, this, &Secret8021xSection::saveUserInputIdentify);

    connect(m_password->textEdit(), &QLineEdit::editingFinished, this, &Secret8021xSection::allInputValid);
    connect(m_password->textEdit(), &QLineEdit::editingFinished, this, &Secret8021xSection::saveUserInputPassword);

    connect(m_enableWatcher, &Secret8021xEnableWatcher::secretEnableChanged, this, &Secret8021xSection::onSecretEnableChanged);

    connect(m_eapMethmodChooser, &ComboBoxWidget::requestPage, this, &Secret8021xSection::requestNextPage);
    connect(m_eapMethmodChooser, &ComboBoxWidget::dataChanged, this, [=](const QVariant &data) {
        onEapMethodChanged(data.value<NetworkManager::Security8021xSetting::EapMethod>());
    });

    connect(m_passwordFlagsChooser, &ComboBoxWidget::requestPage, this, &Secret8021xSection::requestNextPage);
    connect(m_passwordFlagsChooser, &ComboBoxWidget::dataChanged, this, [=](const QVariant &data) {
        onPasswordFlagsChanged(data.value<NetworkManager::Setting::SecretFlagType>());
    });
}

void Secret8021xSection::initEapItems(NetworkManager::Security8021xSetting::EapMethod method)
{
    if (m_eapMethodItemsMap.contains(method)) {
        return;
    }

    QList<SettingsItem *> itemList;

    switch (method) {
        case NetworkManager::Security8021xSetting::EapMethodTls: {
            initEapMethodTlsItems(&itemList);
            break;
        }
        case NetworkManager::Security8021xSetting::EapMethodMd5: {
            // md5 has no special item
            break;
        }
        case NetworkManager::Security8021xSetting::EapMethodLeap: {
            // leap has no special item
            break;
        }
        case NetworkManager::Security8021xSetting::EapMethodFast: {
            initEapMethodFastItems(&itemList);
            break;
        }
        case NetworkManager::Security8021xSetting::EapMethodTtls: {
            initEapMethodTtlsItems(&itemList);
            break;
        }
        case NetworkManager::Security8021xSetting::EapMethodPeap: {
            initEapMethodPeapItems(&itemList);
            break;
        }
        default:
            qDebug() << "init EapItems failed! unhandled EapMethod" << method;
            break;
    }

    m_eapMethodItemsMap.insert(method, itemList);
}

void Secret8021xSection::initEapMethodTlsItems(QList<SettingsItem *> *itemList)
{
    FileChooseWidget *privateKey = new FileChooseWidget(this);
    privateKey->setTitle(tr("Private Key"));
    privateKey->edit()->setText(m_secretSetting->privateKey());

    FileChooseWidget *caCert = new FileChooseWidget(this);
    caCert->setTitle(tr("CA Cert"));
    caCert->edit()->setText(m_secretSetting->caCertificate());

    FileChooseWidget *userCert = new FileChooseWidget(this);
    userCert->setTitle(tr("User Cert"));
    userCert->edit()->setText(m_secretSetting->clientCertificate());

    connect(privateKey->edit(), &QLineEdit::textChanged, this, &Secret8021xSection::allInputValid);
    connect(userCert->edit(), &QLineEdit::textChanged, this, &Secret8021xSection::allInputValid);

    connect(privateKey, &FileChooseWidget::requestFrameKeepAutoHide, this, &Secret8021xSection::requestFrameAutoHide);
    connect(caCert, &FileChooseWidget::requestFrameKeepAutoHide, this, &Secret8021xSection::requestFrameAutoHide);
    connect(userCert, &FileChooseWidget::requestFrameKeepAutoHide, this, &Secret8021xSection::requestFrameAutoHide);

    appendItem(privateKey);
    appendItem(caCert);
    appendItem(userCert);

    itemList->append(privateKey);
    itemList->append(caCert);
    itemList->append(userCert);
}

void Secret8021xSection::initEapMethodFastItems(QList<SettingsItem *> *itemList)
{
    LineEditWidget *anonymousID = new LineEditWidget(this);
    anonymousID->setTitle(tr("Anonymous ID"));
    anonymousID->setText(m_secretSetting->anonymousIdentity());

    ComboBoxWidget *provisioning = new ComboBoxWidget(this);
    provisioning->setTitle(tr("Provisioning"));
    for (const QString &key : FastrProvisioningStrMap.keys()) {
        provisioning->appendOption(key, FastrProvisioningStrMap.value(key));
    }
    if (m_secretSetting->phase1FastProvisioning() ==
            NetworkManager::Security8021xSetting::FastProvisioning::FastProvisioningUnknown) {
        provisioning->setCurrent(FastrProvisioningStrMap.first());
    } else {
        provisioning->setCurrent(m_secretSetting->phase1FastProvisioning());
    }

    FileChooseWidget *pacFile = new FileChooseWidget(this);
    pacFile->setTitle(tr("PAC file"));
    pacFile->edit()->setText(m_secretSetting->pacFile());

    ComboBoxWidget *authMethod = new ComboBoxWidget(this);
    authMethod->setTitle(tr("Inner Auth"));
    for (const QString &key : AuthMethodStrMapFast.keys()) {
        authMethod->appendOption(key, AuthMethodStrMapFast.value(key));
    }
    if (m_secretSetting->phase2AuthMethod() == NetworkManager::Security8021xSetting::AuthMethod::AuthMethodUnknown) {
        authMethod->setCurrent(AuthMethodStrMapFast.first());
    } else if (AuthMethodStrMapFast.values().contains(m_secretSetting->phase2AuthMethod())) {
        authMethod->setCurrent(m_secretSetting->phase2AuthMethod());
    } else {
        authMethod->setCurrent(AuthMethodStrMapFast.first());
    }

    connect(provisioning, &ComboBoxWidget::requestPage, this, &Secret8021xSection::requestNextPage);
    connect(authMethod, &ComboBoxWidget::requestPage, this, &Secret8021xSection::requestNextPage);

    connect(pacFile, &FileChooseWidget::requestFrameKeepAutoHide, this, &Secret8021xSection::requestFrameAutoHide);

    appendItem(anonymousID);
    appendItem(provisioning);
    appendItem(pacFile);
    appendItem(authMethod);

    itemList->append(anonymousID);
    itemList->append(provisioning);
    itemList->append(pacFile);
    itemList->append(authMethod);
}

void Secret8021xSection::initEapMethodTtlsItems(QList<SettingsItem *> *itemList)
{
    LineEditWidget *anonymousID = new LineEditWidget(this);
    anonymousID->setTitle(tr("Anonymous ID"));
    anonymousID->setText(m_secretSetting->anonymousIdentity());

    FileChooseWidget *caCert = new FileChooseWidget(this);
    caCert->setTitle(tr("CA Cert"));
    caCert->edit()->setText(m_secretSetting->caCertificate());

    ComboBoxWidget *authMethod = new ComboBoxWidget(this);
    authMethod->setTitle(tr("Inner Auth"));
    for (const QString &key : AuthMethodStrMapTtls.keys()) {
        authMethod->appendOption(key, AuthMethodStrMapTtls.value(key));
    }
    if (m_secretSetting->phase2AuthMethod() == NetworkManager::Security8021xSetting::AuthMethod::AuthMethodUnknown) {
        authMethod->setCurrent(AuthMethodStrMapTtls.first());
    } else if (AuthMethodStrMapTtls.values().contains(m_secretSetting->phase2AuthMethod())) {
        authMethod->setCurrent(m_secretSetting->phase2AuthMethod());
    }
    else {
        authMethod->setCurrent(AuthMethodStrMapTtls.first());
    }

    connect(authMethod, &ComboBoxWidget::requestPage, this, &Secret8021xSection::requestNextPage);
    connect(caCert, &FileChooseWidget::requestFrameKeepAutoHide, this, &Secret8021xSection::requestFrameAutoHide);

    appendItem(anonymousID);
    appendItem(caCert);
    appendItem(authMethod);

    itemList->append(anonymousID);
    itemList->append(caCert);
    itemList->append(authMethod);
}

void Secret8021xSection::initEapMethodPeapItems(QList<SettingsItem *> *itemList)
{
    LineEditWidget *anonymousID = new LineEditWidget(this);
    anonymousID->setTitle(tr("Anonymous ID"));
    anonymousID->setText(m_secretSetting->anonymousIdentity());

    FileChooseWidget *caCert = new FileChooseWidget(this);
    caCert->setTitle(tr("CA Cert"));
    caCert->edit()->setText(m_secretSetting->caCertificate());

    ComboBoxWidget *peapVersion = new ComboBoxWidget(this);
    peapVersion->setTitle(tr("PEAP Version"));
    for (const QString &key : PeapVersionStrMap.keys()) {
        peapVersion->appendOption(key, PeapVersionStrMap.value(key));
    }
    if (m_secretSetting->phase1PeapVersion() == NetworkManager::Security8021xSetting::PeapVersion::PeapVersionUnknown) {
        peapVersion->setCurrent(PeapVersionStrMap.first());
    } else {
        peapVersion->setCurrent(m_secretSetting->phase1PeapVersion());
    }

    ComboBoxWidget *authMethod = new ComboBoxWidget(this);
    authMethod->setTitle(tr("Inner Auth"));
    for (const QString &key : AuthMethodStrMapPeap.keys()) {
        authMethod->appendOption(key, AuthMethodStrMapPeap.value(key));
    }
    if (m_secretSetting->phase2AuthMethod() == NetworkManager::Security8021xSetting::AuthMethod::AuthMethodUnknown) {
        authMethod->setCurrent(AuthMethodStrMapPeap.first());
    } else if (AuthMethodStrMapPeap.values().contains(m_secretSetting->phase2AuthMethod())) {
        authMethod->setCurrent(m_secretSetting->phase2AuthMethod());
    }
    else {
        authMethod->setCurrent(AuthMethodStrMapPeap.first());
    }

    connect(peapVersion, &ComboBoxWidget::requestPage, this, &Secret8021xSection::requestNextPage);
    connect(authMethod, &ComboBoxWidget::requestPage, this, &Secret8021xSection::requestNextPage);

    connect(caCert, &FileChooseWidget::requestFrameKeepAutoHide, this, &Secret8021xSection::requestFrameAutoHide);

    appendItem(anonymousID);
    appendItem(caCert);
    appendItem(peapVersion);
    appendItem(authMethod);

    itemList->append(anonymousID);
    itemList->append(caCert);
    itemList->append(peapVersion);
    itemList->append(authMethod);
}

void Secret8021xSection::onSecretEnableChanged(const bool enable)
{
    m_eapMethmodChooser->setVisible(enable);
    m_identity->setVisible(enable);
    m_passwordFlagsChooser->setVisible(enable);
    m_password->setVisible(enable);

    // set current eap method items visible
    if (!m_eapMethodItemsMap.contains(m_currentEapMethod)) {
        initEapItems(m_currentEapMethod);
    }
    for (auto i : m_eapMethodItemsMap.value(m_currentEapMethod)) {
        i->setVisible(enable);
    }
}

void Secret8021xSection::onEapMethodChanged(NetworkManager::Security8021xSetting::EapMethod method)
{
    if (!m_eapMethodItemsMap.contains(m_currentEapMethod)) {
        initEapItems(m_currentEapMethod);
    }
    if (!m_eapMethodItemsMap.contains(method)) {
        initEapItems(method);
    }

    if (m_currentEapMethod != method) {
        // hide old eap method items
        for (auto i : m_eapMethodItemsMap.value(m_currentEapMethod)) {
            i->setVisible(false);
        }

        m_currentEapMethod = method;

        // show new eap method items
        for (auto i : m_eapMethodItemsMap.value(m_currentEapMethod)) {
            i->setVisible(true);
        }
    }

    if (m_userInputIdentifyMap.contains(m_currentEapMethod)) {
        m_identity->setText(m_userInputIdentifyMap.value(m_currentEapMethod));
    } else {
        m_identity->setText(m_secretSetting->identity());
    }

    if (m_userInputPasswordMap.contains(m_currentEapMethod)) {
        m_password->setText(m_userInputPasswordMap.value(m_currentEapMethod));
    } else {
        if (m_currentEapMethod == NetworkManager::Security8021xSetting::EapMethodTls) {
            m_password->setText(m_secretSetting->privateKeyPassword());
        } else {
            m_password->setText(m_secretSetting->password());
        }
    }

    if (m_currentEapMethod == NetworkManager::Security8021xSetting::EapMethodTls) {
        m_password->setTitle(tr("Private Pwd"));
    } else {
        m_password->setTitle(tr("Password"));
    }
}

void Secret8021xSection::onPasswordFlagsChanged(NetworkManager::Setting::SecretFlagType type)
{
    m_currentPasswordType = type;
    if (m_enableWatcher->secretEnabled()) {
        m_password->setVisible(m_currentPasswordType == NetworkManager::Setting::None);
    }
}

void Secret8021xSection::saveUserInputIdentify()
{
    m_userInputIdentifyMap.insert(m_currentEapMethod, m_identity->text());
}

void Secret8021xSection::saveUserInputPassword()
{
    m_userInputPasswordMap.insert(m_currentEapMethod, m_password->text());
}

bool Secret8021xSection::commonItemsInpuValid()
{
    bool valid = true;

    if (m_identity->text().isEmpty()) {
        valid = false;
        m_identity->setIsErr(true);
    } else {
        m_identity->setIsErr(false);
    }

    if (m_currentPasswordType == NetworkManager::Setting::None) {
        if (m_password->text().isEmpty()) {
            valid = false;
            m_password->setIsErr(true);
        } else {
            m_password->setIsErr(false);
        }
    }

    return valid;
}

bool Secret8021xSection::tlsItemsInputValid()
{
    bool valid = true;
    const QList<SettingsItem *> &itemsList = m_eapMethodItemsMap.value(NetworkManager::Security8021xSetting::EapMethodTls);

    FileChooseWidget *privateKey = static_cast<FileChooseWidget *>(itemsList.at(0));
    if (privateKey->edit()->text().isEmpty()) {
        valid = false;
        privateKey->setIsErr(true);
    } else {
        privateKey->setIsErr(false);
    }

    FileChooseWidget *userCert = static_cast<FileChooseWidget *>(itemsList.at(2));
    if (userCert->edit()->text().isEmpty()) {
        valid = false;
        userCert->setIsErr(true);
    } else {
        userCert->setIsErr(false);
    }

    return valid;
}

bool Secret8021xSection::fastItemsInputValid()
{
    // nothing to check?
    return true;
}

bool Secret8021xSection::ttlsItemsInputValid()
{
    // nothing to check?
    return true;
}

bool Secret8021xSection::peapItemsInputValid()
{
    // nothing to check?
    return true;
}

void Secret8021xSection::saveCommonItems()
{
    m_secretSetting->setEapMethods(QList<NetworkManager::Security8021xSetting::EapMethod>() << m_currentEapMethod);
    m_secretSetting->setIdentity(m_identity->text());

    if (m_currentEapMethod == NetworkManager::Security8021xSetting::EapMethodTls) {
        m_secretSetting->setPrivateKeyPasswordFlags(m_currentPasswordType);
        if (m_currentPasswordType == NetworkManager::Setting::None) {
            m_secretSetting->setPrivateKeyPassword(m_password->text());
        } else {
            m_secretSetting->setPrivateKeyPassword(QString());
        }
    } else {
        m_secretSetting->setPasswordFlags(m_currentPasswordType);
        if (m_currentPasswordType == NetworkManager::Setting::None) {
            m_secretSetting->setPassword(m_password->text());
        } else {
            m_secretSetting->setPassword(QString());
        }
    }
}

// 转成networkmanager所需要的文件uri格式
static QByteArray formatFileUriForNMPath(const QString &localFilePath)
{
    if (localFilePath.isEmpty())
        return QByteArray();

    QByteArray uri;

    if (!localFilePath.startsWith("file://")) {
        uri.append("file://");
    }

    uri.append(QFile::encodeName(localFilePath));

    return uri.append('\0');
}

void Secret8021xSection::saveTlsItems()
{
    const QList<SettingsItem *> &itemsList = m_eapMethodItemsMap.value(NetworkManager::Security8021xSetting::EapMethodTls);
    FileChooseWidget *privateKey = static_cast<FileChooseWidget *>(itemsList.at(0));
    FileChooseWidget *caCert = static_cast<FileChooseWidget *>(itemsList.at(1));
    FileChooseWidget *userCert = static_cast<FileChooseWidget *>(itemsList.at(2));

    m_secretSetting->setPrivateKey(formatFileUriForNMPath(privateKey->edit()->text()));
    m_secretSetting->setCaCertificate(formatFileUriForNMPath(caCert->edit()->text()));
    m_secretSetting->setClientCertificate(formatFileUriForNMPath(userCert->edit()->text()));
}

void Secret8021xSection::saveFastItems()
{
    const QList<SettingsItem *> &itemsList = m_eapMethodItemsMap.value(NetworkManager::Security8021xSetting::EapMethodFast);
    LineEditWidget *anonymousID = static_cast<LineEditWidget *>(itemsList.at(0));
    ComboBoxWidget *provisioning = static_cast<ComboBoxWidget *>(itemsList.at(1));
    FileChooseWidget *pacFile = static_cast<FileChooseWidget *>(itemsList.at(2));
    ComboBoxWidget *authMethod = static_cast<ComboBoxWidget *>(itemsList.at(3));

    m_secretSetting->setAnonymousIdentity(anonymousID->text());
    m_secretSetting->setPhase1FastProvisioning(FastrProvisioningStrMap.value(provisioning->value()));
    m_secretSetting->setPacFile(pacFile->edit()->text());
    m_secretSetting->setPhase2AuthMethod(AuthMethodStrMapFast.value(authMethod->value()));
}

void Secret8021xSection::saveTtlsItems()
{
    const QList<SettingsItem *> &itemsList = m_eapMethodItemsMap.value(NetworkManager::Security8021xSetting::EapMethodTtls);
    LineEditWidget *anonymousID = static_cast<LineEditWidget *>(itemsList.at(0));
    FileChooseWidget *caCert = static_cast<FileChooseWidget *>(itemsList.at(1));
    ComboBoxWidget *authMethod = static_cast<ComboBoxWidget *>(itemsList.at(2));

    m_secretSetting->setAnonymousIdentity(anonymousID->text());
    m_secretSetting->setCaCertificate(formatFileUriForNMPath(caCert->edit()->text()));
    m_secretSetting->setPhase2AuthMethod(AuthMethodStrMapTtls.value(authMethod->value()));
}

void Secret8021xSection::savePeapItems()
{
    const QList<SettingsItem *> &itemsList = m_eapMethodItemsMap.value(NetworkManager::Security8021xSetting::EapMethodPeap);
    LineEditWidget *anonymousID = static_cast<LineEditWidget *>(itemsList.at(0));
    FileChooseWidget *caCert = static_cast<FileChooseWidget *>(itemsList.at(1));
    ComboBoxWidget *peapVersion = static_cast<ComboBoxWidget *>(itemsList.at(2));
    ComboBoxWidget *authMethod = static_cast<ComboBoxWidget *>(itemsList.at(3));

    m_secretSetting->setAnonymousIdentity(anonymousID->text());
    m_secretSetting->setCaCertificate(formatFileUriForNMPath(caCert->edit()->text()));
    m_secretSetting->setPhase1PeapVersion(PeapVersionStrMap.value(peapVersion->value()));
    m_secretSetting->setPhase2AuthMethod(AuthMethodStrMapPeap.value(authMethod->value()));
}
