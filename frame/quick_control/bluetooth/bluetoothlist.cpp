/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "bluetoothlist.h"
#include "bluetoothdelegate.h"
#include "bluetooth/bluetoothmodel.h"
#include <QVBoxLayout>

BluetoothList::BluetoothList(BluetoothModel *model, QWidget *parent)
    : QWidget(parent),
      m_model(new BluetoothListModel(model))
{
    BasicListView *listView = new BasicListView;
    listView->setModel(m_model);

    BluetoothDelegate *delegate = new BluetoothDelegate;
    listView->setItemDelegate(delegate);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addStretch();
    centralLayout->addWidget(listView);
    centralLayout->setMargin(0);

    setLayout(centralLayout);

    connect(listView, &BasicListView::entered, m_model, &BluetoothListModel::setCurrentHovered);
    connect(listView, &BasicListView::clicked, this, &BluetoothList::onItemClicked);
}

void BluetoothList::onItemClicked(const QModelIndex &index) const
{
    QJsonObject json = index.data(BluetoothListModel::ItemInfoRole).toJsonObject();

    QMap<const Adapter *, QList<QJsonObject> > list = m_model->adapterList();
    QMap<const Adapter *, QList<QJsonObject> >::iterator map = list.begin();
    while (map != list.end()) {
        for (const Device *device : map.key()->devices()) {
            if (device->id() == json["Path"].toString()) {
                emit requestConnect(device);
                return;
            }
        }
        ++map;
    }
}
