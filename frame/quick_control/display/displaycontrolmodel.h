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

#ifndef DISPLAYCONTROLMODEL_H
#define DISPLAYCONTROLMODEL_H

#include "display/displaymodel.h"

#include <QAbstractListModel>

class DisplayControlModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit DisplayControlModel(dcc::display::DisplayModel *model, QObject *parent = 0);

    enum DisplayRole
    {
        ItemTitleRole = Qt::DisplayRole,
        UnusedRole = Qt::UserRole,
        ItemDescriptionRole,
        ItemTypeRole,
        ItemNameRole,
        ItemIsLastRole,
        ItemSelectedRole,
        ItemIconRole,
        ItemConfigNameRole,
    };

    enum ItemType
    {
        Duplicate,
        Extend,
        Specified,
        Custom,
        NewConfig,
    };

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

private:
    ItemType optionType(const int index) const;
    const QPixmap optionIcon(const int index) const;
    const QString optionName(const int index) const;
    const QString optionDescription(const int index) const;

private slots:
    void onDisplayModeChanged(const int mode);

private:
    QModelIndex m_selectedIndex;

    dcc::display::DisplayModel *m_displayModel;
};

Q_DECLARE_METATYPE(DisplayControlModel::ItemType)

#endif // DISPLAYCONTROLMODEL_H
