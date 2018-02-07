/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#ifndef NETWORKDEVICE_H
#define NETWORKDEVICE_H

#include <QObject>
#include <QJsonObject>
#include <QSet>

namespace dcc {

namespace network {

class NetworkDevice : public QObject
{
    Q_OBJECT

public:
    enum DeviceType
    {
        None,
        Wired,
        Wireless,
    };

    enum DeviceStatus
    {
        Unknow          = 0,
        Unmanaged       = 10,
        Unavailable     = 20,
        Disconnected    = 30,
        Prepare         = 40,
        Config          = 50,
        NeedAuth        = 60,
        IpConfig        = 70,
        IpCheck         = 80,
        Secondaries     = 90,
        Activated       = 100,
        Deactivation    = 110,
        Failed          = 120,
    };

public:
    virtual ~NetworkDevice();

    bool enabled() const { return m_enabled; }
    DeviceType type() const { return m_type; }
    DeviceStatus status() const { return m_status; }
    const QString statusString() const;
    const QJsonObject info() const { return m_deviceInfo; }
    const QString path() const;
    const QString usingHwAdr() const;

signals:
    void removed() const;
    void statusChanged(DeviceStatus stat) const;
    void statusChanged(const QString &statStr) const;
    void enableChanged(const bool enabled) const;
    void sessionCreated(const QString &sessionPath) const;

public slots:
    void setEnabled(const bool enabled);
    void updateDeviceInfo(const QJsonObject &devInfo);

protected:
    explicit NetworkDevice(const DeviceType type, const QJsonObject &info, QObject *parent = 0);

private slots:
    void setDeviceStatus(const int status);

private:
    const DeviceType m_type;
    DeviceStatus m_status;
    QJsonObject m_deviceInfo;

    bool m_enabled;
};

}   // namespace network

}   // namespace dcc

#endif // NETWORKDEVICE_H
