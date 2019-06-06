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

#include "connectioneditpage.h"
#include "widgets/translucentframe.h"

#include "settings/wiredsettings.h"
#include "settings/wirelesssettings.h"
#include "settings/dslpppoesettings.h"

#include <networkmanagerqt/settings.h>
#include <networkmanagerqt/security8021xsetting.h>
#include <networkmanagerqt/wirelesssecuritysetting.h>
#include <networkmanagerqt/pppoesetting.h>
#include <networkmanagerqt/vpnsetting.h>

using namespace dcc::widgets;
using namespace dcc::network;
using namespace NetworkManager;

static QString DevicePath = "";

ConnectionEditPage::ConnectionEditPage(ConnectionType connType,
                                       const QString &devPath,
                                       const QString &connUuid,
                                       QWidget *      parent)
    : ContentWidget(parent)
    , m_settingsLayout(new QVBoxLayout)
    , m_connection(nullptr)
    , m_connectionSettings(nullptr)
    , m_settingsWidget(nullptr)
    , m_mainLayout(new QVBoxLayout)
    , m_disconnectBtn(new QPushButton)
    , m_removeBtn(new QPushButton)
    , m_buttonTuple(new ButtonTuple)
    , m_subPage(nullptr)
    , m_connType(static_cast<NetworkManager::ConnectionSettings::ConnectionType>(connType))
    , m_isNewConnection(false)
    , m_connectionUuid(connUuid)
{
    DevicePath = devPath;

    initUI();

    if (m_connectionUuid.isEmpty()) {
        qDebug() << "connection uuid is empty, creating new ConnectionSettings...";
        createConnSettings();
        m_isNewConnection = true;
    } else {
        m_connection = findConnectionByUuid(m_connectionUuid);
        if (!m_connection) {
            qDebug() << "can't find connection by uuid:" << m_connectionUuid;
            return;
        }
        m_connectionSettings = m_connection->settings();
        m_isNewConnection = false;
        initConnectionSecrets();
    }

    initHeaderButtons();
    initConnection();
}

ConnectionEditPage::~ConnectionEditPage()
{
}

void ConnectionEditPage::initUI()
{
    m_settingsLayout->setSpacing(10);

    m_disconnectBtn->setText(tr("Disconnect"));
    m_disconnectBtn->setVisible(false);
    m_removeBtn->setText(tr("Delete"));
    m_removeBtn->setVisible(false);

    QPushButton *cancelBtn = m_buttonTuple->leftButton();
    QPushButton *acceptBtn = m_buttonTuple->rightButton();
    cancelBtn->setText(tr("Cancel"));
    acceptBtn->setText(tr("Save"));

    m_mainLayout->addSpacing(10);
    m_mainLayout->addWidget(m_disconnectBtn);
    m_mainLayout->addWidget(m_removeBtn);
    m_mainLayout->addLayout(m_settingsLayout);
    m_mainLayout->addWidget(m_buttonTuple);
    m_mainLayout->addStretch();
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(m_mainLayout);

    setContent(mainWidget);
}

void ConnectionEditPage::initHeaderButtons()
{
    if (m_isNewConnection) {
        return;
    }

    for (auto conn : activeConnections()) {
        if (conn->uuid() == m_connection->uuid()) {
            m_disconnectBtn->setVisible(true);
            m_disconnectBtn->setProperty("activeConnectionPath", conn->path());
            break;
        }
    }

    m_removeBtn->setVisible(true);
}

void ConnectionEditPage::initSettingsWidget()
{
    if (!m_connectionSettings) {
        return;
    }

    switch (m_connType) {
        case NetworkManager::ConnectionSettings::ConnectionType::Wired: {
            m_settingsWidget = new WiredSettings(m_connectionSettings, this);
            break;
        }
        case NetworkManager::ConnectionSettings::ConnectionType::Wireless: {
            m_settingsWidget = new WirelessSettings(m_connectionSettings, this);
            break;
        }
        case NetworkManager::ConnectionSettings::ConnectionType::Pppoe: {
            m_settingsWidget = new DslPppoeSettings(m_connectionSettings, this);
            break;
        }
        default:
            break;
    }

    connect(m_settingsWidget, &AbstractSettings::requestNextPage, this, &ConnectionEditPage::onRequestNextPage);
    connect(m_settingsWidget, &AbstractSettings::requestFrameAutoHide, this, &ConnectionEditPage::requestFrameAutoHide);

    m_settingsLayout->addWidget(m_settingsWidget);
}

