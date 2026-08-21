// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
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

void BluetoothDevice::setBattery(int battery)
{
    if (m_battery != battery) {
        m_battery = battery;
        Q_EMIT batteryChanged(battery);
    }
}

void BluetoothDevice::setDeviceType(const QString &deviceType)
{
    m_deviceType = deviceType2Icon.contains(deviceType) ? deviceType2Icon[deviceType] : "bluetooth_other";
}

bool BluetoothDevice::canSendFile() const
{
    // 目前pc和手机可以发送蓝牙文件
    if ((m_deviceType == "bluetooth_pc") || (m_deviceType == "bluetooth_phone")) {
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
