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

#include "machine.h"

using namespace DCC_NAMESPACE;

Machine::Machine(QObject *parent)
    : QObject(parent)
    , m_connected(false)
    , m_deviceSharing(false)
    , m_isHistoryDev(false)
    , m_direction(1)
{

}

Machine::~Machine()
{

}

void Machine::setPath(const QString &path)
{
    m_path = path;
}

void Machine::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        Q_EMIT nameChanged(name);
    }
}

void Machine::setIP(const QString &ip)
{
    if (m_IP != ip) {
        m_IP = ip;
        Q_EMIT IPChanged(ip);
    }
}

void Machine::setConnected(const bool connected)
{
    if (m_connected != connected) {
        m_connected = connected;
        Q_EMIT connectedChanged(m_connected);
    }
}

void Machine::setDeviceSharing(const bool deviceSharing)
{
    if (m_deviceSharing != deviceSharing) {
        m_deviceSharing = deviceSharing;
        Q_EMIT  deviceSharingChanged(deviceSharing);
    }
}

void Machine::setDisconnectStatus(bool status)
{
    Q_EMIT disconnnectStatusChanged(status);
}

void Machine::setHistoryStates(bool isHistory)
{
    if (m_isHistoryDev != isHistory) {
        m_isHistoryDev = isHistory;
        Q_EMIT historyStatusChanged(isHistory);
    }
}

void Machine::setUUID(const QString &uuid)
{
    m_UUID = uuid;
}

void Machine::setDirection(const int dir)
{
    if (m_direction != dir) {
        m_direction = dir;
        Q_EMIT directionChanged(dir);
    }
}