const QString ConnectionEditPage::devicePath()
{
    return DevicePath;
}

void ConnectionEditPage::onDeviceRemoved()
{
    if (m_subPage) {
        Q_EMIT m_subPage->back();
    }

    Q_EMIT back();
}

void ConnectionEditPage::initConnection()
{
    connect(m_buttonTuple->rightButton(), &QPushButton::clicked, this, &ConnectionEditPage::saveConnSettings);
    connect(m_buttonTuple->leftButton(), &QPushButton::clicked, this, &ConnectionEditPage::back);
    connect(this, &ConnectionEditPage::saveSettingsDone, this, &ConnectionEditPage::prepareConnection);
    connect(this, &ConnectionEditPage::prepareConnectionDone, this, &ConnectionEditPage::updateConnection);

    connect(m_removeBtn, &QPushButton::clicked, this, [=]() {
        m_connection->remove();
        Q_EMIT back();
    });

    connect(m_disconnectBtn, &QPushButton::clicked, this, [=]() {
        deactivateConnection(m_disconnectBtn->property("activeConnectionPath").toString());
        Q_EMIT back();
    });
}

NMVariantMapMap ConnectionEditPage::secretsMapMapBySettingType(NetworkManager::Setting::SettingType settingType)
{
    QDBusPendingReply<NMVariantMapMap> reply;
    reply = m_connection->secrets(m_connectionSettings->setting(settingType)->name());

    reply.waitForFinished();
    if (reply.isError() || !reply.isValid()) {
        qDebug() << "get secrets error for connection:" << reply.error();
    }

    return reply.value();
}

template <typename T>
void ConnectionEditPage::setSecretsFromMapMap(NetworkManager::Setting::SettingType settingType, NMVariantMapMap secretsMapMap)
{
    QSharedPointer<T> setting = m_connectionSettings->setting(settingType).staticCast<T>();
    setting->secretsFromMap(secretsMapMap.value(setting->name()));
}

void ConnectionEditPage::onRequestNextPage(dcc::ContentWidget * const page)
{
    m_subPage = page;

    Q_EMIT requestNextPage(page);
}

void ConnectionEditPage::initConnectionSecrets()
{
    NetworkManager::Setting::SettingType sType;
    NMVariantMapMap sSecretsMapMap;

    switch (m_connType) {
        case NetworkManager::ConnectionSettings::ConnectionType::Wired: {
            sType = NetworkManager::Setting::SettingType::Security8021x;
            if (!m_connectionSettings->setting(sType).staticCast<NetworkManager::Security8021xSetting>()->eapMethods().isEmpty()) {
                sSecretsMapMap = secretsMapMapBySettingType(sType);
                setSecretsFromMapMap<NetworkManager::Security8021xSetting>(sType, sSecretsMapMap);
            }
            break;
        }
        case NetworkManager::ConnectionSettings::ConnectionType::Wireless: {
            sType = NetworkManager::Setting::SettingType::WirelessSecurity;

            NetworkManager::WirelessSecuritySetting::KeyMgmt keyMgmt =
                m_connectionSettings->setting(sType).staticCast<NetworkManager::WirelessSecuritySetting>()->keyMgmt();
            if (keyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaNone
                    || keyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::Unknown) {
                break;
            }

            if (keyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaEap) {
                sType = NetworkManager::Setting::SettingType::Security8021x;
            }
            sSecretsMapMap = secretsMapMapBySettingType(sType);
            setSecretsFromMapMap<NetworkManager::WirelessSecuritySetting>(sType, sSecretsMapMap);
            break;
        }
        case NetworkManager::ConnectionSettings::ConnectionType::Vpn: {
            sType = NetworkManager::Setting::SettingType::Vpn;
            sSecretsMapMap = secretsMapMapBySettingType(sType);
            setSecretsFromMapMap<NetworkManager::VpnSetting>(sType, sSecretsMapMap);
            break;
        }
        case NetworkManager::ConnectionSettings::ConnectionType::Pppoe: {
            sType = NetworkManager::Setting::SettingType::Pppoe;
            sSecretsMapMap = secretsMapMapBySettingType(sType);
            setSecretsFromMapMap<NetworkManager::PppoeSetting>(sType, sSecretsMapMap);
            break;
        }
        default:
            break;
    }
}

