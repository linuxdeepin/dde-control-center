/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     lq <longqi_cm@deepin.com>
 *
 * Maintainer: lq <longqi_cm@deepin.com>
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

#include "monitorinfodelegate.h"
#include <QFontMetrics>
#include <QPainter>

using namespace DCC_NAMESPACE::display;

MonitorInfoDelegate::MonitorInfoDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{

}

void MonitorInfoDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

    auto rect = option.rect;
    QPointF imagepos = rect.topLeft();
    painter->setPen(Qt::transparent);
    painter->setBrush(QBrush(QColor(0xcd, 0xcd, 0xcd)));
    painter->drawRoundRect(rect.x() + 20, rect.top() + 5
                           , rect.width() - 40, rect.height() - 10, 5, 5);

    int xmargin = 10;
    int ymargin = 5;
    QFontMetrics fm(painter->font());
    imagepos += QPointF(80, 20);
    painter->setPen(Qt::black);
    QString str = index.data(0).toString();
    painter->drawText(imagepos, str);

    imagepos += QPointF(0, ymargin + fm.boundingRect(str).height());
    str =  QString("%1 Inch").arg(index.data(1).toFloat());
    painter->drawText(imagepos, str);

    imagepos += QPointF(xmargin + fm.boundingRect(str).width(), 0);
    auto resoStr = QString("resolution %1 x %2").arg(index.data(2).toInt()).arg(index.data(3).toInt());
    painter->drawText(imagepos, resoStr);

}

QSize MonitorInfoDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    return index.data(Qt::SizeHintRole).toSize();
}
