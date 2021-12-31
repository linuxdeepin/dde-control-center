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
#include "window/gsettingwatcher.h"

#include <networkmanagerqt/settings.h>
#include <networkmanagerqt/security8021xsetting.h>
#include <networkmanagerqt/wirelesssecuritysetting.h>
#include <networkmanagerqt/pppoesetting.h>
#include <networkmanagerqt/vpnsetting.h>

#include <DDialog>
#include <QDBusMetaType>
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::network;
using namespace NetworkManager;
DWIDGET_USE_NAMESPACE

static QString DevicePath = "";

dccV20::FrameProxyInterface *ConnectionEditPage::m_frame = nullptr;

ConnectionEditPage::ConnectionEditPage(ConnectionType connType,
                                       const QString &devPath,
                                       const QString &connUuid,
                                       QWidget *parent,
                                       bool isHotSpot)
    : ContentWidget(parent)
    , m_settingsLayout(new QVBoxLayout)
    , m_connection(nullptr)
    , m_connectionSettings(nullptr)
    , m_settingsWidget(nullptr)
    , m_mainLayout(new QVBoxLayout)
    , m_disconnectBtn(nullptr)
    , m_removeBtn(nullptr)
    , m_buttonTuple(new ButtonTuple(ButtonTuple::Save))
    , m_buttonTuple_conn(new ButtonTuple(ButtonTuple::Delete))
    , m_subPage(nullptr)
    , m_connType(static_cast<NetworkManager::ConnectionSettings::ConnectionType>(connType))
    , m_isNewConnection(false)
    , m_connectionUuid(connUuid)
    , m_isHotSpot(isHotSpot)
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
            qDebug() << "can't find connection by uuid";
            return;
        }
        m_connectionSettings = m_connection->settings();
        m_isNewConnection = false;
        initConnectionSecrets();
    }

    initHeaderButtons();
    initConnection();

    m_removeBtn->installEventFilter(this);
}

ConnectionEditPage::~ConnectionEditPage()
{
    GSettingWatcher::instance()->erase("removeConnection", m_removeBtn);
}

void ConnectionEditPage::initUI()
{
    setAccessibleName("ConnectionEditPage");
    m_settingsLayout->setSpacing(10);

    m_disconnectBtn = m_buttonTuple_conn->leftButton();
    m_removeBtn = m_buttonTuple_conn->rightButton();
    GSettingWatcher::instance()->bind("removeConnection", m_removeBtn);

    m_disconnectBtn->setText(tr("Disconnect"));
    m_disconnectBtn->setVisible(false);
    m_removeBtn->setText(tr("Delete"));
    m_removeBtn->setVisible(false);

    QPushButton *cancelBtn = m_buttonTuple->leftButton();
    QPushButton *acceptBtn = m_buttonTuple->rightButton();
    m_buttonTuple->setAutoFillBackground(true);
    cancelBtn->setText(tr("Cancel"));
    acceptBtn->setText(tr("Save"));
    m_buttonTuple->leftButton()->setEnabled(false);
    m_buttonTuple->rightButton()->setEnabled(false);

    m_mainLayout->setContentsMargins(10, 10, 10, 10);
    m_mainLayout->addSpacing(10);
    m_mainLayout->addWidget(m_buttonTuple_conn);
    m_mainLayout->addLayout(m_settingsLayout);
    m_mainLayout->addStretch();
    m_mainLayout->setSpacing(10);

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(m_mainLayout);

    setContent(mainWidget);
    QVBoxLayout * btnTupleLayout = new QVBoxLayout();
//    btnTupleLayout->setMargin(0);
    btnTupleLayout->setSpacing(0);
    btnTupleLayout->setContentsMargins(10, 10, 10, 10);
    btnTupleLayout->addWidget(m_buttonTuple);
    qobject_cast<QVBoxLayout *>(layout())->addLayout(btnTupleLayout);

    setMinimumWidth(380);
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
            m_disconnectBtn->setProperty("connectionUuid", conn->uuid());
            break;
        }
    }
    m_removeBtn->setVisible(true);

    //当只有m_removeBtn显示时,由于布局中添加了space,导致删除按钮未对齐,需要删除空格
    if (!m_disconnectBtn->isHidden())
        return;
    m_buttonTuple_conn->removeSpacing();

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
        m_settingsWidget = new DslPppoeSettings(m_connectionSettings, DevicePath, this);
        break;
    }
    default:
        break;
    }

    connect(m_settingsWidget, &AbstractSettings::anyEditClicked, this, [this]{
        m_buttonTuple->leftButton()->setEnabled(true);
        m_buttonTuple->rightButton()->setEnabled(true);
    });
    connect(m_settingsWidget, &AbstractSettings::requestNextPage, this, &ConnectionEditPage::onRequestNextPage);
    connect(m_settingsWidget, &AbstractSettings::requestFrameAutoHide, this, &ConnectionEditPage::requestFrameAutoHide);

    m_settingsLayout->addWidget(m_settingsWidget);
}

