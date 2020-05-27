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

#include "navigationdelegate.h"
#include "navigationmodel.h"
#include "widgets/basiclistdelegate.h"

#include <QPainter>
#include <QDebug>

namespace dcc {

NavigationDelegate::NavigationDelegate(QObject *parent)
    : QItemDelegate(parent)
{

}

void NavigationDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const NavigationModel::EdgeFlags edgeFlags = index.data(NavigationModel::ItemEdgeRole).value<NavigationModel::EdgeFlags>();
    const int r = 5;
    const int ox = option.rect.topLeft().x();
    const int oy = option.rect.topLeft().y();
    const int w = option.rect.width();
    const int h = option.rect.height();

    QPainterPath path(option.rect.topLeft());
    path.addRect(option.rect);

    if (edgeFlags.testFlag(NavigationModel::Top))
    {
        if (edgeFlags.testFlag(NavigationModel::Left))
        {
            // top-left
            QPainterPath sub(option.rect.topLeft());
            sub.lineTo(ox + r, oy);
            sub.arcTo(ox, oy, r * 2, r * 2, 90, 90);
            sub.closeSubpath();

            path = path.subtracted(sub);
        }
        else if (edgeFlags.testFlag(NavigationModel::Right))
        {
            // top-right
            QPainterPath sub(QPoint(ox + w, oy));
            sub.lineTo(ox + w - r, oy);
            sub.arcTo(ox + w - r * 2, oy, r * 2, r * 2, 90, -90);
            sub.closeSubpath();

            path = path.subtracted(sub);
        }
    }
    else if (edgeFlags.testFlag(NavigationModel::Bottom))
    {
        if (edgeFlags.testFlag(NavigationModel::Left))
        {
            // bottom-left
            QPainterPath sub(QPoint(ox, oy + h));
            sub.lineTo(ox, oy + h - r);
            sub.arcTo(ox, oy + h - r * 2, r * 2, r * 2, 180, 90);
            sub.closeSubpath();

            path = path.subtracted(sub);
        }
        else if (edgeFlags.testFlag(NavigationModel::Right))
        {
            // bottom-right
            QPainterPath sub(QPoint(ox + w, oy + h));
            sub.lineTo(ox + w, oy + h - r);
            sub.arcTo(ox + w - r * 2, oy + h - r * 2, r * 2, r * 2, 0, -90);
            sub.closeSubpath();

            path = path.subtracted(sub);
        }
    }

    painter->setPen(Qt::NoPen);
    painter->setRenderHint(QPainter::Antialiasing);
    if (index.data(NavigationModel::ItemHoveredRole).toBool())
        painter->fillPath(path, QColor::fromRgbF(1, 1, 1, 0.3));
    else
        painter->fillPath(path, QColor::fromRgbF(1, 1, 1, 0.2));

    const QString module = index.data().toString();
    if (module.isEmpty())
        return;

    const QString picFile = QString(":/%1/themes/dark/icons/nav_%1.svg").arg(module);

    QRect pixRect;
    QPixmap pixmap(loadPixmap(picFile));
    const qreal devicePixelRatio = qApp->devicePixelRatio();

    if (devicePixelRatio > 1.0) {
        pixRect = QRect(0, 0, pixmap.width() / devicePixelRatio, pixmap.height() / devicePixelRatio);
    } else {
        pixRect = pixmap.rect();
    }

    pixRect.moveCenter(option.rect.center());

    painter->drawPixmap(pixRect, pixmap);
}

QSize NavigationDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);

    return index.data(Qt::SizeHintRole).toSize();
}

}
