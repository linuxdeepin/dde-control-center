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

#include "bluetoothmodel.h"

namespace dcc {
namespace bluetooth {

BluetoothModel::BluetoothModel(QObject *parent)
    : QObject(parent)
    , m_transPortable(false)
    , m_canSendFile(false)
    , m_airplaneEnable(false)
{
    m_adapters.clear();
}

void BluetoothModel::addAdapter(Adapter *adapter)
{
    if (!adapterById(adapter->id())) {
        m_adapters[adapter->id()] = adapter;
        Q_EMIT adapterAdded(adapter);
        Q_EMIT adpaterListChanged();
        return;
    }

    adapter->deleteLater();
}

const Adapter *BluetoothModel::removeAdapater(const QString &adapterId)
{
    const Adapter *adapter = nullptr;

    adapter = adapterById(adapterId);
    if (adapter) {
        m_adapters.remove(adapterId);
        Q_EMIT adapterRemoved(adapter);
        Q_EMIT adpaterListChanged();
    }

    return adapter;
}

QMap<QString, const Adapter *> BluetoothModel::adapters() const
{
    return m_adapters;
}

const Adapter *BluetoothModel::adapterById(const QString &id)
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

} // namespace bluetooth
} // namespace dcc
