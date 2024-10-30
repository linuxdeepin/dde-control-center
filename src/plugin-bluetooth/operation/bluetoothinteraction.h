// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef BLUETOOTHINTERACTION_H
#define BLUETOOTHINTERACTION_H

#include <QObject>

#include "bluetoothmodel.h"
#include "bluetoothworker.h"

class BluetoothInteraction : public QObject
{
    Q_OBJECT
public:
    explicit BluetoothInteraction(QObject *parent = nullptr);

    Q_INVOKABLE BluetoothModel *model() const;
    void setModel(BluetoothModel *newModel);

    Q_INVOKABLE BluetoothWorker *work() const;
    void setWork(BluetoothWorker *newWork);

signals:

private:
    BluetoothModel* m_model;
    BluetoothWorker* m_work;
};

#endif // BLUETOOTHINTERACTION_H
