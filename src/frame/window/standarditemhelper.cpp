/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     justforlxz <justforlxz@outlook.com>
 *
 * Maintainer: justforlxz <justforlxz@outlook.com>
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

#include "standarditemhelper.h"

#include <QListView>
#include <QStandardItemModel>

StandardItemHelper::StandardItemHelper(const QListView *const listView, const QStandardItemModel *const model, QObject *parent)
    : QObject(parent)
    , m_listView(listView)
    , m_model(model)
{
    connect(listView, &QListView::clicked, this, &StandardItemHelper::onListViewClicked);
}

void StandardItemHelper::onListViewClicked(const QModelIndex &index)
{
    if (m_previousIndex.isValid()) {
        QStandardItem* previousItem = m_model->itemFromIndex(m_previousIndex);
        previousItem->setCheckState(Qt::Unchecked);
    }

    QStandardItem* item = m_model->itemFromIndex(index);
    item->setCheckState(Qt::Checked);

    m_previousIndex = index;
}
