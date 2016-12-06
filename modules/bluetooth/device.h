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

namespace dcc {
namespace bluetooth {

class Device : public QObject
{
    Q_OBJECT
public:
    explicit Device(QObject *parent = 0);

    inline QString id() const { return m_id; }
    void setId(const QString &id);

    inline QString name() const { return m_name; }
    void setName(const QString &name);

    inline bool paired() const { return m_paired; }
    void setPaired(bool paired);

signals:
    void nameChanged(const QString &name) const;
    void pairedChanged(const bool paired) const;

private:
    QString m_id;
    QString m_name;
    bool m_paired;
};

} // namespace bluetooth
} // namespace dcc

#endif // DCC_BLUETOOTH_DEVICE_H
