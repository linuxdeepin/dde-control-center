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
#include "mainwindow.h"
#include "constant.h"

#include "navigation/navmodel.h"
#include "navigation/navdelegate.h"

#include <dlistview.h>

#include <QDebug>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent)
{
    initUI();
}

void MainWindow::initUI()
{
    DListView *view = new DListView(this);

    auto delegate = new NavDelegate(QListView::IconMode, view);
    delegate->setItemSize(QSize(Constant::HomeIconSize, Constant::HomeIconSize));

    view->setItemDelegate(delegate);
    view->setModel(new NavModel(1, view));
    view->setWordWrap(true);
    view->setOrientation(QListView::LeftToRight, true);
    view->setResizeMode(QListView::Adjust);
    view->setSpacing(Constant::NormalSpacing);

    setCentralWidget(view);
}