void ConnectionEditPage::saveConnSettings()
{
    if (!m_settingsWidget->allInputValid()) {
        return;
    }

    if (m_settingsWidget->isAutoConnect()) {
        // deactivate this device's ActiveConnection
        QDBusPendingReply<> reply;
        for (auto aConn : activeConnections()) {
            for (auto devPath : aConn->devices()) {
                if (devPath == DevicePath) {
                    reply = deactivateConnection(aConn->path());
                    reply.waitForFinished();
                    if (reply.isError()) {
                        qDebug() << "error occurred while deactivate connection" << reply.error();
                    }
                }
            }
        }
    }

    m_settingsWidget->saveSettings();

    Q_EMIT saveSettingsDone();
}

void ConnectionEditPage::prepareConnection()
{
    if (!m_connection) {
        qDebug() << "preparing connection...";
        QDBusPendingReply<QDBusObjectPath> reply = addConnection(m_connectionSettings->toMap());
        reply.waitForFinished();
        const QString &connPath = reply.value().path();
        m_connection = findConnection(connPath);
        if (!m_connection) {
            qDebug() << "create connection failed..." << reply.error();
            return;
        }
    }

    Q_EMIT prepareConnectionDone();
}

void ConnectionEditPage::updateConnection()
{
    QDBusPendingReply<> reply;

    // update function saves the settings on the hard disk
    reply = m_connection->update(m_connectionSettings->toMap());
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << "error occurred while updating the connection" << reply.error();
        return;
    }

    if (m_settingsWidget->isAutoConnect()) {
        reply = activateConnection(m_connection->path(), DevicePath, "");
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "error occurred while activate connection" << reply.error();
        }
    }

    Q_EMIT back();
}

void ConnectionEditPage::createConnSettings()
{
    m_connectionSettings = QSharedPointer<NetworkManager::ConnectionSettings>(
            new NetworkManager::ConnectionSettings(m_connType));

    // do not handle vpn name here
    QString connName;
    switch (m_connType) {
        case NetworkManager::ConnectionSettings::ConnectionType::Wired: {
            connName = tr("Wired Connection %1");
            break;
        }
        case NetworkManager::ConnectionSettings::ConnectionType::Wireless: {
            connName = tr("Wireless Connection %1");
            break;
        }
        case NetworkManager::ConnectionSettings::ConnectionType::Pppoe: {
            connName = tr("PPPoE Connection %1");
            break;
        }
        default:
            break;
    }

    if (!connName.isEmpty()) {
        m_connectionSettings->setId(connName.arg(connectionSuffixNum(connName)));
    }
    m_connectionSettings->setUuid(m_connectionSettings->createNewUuid());
}

int ConnectionEditPage::connectionSuffixNum(const QString &matchConnName)
{
    if (matchConnName.isEmpty()) {
        return 0;
    }

    NetworkManager::Connection::List connList = listConnections();
    QStringList connNameList;
    int connSuffixNum = 1;

    for (auto conn : connList) {
        if (conn->settings()->connectionType() == m_connType) {
            connNameList.append(conn->name());
        }
    }

    for (int i = 1; i <= connNameList.size(); ++i) {
        if (!connNameList.contains(matchConnName.arg(i))) {
            connSuffixNum = i;
            break;
        } else if (i == connNameList.size()) {
            connSuffixNum = i + 1;
        }
    }

    return connSuffixNum;
}

void ConnectionEditPage::addHeaderButton(QPushButton *button)
{
    m_mainLayout->insertWidget(m_mainLayout->indexOf(m_removeBtn) + 1, button);
}
