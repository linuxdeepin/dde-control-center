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

#ifndef DCC_BLUETOOTH_DEVICE_H
#define DCC_BLUETOOTH_DEVICE_H

#include <QObject>
#include <QDebug>

static const QMap<QString,QString> deviceType2Icon {
    {"unknow","other"},
    {"computer","pc"},
    {"phone","phone"},
    {"video-display","vidicon"},
    {"multimedia-player","tv"},
    {"scanner","scaner"},
    {"input-keyboard","keyboard"},
    {"input-mouse","mouse"},
    {"input-gaming","other"},
    {"input-tablet","touchpad"},
    {"audio-card","pheadset"},
    {"network-wireless","lan"},
    {"camera-video","vidicon"},
    {"printer","print"},
    {"camera-photo","camera"},
    {"modem","other"}
};

namespace dcc {
namespace bluetooth {

class Device : public QObject
{
    Q_OBJECT

public:
    enum State {
        StateUnavailable    = 0,
        StateAvailable      = 1,
        StateConnected      = 2,
        StateDisconnecting  = 3
    };
    Q_ENUM(State)

public:
    explicit Device(QObject *parent = 0);

    inline QString deviceType () const {return m_deviceType; }
    void setDeviceType(const QString &deviceType);

    inline QString id() const { return m_id; }
    void setId(const QString &id);

    inline QString address() const { return m_address; }
    void setAddress(const QString &address);

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
    QString m_name;
    QString m_alias;
    QString m_deviceType;
    bool m_paired;
    bool m_trusted;
    bool m_connecting;
    bool m_connectState;
    State m_state;
    QString m_address; //mac地址
};

QDebug &operator<<(QDebug &stream, const Device *device);

} // namespace bluetooth
} // namespace dcc

#endif // DCC_BLUETOOTH_DEVICE_H
