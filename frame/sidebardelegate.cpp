/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "sidebardelegate.h"
#include "sidebarmodel.h"

#include <QPainter>
#include <QDebug>

SidebarDelegate::SidebarDelegate(QObject *parent)
    : QItemDelegate(parent)
{

}

void SidebarDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QPixmap pixmap;
    if (option.state & QStyle::State_MouseOver)
        pixmap = index.data(SidebarModel::HoverIcon).value<QPixmap>();
    else
        pixmap = index.data(SidebarModel::DefaultIcon).value<QPixmap>();

    painter->drawPixmap(option.rect.center() - pixmap.rect().center(), pixmap);
}

