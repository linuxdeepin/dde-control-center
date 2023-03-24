// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCC_BLUETOOTH_DEVICE_H
#define DCC_BLUETOOTH_DEVICE_H

#include <QDebug>
#include <QObject>

// INFO: when meet problem, view the link underline
// https://github.com/bluez/bluez/blob/master/src/dbus-common.c#L53-L115

class BluetoothDevice : public QObject
{
    Q_OBJECT

public:
    enum State {
        StateUnavailable = 0,
        StateAvailable = 1,
        StateConnected = 2,
        StateDisconnecting = 3
    };
    Q_ENUM(State)

public:
    explicit BluetoothDevice(QObject *parent = nullptr);

    inline QString id() const { return m_id; }

    void setId(const QString &id);

    inline QString address() const { return m_address; }

    void setAddress(const QString &addr);

    inline QString name() const { return m_name; }

    void setName(const QString &name);

    inline QString alias() const { return m_alias; }

    void setAlias(const QString &alias);

    inline bool paired() const { return m_paired; }

    void setPaired(bool paired);

    inline State state() const { return m_state; }

    void setState(const State &state, bool connectState);

    inline bool trusted() const { return m_trusted; }

    void setTrusted(bool trusted);

    inline bool connecting() const { return m_connecting; }

    void setConnecting(bool connecting);

    inline QString deviceType() const { return m_deviceType; }

    void setDeviceType(const QString &deviceType);

    inline bool connectState() const { return m_connectState; }

    bool canSendFile() const;

Q_SIGNALS:
    void nameChanged(const QString &name) const;
    void aliasChanged(const QString &alias) const;
    void pairedChanged(const bool &paired) const;
    void stateChanged(const State &state, bool paired) const;
    void trustedChanged(const bool trusted) const;
    void connectingChanged(const bool &connecting) const;

private:
    QString m_id;
    QString m_address;
    QString m_name;
    QString m_alias;
    QString m_deviceType;
    bool m_paired;
    bool m_trusted;
    bool m_connecting;
    bool m_connectState;
    State m_state;
};

QDebug &operator<<(QDebug &stream, const BluetoothDevice *device);

#endif // DCC_BLUETOOTH_DEVICE_H
