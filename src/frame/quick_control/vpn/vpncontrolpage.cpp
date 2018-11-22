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

#include "vpncontrolpage.h"
#include "vpnlistdelegate.h"
#include "widgets/basiclistview.h"

#include <QDebug>
#include <QListView>
#include <QVBoxLayout>
#include <QEvent>

using dcc::widgets::BasicListView;
using dde::network::NetworkModel;

VpnControlPage::VpnControlPage(NetworkModel *model, QWidget *parent)
    : QWidget(parent),

      m_listModel(new VpnListModel(model, this))
{
    BasicListView *listView = new BasicListView;
    VpnListDelegate *listDelegate = new VpnListDelegate;

    listView->setModel(m_listModel);
    listView->setItemDelegate(listDelegate);
    listView->installEventFilter(this);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addStretch();
    mainLayout->addWidget(listView);
    mainLayout->setMargin(0);

    setLayout(mainLayout);

    connect(listView, &BasicListView::entered, m_listModel, &VpnListModel::setHoveredIndex);
    connect(listView, &BasicListView::clicked, this, &VpnControlPage::onItemClicked);
}

bool VpnControlPage::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched);

    if (event->type() == QEvent::Leave)
        emit mouseLeaveView();

    return false;
}

void VpnControlPage::onItemClicked(const QModelIndex &index) const
{
    if (index.data(VpnListModel::VpnDisableRole).toBool())
        return;

    const VpnListModel::VpnState state = index.data(VpnListModel::VpnStateRole).value<VpnListModel::VpnState>();

    if (state == VpnListModel::NotActive)
        emit requestActivateConnection("/", index.data(VpnListModel::VpnUuidRole).toString());
    else
        emit requestDisconnect(index.data(VpnListModel::VpnUuidRole).toString());
}
