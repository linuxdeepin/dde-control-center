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

#include "displayitemdelegate.h"
#include "displaycontrolmodel.h"
#include "widgets/basiclistdelegate.h"

#include <QPainter>

DisplayItemDelegate::DisplayItemDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{

}

void DisplayItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    painter->setPen(QColor::fromRgbF(1, 1, 1, 0.1));

    if (!index.data(DisplayControlModel::ItemIsLastRole).toBool())
        painter->drawLine(QPoint(60, option.rect.bottom()), QPoint(option.rect.right() - 30, option.rect.bottom()));

    const QPixmap &iconPix = index.data(DisplayControlModel::ItemIconRole).value<QPixmap>();

    const int icon_x = 15;
    const int icon_y = option.rect.top() + (option.rect.height() - iconPix.height() / qApp->devicePixelRatio()) / 2;
    painter->drawPixmap(icon_x, icon_y, iconPix);

    if (index.data(DisplayControlModel::ItemSelectedRole).toBool())
    {
        const int x = option.rect.right() - 24;
        const int y = option.rect.top() + (option.rect.height() - 16) / 2;

        painter->drawPixmap(x, y, loadPixmap(":/frame/themes/dark/icons/select.svg"));
    }

    const int name_x = 65;
    const int name_y = option.rect.top() + 20;
    painter->setPen(Qt::white);
    painter->drawText(name_x, name_y, index.data(DisplayControlModel::ItemTitleRole).toString());

    QRect descRect = option.rect;
    descRect.setTopLeft(QPoint(name_x, name_y + 5));
    descRect.setRight(option.rect.right() - 30);
    painter->setPen(QColor(255, 255, 255, .6 * 255));
    painter->drawText(descRect, Qt::AlignLeft | Qt::TextWordWrap, index.data(DisplayControlModel::ItemDescriptionRole).toString());
}

QSize DisplayItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    return index.data(Qt::SizeHintRole).toSize();
}
