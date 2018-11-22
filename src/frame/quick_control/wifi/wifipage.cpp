/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "wifipage.h"
#include "wifilistmodel.h"
#include "wifilistdelegate.h"
#include "widgets/basiclistview.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QEvent>

#include <networkmodel.h>

using dde::network::NetworkModel;
using dcc::widgets::BasicListView;

WifiPage::WifiPage(NetworkModel *model, QWidget *parent)
    : QWidget(parent),

      m_networkModel(model)
{

    WifiListModel *listModel = new WifiListModel(model);
    WifiListDelegate *delegate = new WifiListDelegate;
    BasicListView *listView = new BasicListView;
    listView->setModel(listModel);
    listView->setItemDelegate(delegate);
    listView->installEventFilter(this);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addStretch();
    centralLayout->addWidget(listView);
    centralLayout->setMargin(0);

    setLayout(centralLayout);

    connect(listModel, &WifiListModel::requestDeviceApList, this, &WifiPage::requestDeviceApList, Qt::QueuedConnection);
    connect(listView, &BasicListView::entered, listModel, &WifiListModel::setCurrentHovered);
    connect(listView, &BasicListView::clicked, listModel, &WifiListModel::setCurrentActivating);
    connect(listView, &BasicListView::clicked, this, &WifiPage::onItemClicked);
}

bool WifiPage::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched);

    if (event->type() == QEvent::Leave)
        emit mouseLeaveView();

    return false;
}

void WifiPage::onItemClicked(const QModelIndex &index)
{
    if (index.data(WifiListModel::ItemIsHeaderRole).toBool() || index.data(WifiListModel::ItemIsPowerOffRole).toBool())
        return;

    if (index.data(WifiListModel::ItemIsHiddenTipsRole).toBool())
    {
        if (index.data(WifiListModel::ItemCountRole) != 1)
            emit requestPage("network", "", false);
        else
            emit requestPage("network", "network", false);
        return;
    }

    const QString uuid = index.data(WifiListModel::ItemUuidRole).toString();
    if (index.data(WifiListModel::ItemIsActiveRole).toBool())
    {
        emit requestDeactivateConnection(uuid);
    } else {
        const QString devPath = index.data(WifiListModel::ItemDevicePathRole).toString();
        const QString apPath = index.data(WifiListModel::ItemApPathRole).toString();

        emit requestActivateAccessPoint(devPath, apPath, uuid);
    }
}