const QString ConnectionEditPage::devicePath()
{
    return DevicePath;
}

void ConnectionEditPage::setDevicePath(const QString &path)
{
    DevicePath = path;
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

    if (m_frame) {
        connect(this, &ConnectionEditPage::back, std::bind(&dccV20::FrameProxyInterface::popWidget, m_frame, nullptr));
    }

    connect(m_removeBtn, &QPushButton::clicked, this, [ = ]() {
        DDialog dialog(this);
        dialog.setAccessibleName("Form_" + tr("Are you sure you want to delete this configuration?"));
        dialog.setTitle(tr("Are you sure you want to delete this configuration?"));
        QStringList btns;
        btns << tr("Cancel");
        btns << tr("Delete");
        dialog.addButtons(btns);
        int ret = dialog.exec();
        if (ret == QDialog::Accepted) {
            m_connection->remove();
            Q_EMIT back();
        }
    });

    connect(m_disconnectBtn, &QPushButton::clicked, this, [ = ]() {
        deactivateConnection(m_disconnectBtn->property("activeConnectionPath").toString());
        Q_EMIT disconnect(m_disconnectBtn->property("connectionUuid").toString());
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

void ConnectionEditPage::onRequestNextPage(dcc::ContentWidget *const page)
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
        if (!m_isHotSpot) {
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
    }

    m_settingsWidget->saveSettings();

    Q_EMIT saveSettingsDone();
}

void ConnectionEditPage::prepareConnection()
{
    if (!m_connection) {
        qDebug() << "preparing connection...";
        qDBusRegisterMetaType<QByteArrayList>();
        QDBusPendingReply<QDBusObjectPath> reply = addConnection(m_connectionSettings->toMap());
        reply.waitForFinished();
        const QString &connPath = reply.value().path();
        m_connection = findConnection(connPath);
        if (!m_connection) {
            qDebug() << "create connection failed..." << reply.error();
            Q_EMIT back();
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
        Q_EMIT back();
        return;
    }

    if (m_settingsWidget->isAutoConnect()) {
        if (static_cast<int>(m_connType) == static_cast<int>(ConnectionEditPage::WiredConnection)) {
            Q_EMIT activateWiredConnection(m_connection->path(), m_connectionUuid);
        } else {
            if (static_cast<int>(m_connType) == static_cast<int>(ConnectionEditPage::WirelessConnection)) {
                Q_EMIT activateWirelessConnection(m_connectionSettings->id(), m_connectionUuid);
            }
            reply = activateConnection(m_connection->path(), DevicePath, "");
            reply.waitForFinished();
            if (reply.isError()) {
                qDebug() << "error occurred while activate connection" << reply.error();
            }
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
        if (m_isHotSpot) {
            connName = tr("hotspot");
        } else {
            connName = tr("Wireless Connection %1");
        }
        m_connectionSettings->setting(Setting::Security8021x).staticCast<NetworkManager::Security8021xSetting>()->setPasswordFlags(Setting::AgentOwned);
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
    m_connectionUuid = m_connectionSettings->createNewUuid();
    while (findConnectionByUuid(m_connectionUuid) != nullptr) {
        qint64 second = QDateTime::currentDateTime().toSecsSinceEpoch();
        m_connectionUuid.replace(24, QString::number(second).length(), QString::number(second));
    }
    m_connectionSettings->setUuid(m_connectionUuid);
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
    m_mainLayout->insertWidget(m_mainLayout->indexOf(m_buttonTuple_conn) + 1, button);
}

bool ConnectionEditPage::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Show && obj == m_removeBtn) {
        QWidget *widget = static_cast<QWidget *>(obj);
        bool visible = "Hidden" != GSettingWatcher::instance()->getStatus("removeConnection");
        if (m_isNewConnection)
            widget->setVisible(false);
        else if (visible != widget->isVisible())
            widget->setVisible(visible);
    }
    return QObject::eventFilter(obj, event);
}

void ConnectionEditPage::setFrameProxy(dccV20::FrameProxyInterface *_frame)
{
    m_frame = _frame;
}

void ConnectionEditPage::setButtonTupleEnable(bool enable)
{
    m_buttonTuple->leftButton()->setEnabled(enable);
    m_buttonTuple->rightButton()->setEnabled(enable);
}
