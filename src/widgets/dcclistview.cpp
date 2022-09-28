/*
 * Copyright (C) 2021 ~ 2023 Deepin Technology Co., Ltd.
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
#include "widgets/dcclistview.h"
#include "widgets/accessibleinterface.h"

#include <DStyledItemDelegate>

DCC_USE_NAMESPACE
SET_FORM_ACCESSIBLE(DCCListView, "DCCListView");
DCCListView::DCCListView(QWidget *parent)
    : DTK_WIDGET_NAMESPACE::DListView(parent)
{
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setBackgroundType(DTK_WIDGET_NAMESPACE::DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    setSelectionMode(QAbstractItemView::NoSelection);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSpacing(1);
}

void DCCListView::updateGeometries()
{
    DListView::updateGeometries();
    QRect r = rectForIndex(model()->index(model()->rowCount() - 1, 0));
    QMargins margins = viewportMargins();
    setFixedHeight(r.y() + r.height() + margins.top() + margins.bottom() + 1);
}
