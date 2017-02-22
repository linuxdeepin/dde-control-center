/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DCC_BLUETOOTH_ADAPTER_H
#define DCC_BLUETOOTH_ADAPTER_H

#include <QObject>

#include "device.h"

namespace dcc {
namespace bluetooth {

class Adapter : public QObject
{
    Q_OBJECT
public:
    explicit Adapter(QObject *parent = 0);
    ~Adapter();

    inline QString name() const { return m_name; }
    void setName(const QString &name);

    inline QString id() const { return m_id; }
    void setId(const QString &id);

    QList<const Device *> devices() const;
    const Device *deviceById(const QString &id) const;

    inline bool powered() const { return m_powered; }
    void setPowered(bool powered);

public slots:
    void addDevice(const Device *device);
    const Device *removeDevice(const QString &deviceId);

signals:
    void nameChanged(const QString &name) const;
    void deviceAdded(const Device *device) const;
    void deviceRemoved(const QString &deviceId) const;
    void poweredChanged(const bool &powered) const;

private:
    QString m_id;
    QString m_name;
    bool m_powered;
    QList<const Device *> m_devices;
};

} // namespace bluetooth
} // namespace dcc

#endif // DCC_BLUETOOTH_ADAPTER_H
