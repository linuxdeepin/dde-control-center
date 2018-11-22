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

#ifndef NAVIGATIONMODEL_H
#define NAVIGATIONMODEL_H

#include <QAbstractListModel>
#include <QFlags>

namespace dcc {

class NavigationModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit NavigationModel(QObject *parent = 0);

    enum NavigationItemRole
    {
        ItemReservedRole = Qt::UserRole,
        ItemHoveredRole,
        ItemEdgeRole,
    };

    enum EdgeFlag
    {
        Top     =   1 << 0,
        Left    =   1 << 1,
        Bottom  =   1 << 2,
        Right   =   1 << 3,
    };
    Q_DECLARE_FLAGS(EdgeFlags, EdgeFlag)

public Q_SLOTS:
    void insertItem(const QString &item);
    void removeItem(const QString &item);
    void appendAvailableItem(const QString &item);

    void setCurrentItem(const QModelIndex &index);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

private:
    NavigationModel::EdgeFlags indexEdgeFlag(const QModelIndex &index) const;

private:
    QStringList m_items;
    QStringList m_allItems;

    QModelIndex m_currentIndex;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(dcc::NavigationModel::EdgeFlags)
Q_DECLARE_METATYPE(dcc::NavigationModel::EdgeFlags)

#endif // NAVIGATIONMODEL_H
