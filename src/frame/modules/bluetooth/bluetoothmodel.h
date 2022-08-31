// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
