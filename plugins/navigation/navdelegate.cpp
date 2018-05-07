/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     listenerri <190771752ri@gmail.com>
 *
 * Maintainer: listenerri <190771752ri@gmail.com>
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

#include "navdelegate.h"
#include "navmodel.h"

#include <QPainter>

NavDelegate::NavDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

void NavDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    bool isHover = index.data(NavModel::NavHoverRole).toBool();

    QRect rect = QRect(option.rect.left(), option.rect.top(), option.rect.width()-1, option.rect.height()-1);

    // draw background
    if (isHover) {
        painter->fillRect(rect, QColor(255, 255, 255, 25.5));
    } else {
        painter->fillRect(rect, QColor(255, 255, 255, 7.65));
    }

    QString moduleName = index.data(Qt::WhatsThisRole).toString();
    if (!moduleName.isEmpty()) {
        QPixmap modulePm = QPixmap(QString(":/icons/nav_%1.png").arg(moduleName));
        painter->drawPixmap(option.rect.center().x() - modulePm.width()/2,
                            option.rect.center().y() - modulePm.height()/2,
                            modulePm);
    }

    QStyledItemDelegate::paint(painter, option, index);
}
