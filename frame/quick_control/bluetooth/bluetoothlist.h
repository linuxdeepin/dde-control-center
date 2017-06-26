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
#include "basiclistview.h"
#include <QWidget>

using dcc::bluetooth::BluetoothModel;
using dcc::widgets::BasicListView;

class BluetoothList : public QWidget
{
    Q_OBJECT
public:
    explicit BluetoothList(BluetoothModel *model, QWidget *parent = 0);

signals:
    void mouseLeaveView() const;
    void requestConnect(const Device *device) const;
    void requestDisConnect(const Device *device) const;
    void requestConnectOther(const QString &module, const QString &page) const;
    void requestAdapterDiscoverable(const QString &id) const;

private:
    bool eventFilter(QObject *watched, QEvent *event);

private slots:
    void onItemClicked(const QModelIndex &index) const;

private:
    BluetoothListModel *m_model;
    BluetoothModel     *m_btModel;
};

#endif // BLUETOOTHLIST_H
