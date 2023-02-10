/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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

#include "bluetoothdevice.h"

BluetoothDevice::BluetoothDevice(QObject *parent)
    : QObject(parent)
    , m_id("")
    , m_name("")
    , m_paired(false)
    , m_trusted(false)
    , m_connecting(false)
    , m_connectState(false)
    , m_state(StateUnavailable)
{
}

void BluetoothDevice::setId(const QString &id)
{
    m_id = id;
}

void BluetoothDevice::setAddress(const QString &addr)
{
    m_address = addr;
}

void BluetoothDevice::setName(const QString &name)
{
    if (name != m_name) {
        m_name = name;
        Q_EMIT nameChanged(name);
    }
}

void BluetoothDevice::setAlias(const QString &alias)
{
    if (alias != m_alias) {
        m_alias = alias;
        Q_EMIT aliasChanged(alias);
    }
}

void BluetoothDevice::setPaired(bool paired)
{
    if (paired != m_paired) {
        m_paired = paired;
        Q_EMIT pairedChanged(paired);
    }
}

void BluetoothDevice::setState(const State &state, bool connectState)
{
    if ((state != m_state) || (connectState != m_connectState)) {
        m_state = state;
        m_connectState = connectState;
        Q_EMIT stateChanged(state, connectState);
    }
}

void BluetoothDevice::setTrusted(bool trusted)
{
    if (trusted != m_trusted) {
        m_trusted = trusted;
        Q_EMIT trustedChanged(trusted);
    }
}

void BluetoothDevice::setConnecting(bool connecting)
{
    if (connecting != m_connecting) {
        m_connecting = connecting;
        Q_EMIT connectingChanged(connecting);
    }
}

void BluetoothDevice::setDeviceType(const QString &deviceType)
{
    m_deviceType = deviceType2Icon[deviceType];
}

bool BluetoothDevice::canSendFile() const
{
    // 目前pc和手机可以发送蓝牙文件
    if ((m_deviceType == "pc") || (m_deviceType == "phone")) {
        return true;
    }
    return false;
}

QDebug &operator<<(QDebug &stream, const BluetoothDevice *device)
{
    stream << "BluetoothDevice name:" << device->name() << " paired:" << device->paired()
           << " state:" << device->state();

    return stream;
}
