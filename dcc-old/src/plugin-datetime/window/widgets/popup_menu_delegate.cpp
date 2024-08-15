// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "popup_menu_delegate.h"

#include <QPainter>

namespace installer {

PopupMenuDelegate::PopupMenuDelegate(QWidget *parent)
    : QStyledItemDelegate(parent)
{
    this->setObjectName("popup_menu_delegate");
}

void PopupMenuDelegate::paint(QPainter *painter,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    painter->save();

    const QRect &rect(option.rect);

    if (option.state & QStyle::State_MouseOver) {
        // Draw background color of selected item, no matter it is active or not.
        // #2ca7f8
        const QColor selected_color = QColor::fromRgb(44, 167, 248);
        painter->fillRect(rect, QBrush(selected_color));
    }

    // Draw text. Default color is #303030.
    QColor text_color = QColor::fromRgb(48, 48, 48);
    if (option.state & QStyle::State_MouseOver) {
        // Change text color to white on mouse hover.
        text_color = Qt::white;
    }
    painter->setPen(QPen(text_color));
    const QString text = index.model()->data(index, Qt::DisplayRole).toString();

    // Text alignment is center.
    painter->drawText(rect, Qt::AlignHCenter | Qt::AlignVCenter, text);

    painter->restore();
}

} // namespace installer
