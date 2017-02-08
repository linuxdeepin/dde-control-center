/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "../basiclistview.h"
#include "bluetoothlist.h"
#include <QVBoxLayout>

BluetoothList::BluetoothList(BluetoothModel *model, QWidget *parent)
    : QWidget(parent),
      m_model(new BluetoothListModel(model))
{
    BasicListView *listView = new BasicListView;
    listView->setModel(m_model);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addStretch();
    centralLayout->addWidget(listView);
    centralLayout->setMargin(0);

    setLayout(centralLayout);
}
