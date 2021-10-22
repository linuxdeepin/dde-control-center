/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     caixiangrong <caixiangrong@uniontech.com>
 *
 * Maintainer: caixiangrong <caixiangrong@uniontech.com>
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

#ifndef WIRELESSCONNECT_H
#define WIRELESSCONNECT_H

#include <NetworkManagerQt/WirelessSecuritySetting>
#include <NetworkManagerQt/WirelessSecuritySetting>
#include <NetworkManagerQt/Connection>

namespace dde {
    namespace network {
        class NetworkDeviceBase;
        class WiredDevice;
        class WirelessDevice;
        class AccessPoints;
        class WiredConnection;
        enum class DeviceType;
    } // namespace network
} // namespace dde

class WirelessConnect : public QObject
{
    Q_OBJECT

public:
    explicit WirelessConnect(QObject *parent, dde::network::WirelessDevice *device, dde::network::AccessPoints *ap);
    ~WirelessConnect() Q_DECL_OVERRIDE;

    void setSsid(const QString &ssid);
    bool passwordIsValid(const QString &password);
    void getoldPassword();

protected:
    void setPassword(const QString &password);
    bool hasPassword(QString &password);
    void initConnection();
    void activateConnection();
    NetworkManager::WirelessSecuritySetting::KeyMgmt getKeyMgmtByAp(dde::network::AccessPoints *ap);

public Q_SLOTS:
    void connectNetwork();
    void connectNetworkPassword(const QString password);

Q_SIGNALS:
    void passwordError(const QString oldPassword);

private:
    dde::network::WirelessDevice *m_device;
    dde::network::AccessPoints *m_accessPoint;
    QString m_ssid;

    NetworkManager::ConnectionSettings::Ptr m_connectionSettings;
};

#endif //  WIRELESSCONNECT_H
