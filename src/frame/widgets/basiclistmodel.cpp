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

#include "widgets/basiclistmodel.h"

#include <QSize>
#include <QDebug>

namespace dcc {

namespace widgets {

BasicListModel::BasicListModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

int BasicListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_options.size();
}

QVariant BasicListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
    case ItemTextRole:      return m_options[index.row()];
    case ItemSizeRole:      return QSize(0, 36);
    case ItemSelectedRole:  return m_selectedIndex == index;
    case ItemHoverRole:     return m_hoveredIndex == index;
    default:;
    }

    return QVariant();
}

void BasicListModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, m_options.size());
    m_options.clear();
    m_values.clear();
    endRemoveRows();
}

void BasicListModel::appendOption(const QString &text, const QVariant &data)
{
    beginInsertRows(QModelIndex(), m_options.size(), m_options.size());
    m_options.append(text);
    m_values.append(data);
    endInsertRows();

    //    Q_EMIT layoutChanged();
}

void BasicListModel::setSelectedIndex(const QModelIndex &index)
{
    const QModelIndex oldIndex = m_selectedIndex;

    m_selectedIndex = index;

    Q_EMIT dataChanged(oldIndex, oldIndex);
    Q_EMIT dataChanged(index, index);
}

void BasicListModel::setHoveredIndex(const QModelIndex &index)
{
    m_hoveredIndex = index;

    Q_EMIT dataChanged(index, index);
}

}

}
