/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     zccrs <zccrs@live.com>
 *
 * Maintainer: zccrs <zhangjide@deepin.com>
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
#ifndef NAVIWINVIEW_H
#define NAVIWINVIEW_H

#include <QListView>
#include "navigation/navdelegate.h"

class NavDelegate;
class NavWinView : public QListView
{
public:
    explicit NavWinView(QWidget *parent = nullptr);

    void setViewMode(ViewMode mode);

private:
    using QListView::setItemDelegate;

    NavDelegate *m_delegate;
};

#endif // NAVIGATIONVIEW_H
