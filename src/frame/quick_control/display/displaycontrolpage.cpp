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

#include "displaycontrolpage.h"
#include "displaycontrolmodel.h"
#include "displayitemdelegate.h"
#include "widgets/basiclistview.h"

#include <QVBoxLayout>
#include <QEvent>

using dcc::widgets::BasicListView;
using namespace dcc::display;

DisplayControlPage::DisplayControlPage(DisplayModel *model, QWidget *parent)
    : QWidget(parent)
{

    DisplayControlModel *listModel = new DisplayControlModel(model);
    DisplayItemDelegate *delegate = new DisplayItemDelegate;
    BasicListView *listView = new BasicListView;
    listView->setModel(listModel);
    listView->setItemDelegate(delegate);
    listView->installEventFilter(this);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addStretch();
    mainLayout->addWidget(listView);
    mainLayout->setMargin(0);

    setLayout(mainLayout);

    connect(listView, &BasicListView::clicked, this, &DisplayControlPage::onItemClicked);
}

bool DisplayControlPage::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched);

    if (event->type() == QEvent::Leave)
        emit mouseLeaveView();

    return false;
}

void DisplayControlPage::onItemClicked(const QModelIndex &index) const
{
    const DisplayControlModel::ItemType type = index.data(DisplayControlModel::ItemTypeRole).value<DisplayControlModel::ItemType>();

    switch (type)
    {
    case DisplayControlModel::Duplicate:
        emit requestDuplicateMode();
        return;
    case DisplayControlModel::Extend:
        emit requestExtendMode();
        return;
    case DisplayControlModel::Custom:
        emit requestConfig(index.data(DisplayControlModel::ItemConfigNameRole).toString());
        return;
    case DisplayControlModel::NewConfig:
        emit requestCustom();
        return;
    default:;
    }

    Q_ASSERT(type == DisplayControlModel::Specified);

    emit requestOnlyMonitor(index.data(DisplayControlModel::ItemNameRole).toString());
}
