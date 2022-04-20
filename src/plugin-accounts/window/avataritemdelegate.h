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

#pragma once

#include "interface/namespace.h"

#include <QStyledItemDelegate>
#include <QMetaType>

struct LastItemData {
    bool isDrawLast;
    QString iconPath;
};
Q_DECLARE_METATYPE(LastItemData)

QT_BEGIN_NAMESPACE
class QObject;
class QStyleOptionViewItem;
class QModelIndex;
class QPainter;
class QSize;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {

class AvatarItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit AvatarItemDelegate(QObject *parent = nullptr);

    // painting
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    // set item size
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};
} // DCC_NAMESPACE
