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

#include "avataritemdelegate.h"

#include <DStyle>

#include <QObject>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QPainter>
#include <QPixmap>
#include <QPen>
#include <QSize>
#include <QDebug>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE::accounts;

AvatarItemDelegate::AvatarItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{

}

void AvatarItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.isValid()) {
        int borderWidth = option.widget->style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderWidth), &option, nullptr);
        int borderSpacing = option.widget->style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderSpacing), &option, nullptr);
        const QMargins margins(borderWidth + borderSpacing, borderWidth + borderSpacing, borderWidth + borderSpacing, borderWidth + borderSpacing);
        painter->setRenderHint(QPainter::Antialiasing);
        QPixmap pixmap = index.data(Qt::DecorationRole).value<QPixmap>();
        QPainterPath path;
        path.addEllipse(option.rect.marginsRemoved(margins));
        painter->setClipPath(path);
        painter->drawPixmap(option.rect.marginsRemoved(margins), pixmap);
        painter->setClipping(false);
        if (index.data(Qt::CheckStateRole).toInt() == Qt::Checked) {
            painter->setPen(QPen(option.palette.highlight(), borderWidth));
            painter->setBrush(Qt::NoBrush);
            painter->drawEllipse(option.rect.adjusted(1, 1, -1, -1));
        }
    }
}

QSize AvatarItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    return QSize(74, 74);
}
