//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
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
    bool helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index) override;

    virtual void drawDecoration(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const;
    void drawBackground(const QStyle *style, QPainter *painter, const QStyleOptionViewItem &option) const;
    void drawDisplay(const QStyle *style, QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const;
    void drawEllipse(QPainter *painter, const QStyleOptionViewItem &option, const int message) const;
    void drawFocus(const QStyle *style, QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const;
};

}

#endif // LISTITEMDELEGATE_H
