// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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

    inline QString name() const { return m_name; }
    void setName(const QString &name);

    inline QString id() const { return m_id; }
    void setId(const QString &id);

    QMap<QString, const Device *> devices() const;
    QList<QString> devicesId() const;
    const Device *deviceById(const QString &id) const;

    inline bool powered() const { return m_powered; }
    void setPowered(bool powered, bool discovering);

    inline bool discoverabled() const { return m_discoverable; }
    void setDiscoverabled(const bool discoverable);

    inline bool discovering() const { return m_discovering; }

public Q_SLOTS:
    void addDevice(const Device *device);
    void removeDevice(const QString &deviceId);

Q_SIGNALS:
    void nameChanged(const QString &name) const;
    void deviceAdded(const Device *device) const;
    void deviceRemoved(const QString &deviceId) const;
    void poweredChanged(const bool &powered, const bool &discovering) const;
    void loadStatus() const;
    void discoverableChanged(const bool &discoverable) const;
    void closeDetailPage() const;

private:
    QString m_id;
    QString m_name;
    bool m_powered;
    bool m_discovering;
    bool m_discoverable;
    QMap<QString, const Device *> m_devices;
    //按序存放设备id,确定设备显示顺序
    QList<QString> m_devicesId;
};

} // namespace bluetooth
} // namespace dcc

#endif // DCC_BLUETOOTH_ADAPTER_H
