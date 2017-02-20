/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

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
    explicit BluetoothModel(QObject *parent = 0);

    QList<const Adapter *> adapters() const;
    const Adapter *adapterById(const QString &id);

public slots:
    void addAdapter(const Adapter *adapter);
    const Adapter *removeAdapater(const QString &adapterId);

signals:
    void adapterAdded(const Adapter *adapter) const;
    void adapterRemoved(const Adapter *adapter) const;
    void adpaterListChanged();

private:
    QList<const Adapter *> m_adapters;

    friend class BluetoothWorker;
};

} // namespace bluetooth
} // namespace dcc

#endif // DCC_BLUETOOTH_BLUETOOTHMODEL_H
