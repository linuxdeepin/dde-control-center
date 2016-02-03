/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef SIDEBARDELEGATE_H
#define SIDEBARDELEGATE_H

#include <QItemDelegate>

class SidebarDelegate : public QItemDelegate
{
public:
    SidebarDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // SIDEBARDELEGATE_H
