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
#include "tabitemdelegate.h"
#include "listview.h"

#include <QPainter>
#include <QIcon>
#include <QVariant>
#include <QDebug>
#include <QApplication>
#include <QAbstractItemView>

#include <DPalette>
#include <DPaletteHelper>
#include <DStyleOption>
#include <DStyle>
#include <DStyledItemDelegate>

DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
DCC_USE_NAMESPACE

TabItemDelegate::TabItemDelegate(QAbstractItemView *parent)
    : DStyledItemDelegate(parent)
{
}

void TabItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();

    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);

    bool isIconMode = option.decorationAlignment == Qt::AlignCenter;
    bool isBeginning = (opt.viewItemPosition == QStyleOptionViewItem::ViewItemPosition::Beginning) || (opt.viewItemPosition == QStyleOptionViewItem::ViewItemPosition::OnlyOne);
    // 选择高亮背景
    if (opt.state & QStyle::State_Selected) {
        QPalette::ColorGroup cg = (option.state & QStyle::State_Enabled)
                                          ? QPalette::Normal
                                          : QPalette::Disabled;
        opt.backgroundBrush = option.palette.color(cg, QPalette::Highlight);
    }

    QVariant value;

    QRect decorationRect;
    value = index.data(Qt::DecorationRole);
    if (value.isValid()) {
        if (isBeginning && isIconMode) {
            decorationRect = QRect(opt.rect.topLeft() + QPoint((opt.rect.width() - opt.decorationSize.width()) / 2, 15), opt.decorationSize);
            opt.displayAlignment = Qt::AlignCenter;
        } else if (isBeginning) {
            opt.decorationSize += QSize(4, 4);
            decorationRect = QRect(opt.rect.topLeft() + QPoint(8, (opt.rect.height() - opt.decorationSize.height()) / 2), opt.decorationSize);
            opt.displayAlignment = Qt::AlignLeft;
        } else {
            decorationRect = QRect(opt.rect.topLeft() + QPoint(8, (opt.rect.height() - opt.decorationSize.height()) / 2), opt.decorationSize);
            opt.displayAlignment = Qt::AlignLeft;
        }
    } else {
        decorationRect = QRect();
    }
    int fontHeight = opt.widget->fontMetrics().height();
    QString text;
    QRect displayRect;
    value = index.data(Qt::DisplayRole);
    if (value.isValid() && !value.isNull()) {
        if (isBeginning && isIconMode) {
            displayRect = QRect(opt.rect.topLeft() + QPoint(0, opt.decorationSize.height() + 40), QSize(opt.rect.width(), -1));
        } else if (isBeginning || isIconMode) {
            displayRect = QRect(opt.rect.topLeft() + QPoint(decorationRect.width() + 18, (opt.rect.height() / 2 - fontHeight+5)), QSize(opt.rect.width() - opt.decorationSize.width() - 30, -1));
        } else {
            displayRect = QRect(opt.rect.topLeft() + QPoint(decorationRect.width() + 18, (opt.rect.height() - fontHeight) / 2), QSize(opt.rect.width() - opt.decorationSize.width() - 30, -1));
        }
    }

    QStyle *style = option.widget ? option.widget->style() : QApplication::style();
    // draw the item
    if (opt.state & QStyle::State_Selected
        //|| opt.state & QStyle::State_Active
        || opt.state & QStyle::State_MouseOver)
        drawBackground(style, painter, opt);
    // 图标的绘制用也可能会使用这些颜色
    QPalette::ColorGroup cg = (opt.state & QStyle::State_Enabled) ? QPalette::Normal : QPalette::Disabled;
    painter->setPen(opt.palette.color(cg, (opt.state & QStyle::State_Selected) ? QPalette::HighlightedText : QPalette::Text));

//    drawDecoration(painter, opt, decorationRect);
    opt.displayAlignment = Qt::AlignCenter;
    drawDisplay(style, painter, opt, opt.rect);

    //drawFocus(style, painter, opt, opt.rect);
    // done
    painter->restore();
}

void TabItemDelegate::drawBackground(const QStyle *style, QPainter *painter, const QStyleOptionViewItem &option) const
{
    DStyleOptionBackgroundGroup boption;
    boption.init(option.widget);
    boption.QStyleOption::operator=(option);
    boption.position = DStyleOptionBackgroundGroup::ItemBackgroundPosition(option.viewItemPosition);

    if (option.backgroundBrush.style() != Qt::NoBrush) {
        boption.dpalette.setBrush(DPalette::ItemBackground, option.backgroundBrush);
    }

    boption.rect = option.rect;

    if (backgroundType() != RoundedBackground) {
        boption.directions = Qt::Vertical;
    }

    style->drawPrimitive(static_cast<QStyle::PrimitiveElement>(DStyle::PE_ItemBackground), &boption, painter, option.widget);
}

void TabItemDelegate::drawDecoration(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const
{
    if (option.features & QStyleOptionViewItem::HasDecoration) {
        QIcon::Mode mode = QIcon::Normal;
        if (!(option.state & QStyle::State_Enabled))
            mode = QIcon::Disabled;
        else if (option.state & QStyle::State_Selected)
            mode = QIcon::Selected;
        QIcon::State state = (option.state & QStyle::State_Open) ? QIcon::On : QIcon::Off;
        option.icon.paint(painter, rect, option.decorationAlignment, mode, state);
    }
}

void TabItemDelegate::drawDisplay(const QStyle *style, QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const
{
    DStyle::viewItemDrawText(style, painter, &option, rect);
}

void TabItemDelegate::drawEllipse(QPainter *painter, const QStyleOptionViewItem &option, const int message) const
{
    if (message <= 0)
        return;

    QRadialGradient Conical(0, 0, 7, 0, 0);
    Conical.setColorAt(1, QColor(255, 106, 106, 255));
    Conical.setColorAt(0.2, QColor(255, 106, 106, 255));
    Conical.setColorAt(0, QColor(255, 106, 106, 25));

    painter->setBrush(Conical);
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(option.rect.center() + QPoint(option.rect.width() / 2 - 30, 0), 7, 7);
}

void TabItemDelegate::drawFocus(const QStyle *style, QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const
{
    if ((option.state & QStyle::State_HasFocus) == 0 || !rect.isValid())
        return;
    QStyleOptionFocusRect o;
    o.QStyleOption::operator=(option);
    o.rect = rect;
    o.state |= QStyle::State_KeyboardFocusChange;
    o.state |= QStyle::State_Item;
    QPalette::ColorGroup cg = (option.state & QStyle::State_Enabled)
                                      ? QPalette::Normal
                                      : QPalette::Disabled;
    o.backgroundColor = option.palette.color(cg, (option.state & QStyle::State_Selected)
                                                         ? QPalette::Highlight
                                                         : QPalette::Window);
    style->drawPrimitive(QStyle::PE_FrameFocusRect, &o, painter, option.widget);
}
