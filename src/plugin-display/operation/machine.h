/*
 * Copyright (C) 2011 ~ 2022 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>

 * Maintainer: duanhongyu <duanhongyu@uniontech.com>
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

#ifndef MACHINE_H
#define MACHINE_H

#include "interface/namespace.h"

#include <QObject>

namespace DCC_NAMESPACE {

class Machine : public QObject
{
    Q_OBJECT
public:
    explicit Machine(QObject *parent = 0);
    ~Machine();

    void setPath(const QString &path);

    inline QString Path() { return m_path; }

    inline const QString Name() { return m_name; }

    inline const QString IP() { return m_IP; }

    inline const bool Connected() { return m_connected; }

    inline const bool deviceSharing() { return m_deviceSharing; }

    inline const bool isHistory() { return m_isHistoryDev; }

    inline const QString UUID() { return m_UUID; }

    inline const quint16 direction() { return m_direction; }

public Q_SLOTS:
    void setName(const QString &name);
    void setIP(const QString &ip);
    void setConnected(const bool connected);
    void setDeviceSharing(const bool deviceSharing);
    void setDisconnectStatus(bool status);
    void setHistoryStates(bool isHistory);
    void setUUID(const QString &uuid);
    void setDirection(const quint16 dir);

Q_SIGNALS:
    void connectedChanged(bool connected);
    void nameChanged(const QString &name);
    void IPChanged(const QString &Ip);
    void deviceSharingChanged(bool deviceSharing);
    void disconnnectStatusChanged(bool status);
    void historyStatusChanged(bool status);
    void directionChanged(const quint16 &dir);

private:
    QString m_path;
    QString m_name;
    QString m_IP;
    bool m_connected;
    bool m_deviceSharing;
    // 判断是否是历史设备
    bool m_isHistoryDev;
    QString m_UUID;
    quint16 m_direction;
};

} // namespace DCC_NAMESPACE
#endif // MACHINE_H
