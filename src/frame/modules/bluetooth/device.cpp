// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "device.h"

namespace dcc {
namespace bluetooth {

Device::Device(QObject *parent) :
    QObject(parent),
    m_id(""),
    m_name(""),
    m_paired(false),
    m_trusted(false),
    m_connecting(false),
    m_connectState(false),
    m_state(StateUnavailable)
{

}

void Device::setId(const QString &id)
{
    m_id = id;
}

void Device::setAddress(const QString &addr)
{
    m_address = addr;
}

void Device::setName(const QString &name)
{
    if (name != m_name) {
        m_name = name;
        Q_EMIT nameChanged(name);
    }
}

void Device::setAlias(const QString &alias)
{
    if (alias != m_alias) {
        m_alias = alias;
        Q_EMIT aliasChanged(alias);
    }
}

void Device::setPaired(bool paired)
{
    if (paired != m_paired) {
        m_paired = paired;
        Q_EMIT pairedChanged(paired);
    }
}

void Device::setState(const State &state, bool connectState)
{
    // 后端频繁发送属性改变信号，dbus信号处理顺序可能异常（不一定按顺序执行），会导致蓝牙连接状态显示异常
    // 如果当前蓝牙设备状态为已连接，后端传递的状态为正在连接中，此操作视为一次异常的信号(Device::StateConnected -> Device::StateAvailable)，不做处理
    // 正常的蓝牙连接状态为Device::StateAvailable -> Device::StateConnected
    if (m_state == Device::StateConnected && state == Device::StateAvailable) {
        return;
    }

    if ((state != m_state) || (connectState != m_connectState)) {
        m_state = state;
        m_connectState = connectState;
        Q_EMIT stateChanged(state, connectState);
    }
}

void Device::setTrusted(bool trusted)
{
    if (trusted != m_trusted) {
        m_trusted = trusted;
        Q_EMIT trustedChanged(trusted);
    }
}

void Device::setConnecting(bool connecting)
{
    if (connecting != m_connecting) {
        m_connecting = connecting;
        Q_EMIT connectingChanged(connecting);
    }
}

void Device::setDeviceType(const QString &deviceType)
{
    m_deviceType = deviceType2Icon[deviceType];
}

bool Device::canSendFile() const
{
    // 目前pc和手机可以发送蓝牙文件
    if ((m_deviceType == "pc") || (m_deviceType == "phone")) {
        return true;
    }
    return false;
}

QDebug &operator<<(QDebug &stream, const Device *device)
{
    stream << "Device name:" << device->name() << " paired:" << device->paired() << " state:" << device->state();

    return stream;
}

} // namespace bluetooth
} // namespace dcc
