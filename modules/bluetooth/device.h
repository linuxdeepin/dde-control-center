/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DCC_BLUETOOTH_DEVICE_H
#define DCC_BLUETOOTH_DEVICE_H

#include <QObject>
#include <QDebug>

namespace dcc {
namespace bluetooth {

class Device : public QObject
{
    Q_OBJECT

public:
    enum State {
        StateUnavailable = 0,
        StateAvailable   = 1,
        StateConnected   = 2
    };
    Q_ENUM(State)

public:
    explicit Device(QObject *parent = 0);

    inline QString id() const { return m_id; }
    void setId(const QString &id);

    inline QString name() const { return m_name; }
    void setName(const QString &name);

    inline bool paired() const { return m_paired; }
    void setPaired(bool paired);

    inline State state() const { return m_state; }
    void setState(const State &state);

signals:
    void nameChanged(const QString &name) const;
    void pairedChanged(const bool &paired) const;
    void stateChanged(const State &state) const;

private:
    QString m_id;
    QString m_name;
    bool m_paired;
    State m_state;
};

QDebug &operator<<(QDebug &stream, const Device *device);

} // namespace bluetooth
} // namespace dcc

#endif // DCC_BLUETOOTH_DEVICE_H
