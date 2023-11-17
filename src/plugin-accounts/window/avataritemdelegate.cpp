// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "avataritemdelegate.h"

#include "avatarlistwidget.h"

#include <DStyle>

#include <QDebug>
#include <QModelIndex>
#include <QObject>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPixmap>
#include <QRect>
#include <QSize>
#include <QStyleOptionViewItem>
#include <QStyledItemDelegate>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;

#define CustomAvatarRole 4

AvatarItemDelegate::AvatarItemDelegate(bool isCustom, QObject *parent)
    : QStyledItemDelegate(parent)
    , m_isCustom(isCustom)
{
}

void AvatarItemDelegate::paint(QPainter *painter,
                               const QStyleOptionViewItem &option,
                               const QModelIndex &index) const
{
    painter->setRenderHints(painter->renderHints() | QPainter::Antialiasing
                            | QPainter::SmoothPixmapTransform);

    if (!index.isValid())
        return;

    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);
    QStyle *style = option.widget ? option.widget->style() : QApplication::style();

    opt.rect = opt.rect.adjusted(4, 4, -4, -4);

    auto pm = static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderWidth);
    int borderWidth = style->pixelMetric(pm, &opt, nullptr);
    pm = static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderSpacing);
    int borderSpacing = style->pixelMetric(pm, &opt, nullptr);
    const QMargins margins(borderWidth + borderSpacing,
                           borderWidth + borderSpacing,
                           borderWidth + borderSpacing,
                           borderWidth + borderSpacing);
    QPixmap pixmap = index.data(Qt::DecorationRole).value<QPixmap>();
    QPainterPath path;
    path.addRoundedRect(opt.rect.marginsRemoved(margins), 8, 8);
    painter->setClipPath(path);

    if (!pixmap.isNull()) {
        painter->drawPixmap(opt.rect.marginsRemoved(margins), pixmap);
        painter->setClipping(false);
    } else {
        qreal tw = opt.rect.width() / 3.0;
        qreal th = opt.rect.height() / 3.0;

        // 绘制背景
        DStyleHelper dh(style);
        QRectF rect(tw + opt.rect.x(), th + opt.rect.y(), tw, th);
        rect.moveCenter(QRect(opt.rect).center());
        painter->setPen(Qt::NoPen);
        painter->setBrush(dh.getColor(&opt, QPalette::Button));

        painter->drawRoundedRect(opt.rect.marginsRemoved(margins), 8, 8);

        // 画+号
        qreal x1 = opt.rect.x() + tw;
        qreal y1 = opt.rect.y() + opt.rect.height() / 2.0 - 0.5;
        qreal x2 = opt.rect.x() + opt.rect.width() / 2.0 - 0.5;
        qreal y2 = opt.rect.y() + th;
        painter->setBrush(dh.getColor(&opt, QPalette::Text));
        painter->drawRect(QRectF(x1, y1, tw, 1.0));
        painter->drawRect(QRectF(x2, y2, 1.0, th));
        return;
    }

    if (m_isCustom) {
        painter->setPen(QPen(opt.palette.highlight(), borderWidth));
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(opt.rect.adjusted(1, 1, -1, -1), 8, 8);

        // 在中间绘制选中小图标
        int radius = 8;
        int cx = opt.rect.marginsRemoved(margins).right();
        int cy = opt.rect.marginsRemoved(margins).top();
        QRect crect(QPoint(cx - radius, cy - radius), QPoint(cx + radius, cy + radius));
        opt.rect = crect;
        opt.state |= QStyle::State_On;
        opt.state &= ~QStyle::State_Selected;
        if (index.data(Qt::CheckStateRole) == Qt::Checked) {
            style->drawPrimitive(DStyle::PE_IndicatorItemViewItemCheck, &opt, painter, nullptr);
        } else {
            style->drawPrimitive(DStyle::PE_IndicatorTabClose, &opt, painter, nullptr);
        } // draw + in the end
        return;
    }

    if (index.data(Qt::CheckStateRole) == Qt::Checked) {
        painter->setPen(QPen(opt.palette.highlight(), borderWidth));
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(opt.rect.adjusted(1, 1, -1, -1), 8, 8);

        // 在中间绘制选中小图标
        int radius = 8;
        int cx = opt.rect.marginsRemoved(margins).right();
        int cy = opt.rect.marginsRemoved(margins).top();
        QRect crect(QPoint(cx - radius, cy - radius), QPoint(cx + radius, cy + radius));
        opt.rect = crect;
        opt.state |= QStyle::State_On;
        opt.state &= ~QStyle::State_Selected;
        style->drawPrimitive(DStyle::PE_IndicatorItemViewItemCheck, &opt, painter, nullptr);
        return;
    } // draw + in the end
}

QSize AvatarItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    return index.data(Qt::SizeHintRole).toSize();
}
