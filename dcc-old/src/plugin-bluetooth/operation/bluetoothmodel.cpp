//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "bluetoothmodel.h"

BluetoothModel::BluetoothModel(QObject *parent)
    : QObject(parent)
    , m_transPortable(false)
    , m_canSendFile(false)
    , m_airplaneEnable(false)
    , m_displaySwitch(false)
{
    m_adapters.clear();
}

void BluetoothModel::addAdapter(BluetoothAdapter *adapter)
{
    if (!adapterById(adapter->id())) {
        m_adapters[adapter->id()] = adapter;
        m_adapterIds << adapter->id();
        Q_EMIT adapterAdded(adapter);
        Q_EMIT adpaterListChanged();
        return;
    }

    adapter->deleteLater();
}

const BluetoothAdapter *BluetoothModel::removeAdapater(const QString &adapterId)
{
    const BluetoothAdapter *adapter = nullptr;

    adapter = adapterById(adapterId);
    if (adapter) {
        m_adapters.remove(adapterId);
        m_adapterIds.removeOne(adapterId);
        Q_EMIT adapterRemoved(adapter);
        Q_EMIT adpaterListChanged();
    }

    return adapter;
}

QList<const BluetoothAdapter *> BluetoothModel::adapters() const
{
    QList<const BluetoothAdapter *> allAdapters = m_adapters.values();
    std::sort(allAdapters.begin(), allAdapters.end(), [ & ](const BluetoothAdapter *adapter1, const BluetoothAdapter *adapter2) {
        return m_adapterIds.indexOf(adapter1->id()) < m_adapterIds.indexOf(adapter2->id());
    });

    return allAdapters;
}

const BluetoothAdapter *BluetoothModel::adapterById(const QString &id)
{
    return m_adapters.keys().contains(id) ? m_adapters[id] : nullptr;
}

/**
 * @brief BluetoothModel::canTransportable
 * @return
 * 返回值表示是否能传输蓝牙文件
 */
bool BluetoothModel::canTransportable() const
{
    return m_transPortable;
}

void BluetoothModel::setMyDeviceVisible(const bool visible)
{
    if (m_myDeviceVisible == visible)
        return;

    m_myDeviceVisible = visible;
    Q_EMIT notifyMyDeviceVisibleChanged(m_myDeviceVisible);
}

void BluetoothModel::setOtherDeviceVisible(const bool visible)
{
    if (m_otherDeviceVisible == visible)
        return;

    m_otherDeviceVisible = visible;
    Q_EMIT notifyOtherDeviceVisibleChanged(m_otherDeviceVisible);
}

/**
 * @brief BluetoothModel::setTransportable
 * @param transPortable
 * 设置是否能传输蓝牙文件
 */
void BluetoothModel::setTransportable(const bool transPortable)
{
    if (m_transPortable != transPortable) {
        m_transPortable = transPortable;
        Q_EMIT transportableChanged(transPortable);
    }
}

void BluetoothModel::setCanSendFile(const bool canSendFile)
{
    if (m_canSendFile != canSendFile) {
        m_canSendFile = canSendFile;
        Q_EMIT canSendFileChanged(canSendFile);
    }
}

void BluetoothModel::setAirplaneEnable(bool enable)
{
    if (m_airplaneEnable == enable)
        return;

    m_airplaneEnable = enable;

    Q_EMIT airplaneEnableChanged(m_airplaneEnable);
}

void BluetoothModel::setDisplaySwitch(bool on)
{
    if (m_displaySwitch == on)
        return;

    m_displaySwitch = on;

    Q_EMIT displaySwitchChanged(m_displaySwitch);
}
