// SPDX-FileCopyrightText: 2016 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "device.h"

#include <DStyleHelper>
#include <QDateTime>

QMap<QString, QString> Device::deviceType2Icon = {
    {"unknow", "other"},
    {"computer", "pc"},
    {"phone", "phone"},
    {"video-display", "vidicon"},
    {"multimedia-player", "tv"},
    {"scanner", "scaner"},
    {"input-keyboard", "keyboard"},
    {"input-mouse", "mouse"},
    {"input-gaming", "other"},
    {"input-tablet", "touchpad"},
    {"audio-card", "pheadset"},
    {"audio-headset", "pheadset"},
    {"audio-headphones", "pheadset"},
    {"network-wireless", "lan"},
    {"camera-video", "vidicon"},
    {"printer", "print"},
    {"camera-photo", "camera"},
    {"modem", "other"}
};

Device::Device(QObject *parent)
    : QObject(parent)
    , m_paired(false)
    , m_trusted(false)
    , m_connecting(false)
    , m_rssi(0)
    , m_state(StateUnavailable)
    , m_connectState(false)
    , m_battery(0)
{
}

Device::~Device()
{
}

void Device::setId(const QString &id)
{
    m_id = id;
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

void Device::setState(const State &state)
{
    // 后端频繁发送属性改变信号，dbus信号处理顺序可能异常（不一定按顺序执行），会导致蓝牙连接状态显示异常
    // 如果当前蓝牙设备状态为已连接，后端传递的状态为正在连接中，此操作视为一次异常的信号(Device::StateConnected -> Device::StateAvailable)，不做处理
    // 正常的蓝牙连接状态为Device::StateAvailable -> Device::StateConnected
    if (m_state == Device::StateConnected && state == Device::StateAvailable) {
        return;
    }

    if (state != m_state) {
        m_state = state;
        Q_EMIT stateChanged(state);
    }
}

void Device::setConnectState(const bool connectState)
{
    if (connectState != m_connectState) {
        m_connectState = connectState;
        Q_EMIT connectStateChanged(connectState);
    }
}

void Device::setRssi(int rssi)
{
    if (m_rssi != rssi) {
        m_rssi = rssi;
        Q_EMIT rssiChanged(rssi);
    }
}

void Device::setBattery(const int battery)
{
    if (m_battery != battery) {
        m_battery = battery;
        Q_EMIT batteryChanged(m_battery);
    }
}

void Device::setDeviceType(const QString &deviceType)
{
    m_deviceType = deviceType2Icon[deviceType];
}

QDebug &operator<<(QDebug &stream, const Device *device)
{
    stream << "Device name: " << device->name()
           << ", paired:" << device->paired()
           << ", state:" << device->state();

    return stream;
}
