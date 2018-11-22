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

#ifndef BASICLISTMODEL_H
#define BASICLISTMODEL_H

#include <QAbstractListModel>

namespace dcc {

namespace widgets {

class BasicListModel : public QAbstractListModel
{
    Q_OBJECT

public:

    enum ItemRole
    {
        ItemSizeRole = Qt::SizeHintRole,
        ItemTextRole = Qt::DisplayRole,
        ReservedRole = Qt::UserRole,
        ItemIsFirstRole,
        ItemIsLastRole,
        ItemSelectedRole,
        ItemHoverRole
    };

    explicit BasicListModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

public Q_SLOTS:
    void clear();
    void appendOption(const QString &text, const QVariant &data = QVariant());
    void setSelectedIndex(const QModelIndex &index);
    void setHoveredIndex(const QModelIndex &index);

private:
    QList<QString> m_options;
    QList<QVariant> m_values;

    QModelIndex m_selectedIndex;
    QModelIndex m_hoveredIndex;
};

}

}

#endif // BASICLISTMODEL_H
