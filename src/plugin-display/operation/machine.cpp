//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
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
    Q_EMIT disconnectStatusChanged(status);
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

void Machine::setDirection(const quint16 dir)
{
    if (m_direction != dir) {
        m_direction = dir;
        Q_EMIT directionChanged(dir);
    }
}

