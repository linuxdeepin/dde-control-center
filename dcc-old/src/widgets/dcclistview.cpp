//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "widgets/dcclistview.h"
#include "widgets/accessibleinterface.h"

#include <DStyledItemDelegate>

using namespace DCC_NAMESPACE;
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
