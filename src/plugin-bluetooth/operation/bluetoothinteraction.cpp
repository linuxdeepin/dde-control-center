// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "bluetoothinteraction.h"
#include "dccfactory.h"

#include <QtQml/qqml.h>

BluetoothInteraction::BluetoothInteraction(QObject *parent)
    : QObject{ parent }
    , m_model(new BluetoothModel(this))
    , m_work(new BluetoothWorker(m_model, this))
{
    m_work->activate();

    qmlRegisterType<BluetoothWorker>("dcc", 1, 0, "BluetoothWorker");
    qmlRegisterType<BluetoothModel>("dcc", 1, 0, "BluetoothModel");
}

BluetoothModel *BluetoothInteraction::model() const
{
    return m_model;
}

void BluetoothInteraction::setModel(BluetoothModel *newModel)
{
    m_model = newModel;
}

BluetoothWorker *BluetoothInteraction::work() const
{
    return m_work;
}

void BluetoothInteraction::setWork(BluetoothWorker *newWork)
{
    m_work = newWork;
}


DCC_FACTORY_CLASS(BluetoothInteraction)

#include "bluetoothinteraction.moc"
