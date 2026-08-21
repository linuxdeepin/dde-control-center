//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCC_BLUETOOTH_BLUETOOTHMODEL_H
#define DCC_BLUETOOTH_BLUETOOTHMODEL_H

#include <QObject>
#include <QtQmlIntegration/qqmlintegration.h>

#include "bluetoothadapter.h"
#include "bluetoothadaptersmodel.h"

class BluetoothModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool displaySwitch READ displaySwitch NOTIFY displaySwitchChanged FINAL)
    Q_PROPERTY(bool airplaneEnable READ airplaneEnable NOTIFY airplaneEnableChanged FINAL)
    Q_PROPERTY(bool showBluetooth READ showBluetooth NOTIFY showBluetoothChanged FINAL)

    QML_NAMED_ELEMENT(BluetoothModel)
    QML_SINGLETON
public:
    explicit BluetoothModel(QObject *parent = nullptr);

    QList<const BluetoothAdapter *> adapters() const;
    const BluetoothAdapter *adapterById(const QString &id);

    bool canTransportable() const;
    inline bool canSendFile() const { return m_canSendFile; }

    inline bool displaySwitch() const { return m_displaySwitch; }

    Q_INVOKABLE BlueToothAdaptersModel *blueToothAdaptersModel() const;

    void updateAdaptersModel(BluetoothAdapter* data);

    bool airplaneEnable() const;

    bool showBluetooth() const;
    void setShowBluetooth(bool newShowBluetooth);

public Q_SLOTS:
    void addAdapter(BluetoothAdapter *adapter);
    const BluetoothAdapter *removeAdapater(const QString &adapterId);
    void setTransportable(const bool transPortable);
    void setCanSendFile(const bool canSendFile);
    void setAirplaneEnable(bool enable);
    void setDisplaySwitch(bool on);

Q_SIGNALS:
    void adapterAdded(const BluetoothAdapter *adapter) const;
    void adapterRemoved(const BluetoothAdapter *adapter) const;
    void adpaterListChanged();
    void transportableChanged(const bool transPortable) const;
    void canSendFileChanged(const bool canSendFile) const;
    void airplaneEnableChanged(bool enable) const;
    void displaySwitchChanged(bool on) const;
    void showBluetoothChanged();

private:
    QMap<QString, const BluetoothAdapter *> m_adapters;
    QStringList m_adapterIds;
    bool m_transPortable;
    bool m_canSendFile;
    bool m_airplaneEnable;
    bool m_displaySwitch;
    friend class BluetoothWorker;

    bool m_showBluetooth;

    BlueToothAdaptersModel* m_blueToothAdaptersModel;

};

#endif // DCC_BLUETOOTH_BLUETOOTHMODEL_H
