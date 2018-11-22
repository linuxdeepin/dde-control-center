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

#include "navigationmodel.h"

#include <QSize>
#include <QDebug>

namespace dcc {

NavigationModel::NavigationModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

void NavigationModel::insertItem(const QString &item)
{
    if (m_items.contains(item))
        return;

    int idx = 0;
    for (const auto it : m_allItems)
    {
        if (it == item)
            break;
        if (m_items.contains(it))
            ++idx;
    }

    m_items.insert(idx, item);

    Q_EMIT layoutChanged();
}

void NavigationModel::removeItem(const QString &item)
{
    const int idx = m_items.indexOf(item);

    if (idx == -1)
        return;

    m_items.removeAt(idx);

    Q_EMIT layoutChanged();
}

void NavigationModel::appendAvailableItem(const QString &item)
{
    m_allItems.append(item);
}

void NavigationModel::setCurrentItem(const QModelIndex &index)
{
    const QModelIndex oldIndex = m_currentIndex;

    m_currentIndex = index;

    Q_EMIT dataChanged(oldIndex, oldIndex);
    Q_EMIT dataChanged(m_currentIndex, m_currentIndex);
}

int NavigationModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    const int s = m_items.size();
    const int r = s % 3;

    return s + (r ? 3 - r : r);
}

QVariant NavigationModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
    {
        const int idx = index.row();
        if (m_items.size() > idx)
            return m_items[idx];
        else
            return QVariant();
    }
    case Qt::SizeHintRole:  return QSize(110, 90);
    case ItemHoveredRole:   return index == m_currentIndex;
    case ItemEdgeRole:      return QVariant::fromValue(indexEdgeFlag(index));
    default:;
    }

    return QVariant();
}

NavigationModel::EdgeFlags NavigationModel::indexEdgeFlag(const QModelIndex &index) const
{
    const int idx = index.row();
    const int r = idx % 3;
    const int h = m_items.size() / 3 - !(m_items.size() % 3);
    EdgeFlags flag = 0;

    if (idx < 3)
        flag |= Top;
    if (idx / 3 == h)
        flag |= Bottom;
    if (r == 0)
        flag |= Left;
    if (r == 2)
        flag |= Right;

    return flag;
}

}
