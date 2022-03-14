/*
* Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
*
* Author:     caixiangrong <caixiangrong@uniontech.com>
*
* Maintainer: caixiangrong <caixiangrong@uniontech.com>
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
#ifndef LISTITEMDELEGATE_H
#define LISTITEMDELEGATE_H

#include "interface/namespace.h"

#include <DStyledItemDelegate>

namespace DCC_NAMESPACE {

class ListItemDelegate : public Dtk::Widget::DStyledItemDelegate
{
public:
    ListItemDelegate(QAbstractItemView *parent = nullptr);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    virtual void drawDecoration(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const;
    void drawBackground(const QStyle *style, QPainter *painter, const QStyleOptionViewItem &option) const;
    void drawDisplay(const QStyle *style, QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const;
    void drawEllipse(QPainter *painter, const QStyleOptionViewItem &option, const int message) const;
    void drawFocus(const QStyle *style, QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const;
};

}

#endif // LISTITEMDELEGATE_H
