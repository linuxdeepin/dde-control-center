/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
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

#include "mysortfilterproxymodel.h"

#include <QVariant>
#include <QModelIndex>
#include <QDebug>

using namespace DCC_NAMESPACE::accounts;

MySortFilterProxyModel::MySortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

//bool MySortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
//{
//    QVariant leftData = sourceModel()->data(left);
//    QVariant rightData = sourceModel()->data(right);

//    qDebug() << leftData << rightData;
//    if (leftData.type() == QVariant::ULongLong && rightData.type() == QVariant::ULongLong) {
//        return leftData.toULongLong() < rightData.toULongLong();
//    }

//    return QSortFilterProxyModel::lessThan(left, right);
//}
