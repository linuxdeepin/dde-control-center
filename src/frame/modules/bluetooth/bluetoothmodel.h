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

#ifndef DCC_BLUETOOTH_BLUETOOTHMODEL_H
#define DCC_BLUETOOTH_BLUETOOTHMODEL_H

#include <QObject>

#include "adapter.h"

namespace dcc {
namespace bluetooth {

class BluetoothModel : public QObject
{
    Q_OBJECT
public:
    explicit BluetoothModel(QObject *parent = nullptr);

    QMap<QString, const Adapter *> adapters() const;
    const Adapter *adapterById(const QString &id);

    bool canTransportable() const;
    inline bool canSendFile() const { return m_canSendFile; }

    inline bool airplaneMode() const { return m_airplaneEnable; }
    inline bool displaySwitch() const { return m_displaySwitch; }

    bool myDeviceVisible() {return m_myDeviceVisible;}
    void setMyDeviceVisible(const bool visible);

    bool otherDeviceVisible() {return m_otherDeviceVisible;}
    void setOtherDeviceVisible(const bool visible);

public Q_SLOTS:
    void addAdapter(Adapter *adapter);
    const Adapter *removeAdapater(const QString &adapterId);
    void setTransportable(const bool transPortable);
    void setCanSendFile(const bool canSendFile);
    void setAirplaneEnable(bool enable);
    void setDisplaySwitch(bool on);

Q_SIGNALS:
    void adapterAdded(const Adapter *adapter) const;
    void adapterRemoved(const Adapter *adapter) const;
    void adpaterListChanged();
    void adpaterPowerChanged(const bool &power) const;
    void transportableChanged(const bool transPortable) const;
    void canSendFileChanged(const bool canSendFile) const;
    void airplaneEnableChanged(bool enable) const;
    void displaySwitchChanged(bool on) const;
    void notifyMyDeviceVisibleChanged(bool);
    void notifyOtherDeviceVisibleChanged(bool);

private:
    QMap<QString, const Adapter *> m_adapters;
    bool m_transPortable;
    bool m_canSendFile;
    bool m_airplaneEnable;
    bool m_displaySwitch;
    friend class BluetoothWorker;
    bool m_myDeviceVisible = false;
    bool m_otherDeviceVisible = false;
};

} // namespace bluetooth
} // namespace dcc

#endif // DCC_BLUETOOTH_BLUETOOTHMODEL_H
