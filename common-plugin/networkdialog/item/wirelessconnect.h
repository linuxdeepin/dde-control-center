// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
    bool hasPassword();
    void initConnection();
    void activateConnection();
    NetworkManager::WirelessSecuritySetting::KeyMgmt getKeyMgmtByAp(dde::network::AccessPoints *ap);

public Q_SLOTS:
    void connectNetwork();
    bool connectNetworkPassword(const QString password);

Q_SIGNALS:
    void passwordError(const QString oldPassword);

private:
    dde::network::WirelessDevice *m_device;
    dde::network::AccessPoints *m_accessPoint;
    QString m_ssid;
    bool m_needUpdate;

    NetworkManager::ConnectionSettings::Ptr m_connectionSettings;
};

#endif //  WIRELESSCONNECT_H
