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

#ifndef CONNECTIONEDITPAGE_H
#define CONNECTIONEDITPAGE_H

#include "settings/abstractsettings.h"

#include <QPointer>

#include <widgets/contentwidget.h>

#include <networkmanagerqt/connection.h>
#include <networkmanagerqt/connectionsettings.h>

namespace dcc {
  namespace widgets {
    class ButtonTuple;
  }
}
namespace dccV20 {
  class FrameProxyInterface;
}

class QPushButton;
class QVBoxLayout;

using namespace dccV20;
using namespace dcc::widgets;

class ConnectionEditPage : public dcc::ContentWidget
{
    Q_OBJECT

public:
    // Keep the same enum value as ConnectionSettings::ConnectionType
    enum ConnectionType {
        PppoeConnection = 9,
        VpnConnection = 11,
        WiredConnection = 13,
        WirelessConnection = 14
    };

    // Connection will NOT be activated if "devPath" is empty
    // A NEW connection will be created if "connUuid" is empty
    explicit ConnectionEditPage(ConnectionType connType, const QString &devPath, const QString &connUuid = QString(), QWidget *parent = nullptr, bool isHotSpot = false);
    virtual ~ConnectionEditPage() Q_DECL_OVERRIDE;

    // This method or some similar method of Subclass must be called after initialization
    virtual void initSettingsWidget();

    static const QString devicePath();
    void setDevicePath(const QString &path);
    inline const QString connectionUuid() { return m_connectionUuid; }

    void onDeviceRemoved();

    static void setFrameProxy(FrameProxyInterface *frame);
    void setButtonTupleEnable(bool enable);

Q_SIGNALS:
    void requestNextPage(ContentWidget *const page);
    void requestFrameAutoHide(const bool autoHide) const;
    void requestWiredDeviceEnabled(const QString &devPath, const bool enabled) const;
    void activateWiredConnection(const QString &connString, const QString &uuid);
    void activateWirelessConnection(const QString &ssid, const QString &uuid);
    // private signals
    void saveSettingsDone();
    void prepareConnectionDone();
    void disconnect(const QString &);

protected:
    int connectionSuffixNum(const QString &matchConnName);
    void addHeaderButton(QPushButton *button);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void initUI();
    void initHeaderButtons();
    void initConnection();
    void initConnectionSecrets();
    void saveConnSettings();
    void prepareConnection();
    void updateConnection();
    void createConnSettings();

    NMVariantMapMap secretsMapMapBySettingType(Setting::SettingType settingType);
    // T means a Setting subclass, like WirelessSecuritySetting
    template <typename T>
    void setSecretsFromMapMap(Setting::SettingType settingType, NMVariantMapMap secretsMapMap);

protected Q_SLOTS:
    void onRequestNextPage(ContentWidget *const page);

protected:
    QVBoxLayout *m_settingsLayout;

    NetworkManager::Connection::Ptr m_connection;
    ConnectionSettings::Ptr m_connectionSettings;

    AbstractSettings *m_settingsWidget;

    static FrameProxyInterface *m_frame;

private:
    QVBoxLayout *m_mainLayout;
    QPushButton *m_disconnectBtn;
    QPushButton *m_removeBtn;
    ButtonTuple *m_buttonTuple;
    ButtonTuple *m_buttonTuple_conn;

    QPointer<ContentWidget> m_subPage;

    ConnectionSettings::ConnectionType m_connType;

    bool m_isNewConnection;
    QString m_connectionUuid;
    bool m_isHotSpot;
};

#endif /* CONNECTIONEDITPAGE_H */
