//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "listitemdelegate.h"

#include <QPainter>
#include <QIcon>
#include <QVariant>
#include <QDebug>
#include <QApplication>
#include <QAbstractItemView>
#include <QHelpEvent>
#include <QToolTip>

#include <DPalette>
#include <DPaletteHelper>
#include <DStyleOption>
#include <DStyle>
#include <DStyledItemDelegate>
#include <DGuiApplicationHelper>
#include <DIconTheme>

#if DTK_VERSION >= DTK_VERSION_CHECK(5, 6, 0, 0)
#    define USE_DCIICON
#endif

#ifdef USE_DCIICON
#    include <DDciIcon>
#endif
DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;

ListItemDelegate::ListItemDelegate(QAbstractItemView *parent)
    : DStyledItemDelegate(parent)
{
}

void ListItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();

    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);

    bool isIconMode = option.decorationAlignment == Qt::AlignCenter;
    bool isBeginning = (opt.viewItemPosition == QStyleOptionViewItem::ViewItemPosition::Beginning) || (opt.viewItemPosition == QStyleOptionViewItem::ViewItemPosition::OnlyOne);
    bool isSelected = opt.state & QStyle::State_Selected;
    bool isHover = opt.state & QStyle::State_MouseOver;

    // 选择高亮背景
    if (isSelected) {
        QPalette::ColorGroup cg = (option.state & QStyle::State_Enabled)
                ? QPalette::Normal
                : QPalette::Disabled;
        opt.backgroundBrush = option.palette.color(cg, QPalette::Highlight);
    }

    QVariant value;

    QRect decorationRect;
    int fontHeight = opt.widget->fontMetrics().height();
    const int firstItemHeight = (opt.rect.height() - (fontHeight * 2 + opt.decorationSize.height())) / 2;
    if (index.data(Qt::DecorationRole).isValid()) {
        if (isBeginning && isIconMode) {
            decorationRect = QRect(opt.rect.topLeft() + QPoint((opt.rect.width() - opt.decorationSize.width()) / 2, firstItemHeight), opt.decorationSize);
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
    QString text;
    QRect displayRect;
    value = index.data(Qt::DisplayRole);
    if (value.isValid() && !value.isNull()) {
        if (isBeginning && isIconMode) {
            displayRect = QRect(opt.rect.topLeft() + QPoint(0, opt.decorationSize.height() + firstItemHeight + 10), QSize(opt.rect.width(), -1));
        } else if (isBeginning || isIconMode) {
            displayRect = QRect(opt.rect.topLeft() + QPoint(decorationRect.width() + 18, (opt.rect.height() / 2 - fontHeight + 5)), QSize(opt.rect.width() - opt.decorationSize.width() - 30, -1));
        } else {
            displayRect = QRect(opt.rect.topLeft() + QPoint(decorationRect.width() + 18, (opt.rect.height() - fontHeight) / 2), QSize(opt.rect.width() - opt.decorationSize.width() - 30, -1));
        }
    }

    QRect tipRect = displayRect.translated(0, opt.widget->fontMetrics().lineSpacing() + 3);

    QStyle *style = option.widget ? option.widget->style() : QApplication::style();
    // draw the item
    if (isIconMode || isSelected || isHover) {
        drawBackground(style, painter, opt);
    }
    // 图标的绘制用也可能会使用这些颜色
    QPalette::ColorGroup cg = (opt.state & QStyle::State_Enabled) ? QPalette::Normal : QPalette::Disabled;
    auto role = (opt.state & QStyle::State_Selected) ? QPalette::HighlightedText : QPalette::Text;
    painter->setPen(opt.palette.color(cg, role));

    drawDecoration(painter, opt, decorationRect);

    drawDisplay(style, painter, opt, displayRect);

    if (isIconMode || isBeginning) {
        painter->save();
        painter->setPen(opt.palette.color(cg, role));
        painter->setOpacity(0.7);
        opt.text = index.data(Qt::StatusTipRole).toString();
        int fontSize = opt.font.pointSize();
        opt.font.setPointSize(fontSize - 3);
        drawDisplay(style, painter, opt, tipRect);
        painter->restore();
    }

    drawEllipse(painter, opt, index.data(Dtk::RightActionListRole).toInt());
    drawFocus(style, painter, opt, opt.rect);
    // done
    painter->restore();
}

void ListItemDelegate::drawBackground(const QStyle *style, QPainter *painter, const QStyleOptionViewItem &option) const
{
    DStyleOptionBackgroundGroup boption;
    boption.init(option.widget);
    boption.QStyleOption::operator=(option);
    boption.position = DStyleOptionBackgroundGroup::ItemBackgroundPosition(option.viewItemPosition);
    bool isIconMode = option.decorationAlignment == Qt::AlignCenter;
    if (isIconMode)
        boption.dpalette.setBrush(DPalette::ItemBackground, option.palette.color(QPalette::Base));

    if (option.backgroundBrush.style() != Qt::NoBrush) {
        boption.dpalette.setBrush(DPalette::ItemBackground, option.backgroundBrush);
    }

    boption.rect = option.rect;

    if (backgroundType() != RoundedBackground) {
        boption.directions = Qt::Vertical;
    }

    style->drawPrimitive(static_cast<QStyle::PrimitiveElement>(DStyle::PE_ItemBackground), &boption, painter, option.widget);
}

bool drawDciIcon(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect)
{
#ifdef USE_DCIICON
    QVariant iconVar = option.index.data(Qt::DecorationRole);
    DDciIcon icon;
    if (iconVar.canConvert<DDciIcon>()) {
        icon = iconVar.value<DDciIcon>();
    } else if (iconVar.type() == QVariant::String) {
        QString iconstr = iconVar.toString();
        if (!iconstr.isEmpty()) {
            icon = DDciIcon::fromTheme(iconstr);
            if (icon.isNull())
                icon = DDciIcon(iconstr);
        }
    }
    if (!icon.isNull()) {
        DDciIcon::Mode dciMode = DDciIcon::Normal;
        if (option.state & QStyle::State_Enabled) {
            if (option.state & (QStyle::State_Sunken | QStyle::State_Selected)) {
                dciMode = DDciIcon::Pressed;
            } else if (option.state & QStyle::State_MouseOver) {
                dciMode = DDciIcon::Hover;
            }
        } else {
            dciMode = DDciIcon::Disabled;
        }

        DDciIcon::Theme theme = DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::DarkType ? DDciIcon::Dark : DDciIcon::Light;

        painter->save();
        painter->setBrush(Qt::NoBrush);
        icon.paint(painter, rect, painter->device() ? painter->device()->devicePixelRatioF() : qApp->devicePixelRatio(), theme, dciMode, option.decorationAlignment);
        painter->restore();
        return true;
    }
#endif
    return false;
}

void ListItemDelegate::drawDecoration(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const
{
    if (option.features & QStyleOptionViewItem::HasDecoration
        && !drawDciIcon(painter, option, rect)) {
        QVariant iconVar = option.index.data(Qt::DecorationRole);
        QIcon icon;
        if (iconVar.type() == QVariant::Icon) {
            icon = iconVar.value<QIcon>();
        } else if (iconVar.type() == QVariant::String) {
            const QString &iconstr = iconVar.toString();
            icon = DIconTheme::findQIcon(iconstr);
            if (icon.isNull())
                icon = QIcon(iconstr);
        }
        if (!icon.isNull()) {
            QIcon::Mode mode = QIcon::Normal;
            if (!(option.state & QStyle::State_Enabled))
                mode = QIcon::Disabled;
            else if (option.state & QStyle::State_Selected)
                mode = QIcon::Selected;

            QIcon::State state = (option.state & QStyle::State_Open) ? QIcon::On : QIcon::Off;
            icon.paint(painter, rect, option.decorationAlignment, mode, state);
            return;
        }
    }
}

void ListItemDelegate::drawDisplay(const QStyle *style, QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const
{
    DStyle::viewItemDrawText(style, painter, &option, rect);
}

void ListItemDelegate::drawEllipse(QPainter *painter, const QStyleOptionViewItem &option, const int message) const
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

void ListItemDelegate::drawFocus(const QStyle *style, QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const
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
    o.backgroundColor = option.palette.color(cg, (option.state & QStyle::State_Selected) ? QPalette::Highlight : QPalette::Window);
    style->drawPrimitive(QStyle::PE_FrameFocusRect, &o, painter, option.widget);
}

bool ListItemDelegate::helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    QStyleOptionViewItem opt(option);
    bool isBeginning = (opt.viewItemPosition == QStyleOptionViewItem::ViewItemPosition::Beginning) || (opt.viewItemPosition == QStyleOptionViewItem::ViewItemPosition::OnlyOne);
    int displayWidth = isBeginning ? opt.rect.width() : opt.rect.width() - opt.decorationSize.width() -30 ;

    QString displayName = index.data(Qt::DisplayRole).toString();

    int fontWidth = opt.widget->fontMetrics().horizontalAdvance(displayName);
    if (fontWidth > displayWidth) {
        if (event->type() == QEvent::ToolTip) {
            QToolTip::showText(QCursor::pos(), displayName, reinterpret_cast<QWidget*>(view), option.rect, 1000);
        }
    }
    return true;
}
