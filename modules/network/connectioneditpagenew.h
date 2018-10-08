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

#ifndef CONNECTIONEDITPAGENEW_H
#define CONNECTIONEDITPAGENEW_H

#include "contentwidget.h"
#include "sections/abstractsection.h"
#include "settings/abstractsettings.h"
#include "buttontuple.h"

#include <QPushButton>
#include <QVBoxLayout>

#include <networkmanagerqt/connection.h>
#include <networkmanagerqt/connectionsettings.h>

namespace dcc {
namespace network {

class ConnectionEditPageNew : public ContentWidget
{
    Q_OBJECT

public:
    // Keep the same enum value as NetworkManager::ConnectionSettings::ConnectionType
    enum ConnectionType {
        PppoeConnection = 9, VpnConnection = 11, WiredConnection = 13, WirelessConnection = 14
    };

    // Connection will NOT be activated if "devPath" is empty
    // A NEW connection will be created if "connUuid" is empty
    explicit ConnectionEditPageNew(ConnectionType connType, const QString &devPath, const QString &connUuid = QString(), QWidget *parent = 0);
    virtual ~ConnectionEditPageNew();

    // This method or some similar method of Subclass must be called after initialization
    virtual void initSettingsWidget();

Q_SIGNALS:
    void requestNextPage(ContentWidget * const page);

    // private signals
    void saveSettingsDone();
    void prepareConnectionDone();

protected:
    int connectionSuffixNum(const QString &matchConnName);

private:
    void initUI();
    void initHeaderButtons();
    void initConnection();
    void initConnectionSecrets();
    void saveConnSettings();
    void prepareConnection();
    void updateConnection();
    void createConnSettings();

    NMVariantMapMap secretsMapMapBySettingType(NetworkManager::Setting::SettingType settingType);
    // T means a NetworkManager::Setting subclass, like NetworkManager::WirelessSecuritySetting
    template <typename T>
    void setSecretsFromMapMap(NetworkManager::Setting::SettingType settingType, NMVariantMapMap secretsMapMap);

protected:
    QVBoxLayout *m_settingsLayout;

    NetworkManager::Connection::Ptr m_connection;
    NetworkManager::ConnectionSettings::Ptr m_connectionSettings;

    dcc::network::AbstractSettings *m_settingsWidget;

private:
    QPushButton *m_disconnectBtn;
    QPushButton *m_removeBtn;
    dcc::widgets::ButtonTuple *m_buttonTuple;

    NetworkManager::ConnectionSettings::ConnectionType m_connType;

    bool m_isNewConnection;
    QString m_devPath;
    QString m_connectionUuid;
};

} /* network */ 
} /* dcc */ 

#endif /* CONNECTIONEDITPAGENEW_H */
