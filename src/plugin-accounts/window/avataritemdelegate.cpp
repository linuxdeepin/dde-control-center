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
#include "avatarlistwidget.h"

#include <DStyle>

#include <QObject>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QPainter>
#include <QPixmap>
#include <QPen>
#include <QSize>
#include <QRect>
#include <QDebug>
#include <QPainterPath>

DWIDGET_USE_NAMESPACE
DCC_USE_NAMESPACE

AvatarItemDelegate::AvatarItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void AvatarItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->setRenderHints(painter->renderHints()
                            | QPainter::Antialiasing
                            | QPainter::SmoothPixmapTransform);

    if (!index.isValid())
        return;

    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);
    auto style = opt.widget->style();

    auto pm = static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderWidth);
    int borderWidth = style->pixelMetric(pm, &opt, nullptr);
    pm = static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderSpacing);
    int borderSpacing = style->pixelMetric(pm, &opt, nullptr);
    const QMargins margins(borderWidth + borderSpacing, borderWidth + borderSpacing,
                           borderWidth + borderSpacing, borderWidth + borderSpacing);
    QPixmap pixmap = index.data(Qt::DecorationRole).value<QPixmap>();
    QPainterPath path;
    path.addEllipse(opt.rect.marginsRemoved(margins));
    painter->setClipPath(path);

    if (!pixmap.isNull()) {
        painter->drawPixmap(opt.rect.marginsRemoved(margins), pixmap);
        painter->setClipping(false);
    } else {
        qreal tw = opt.rect.width() / 3.0;
        qreal th = opt.rect.height() / 3.0;

        //绘制背景
        DStyleHelper dh(style);
        QRectF rect(tw + opt.rect.x(), th + opt.rect.y(), tw, th);
        rect.moveCenter(QRect(opt.rect).center());
        painter->setPen(Qt::NoPen);
        painter->setBrush(dh.getColor(&opt, QPalette::Button));
        painter->drawEllipse(opt.rect.marginsRemoved(margins));

        //画+号
        qreal x1 = opt.rect.x() + tw ;
        qreal y1 = opt.rect.y() + opt.rect.height() / 2.0 - 0.5;
        qreal x2 = opt.rect.x() + opt.rect.width() / 2.0 - 0.5;
        qreal y2 = opt.rect.y() + th;
        painter->setBrush(dh.getColor(&opt, QPalette::Text));
        painter->drawRect(QRectF(x1, y1, tw, 1.0));
        painter->drawRect(QRectF(x2, y2, 1.0, th));
    }

    if (index.data(Qt::CheckStateRole) == Qt::Checked) {
        painter->setPen(QPen(opt.palette.highlight(), borderWidth));
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(opt.rect.adjusted(1, 1, -1, -1));

        //在中间绘制选中小图标
        int radius = 8;
        int cx = opt.rect.marginsRemoved(margins).center().x();
        int cy = opt.rect.marginsRemoved(margins).center().y();
        QRect crect(QPoint(cx - radius, cy - radius), QPoint(cx + radius, cy + radius));
        opt.rect = crect;
        opt.state |= QStyle::State_On;
        style->drawPrimitive(DStyle::PE_IndicatorItemViewItemCheck, &opt, painter, nullptr);
        return;
    }
    // draw + in the end
}

QSize AvatarItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    return index.data(Qt::SizeHintRole).toSize();
}
