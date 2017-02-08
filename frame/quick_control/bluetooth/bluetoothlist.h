/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef BLUETOOTHLIST_H
#define BLUETOOTHLIST_H

#include "bluetooth/bluetoothmodel.h"
#include "bluetoothlistmodel.h"
#include <QWidget>

using dcc::bluetooth::BluetoothModel;

class BluetoothList : public QWidget
{
    Q_OBJECT
public:
    explicit BluetoothList(BluetoothModel *model, QWidget *parent = 0);

private:
    BluetoothListModel *m_model;
};

#endif // BLUETOOTHLIST_H
