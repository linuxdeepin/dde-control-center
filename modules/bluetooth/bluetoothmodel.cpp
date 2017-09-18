/*
 * Copyright (C) 2016 ~ 2017 Deepin Technology Co., Ltd.
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

BluetoothModel::BluetoothModel(QObject *parent) : QObject(parent)
{

}

void BluetoothModel::addAdapter(const Adapter *adapter)
{
    if (!adapterById(adapter->id())) {
        m_adapters.append(adapter);
        emit adapterAdded(adapter);
        emit adpaterListChanged();
    }
}

const Adapter *BluetoothModel::removeAdapater(const QString &adapterId)
{
    const Adapter *adapter = nullptr;

    adapter = adapterById(adapterId);
    if (adapter) {
        m_adapters.removeOne(adapter);
        emit adapterRemoved(adapter);
        emit adpaterListChanged();
    }

    return adapter;
}

QList<const Adapter *> BluetoothModel::adapters() const
{
    return m_adapters;
}

const Adapter *BluetoothModel::adapterById(const QString &id)
{
    for (const Adapter *adapter : m_adapters) {
        if (adapter->id() == id) {
            return adapter;
        }
    }

    return nullptr;
}

} // namespace bluetooth
} // namespace dcc
