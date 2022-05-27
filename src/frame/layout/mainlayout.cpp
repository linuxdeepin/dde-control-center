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
#include "listitemdelegate.h"
#include "listview.h"
#include "mainlayout.h"
#include "moduledatamodel.h"

#include <QBoxLayout>
DCC_USE_NAMESPACE

const int NavViewMaximumWidth = QWIDGETSIZE_MAX;
const int NavViewMinimumWidth = 188;

const QSize ListViweItemIconSize_IconMode(84, 84);
const QSize ListViweItemSize_IconMode(280, 84);
const QSize ListViweItemIconSize_ListMode(32, 32);
const QSize ListViweItemSize_ListMode(168, 48);

MainLayout::MainLayout()
    : m_view(nullptr)
    , m_model(nullptr)
{
}

ModuleObject *MainLayout::autoExpand(ModuleObject *const module, const QList<ModuleObject *> &children)
{
    Q_UNUSED(module)
    return children.isEmpty() ? nullptr : children.first();
}

void MainLayout::setCurrent(ModuleObject *const child)
{
    if (m_view && m_model) {
        m_view->setCurrentIndex(m_model->index(child));
    }
}

QWidget *MainLayout::layoutModule(dccV23::ModuleObject *const module, QWidget *const parent, const QList<ModuleObject *> &children)
{
    QHBoxLayout *vlayout = new QHBoxLayout(parent);
    //    configLayout(vlayout);
    parent->setLayout(vlayout);

    m_model = new ModuleDataModel(parent);
    m_model->setData(module);

    m_view = new ListView(parent);
    ListItemDelegate *delegate = new ListItemDelegate(m_view);
    m_view->setItemDelegate(delegate);
    m_view->setModel(m_model);
    m_view->setFrameShape(QFrame::Shape::NoFrame);
    m_view->setAutoScroll(true);
    m_view->setDragEnabled(false);
    m_view->setMaximumWidth(NavViewMaximumWidth);
    m_view->setMinimumWidth(NavViewMinimumWidth);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setIconSize(ListViweItemIconSize_IconMode);

    //    m_view->setCurrentIndex(m_model->index(children));
    auto onClicked = [](const QModelIndex &index) {
        ModuleObject *obj = static_cast<ModuleObject *>(index.internalPointer());
        if (obj)
            obj->trigger();
    };

    QObject::connect(m_view, &ListView::activated, m_view, &ListView::clicked);
    QObject::connect(m_view, &ListView::clicked, m_view, onClicked);
    QObject::connect(m_view, &ListView::destroyed, module, &ModuleObject::deactive);

    if (children.isEmpty()) {
        m_view->setGridSize(ListViweItemSize_IconMode);
        m_view->setSpacing(20);
        m_view->setViewMode(ListView::IconMode);
        m_view->setAcceptDrops(false);
        m_view->setAlignment(Qt::AlignCenter);
        vlayout->addWidget(m_view);
        return nullptr;
    }
    m_view->setIconSize(ListViweItemIconSize_ListMode);
    m_view->setGridSize(ListViweItemSize_ListMode);
    m_view->setSpacing(0);

    QWidget *childWdiget = new QWidget(parent);
    vlayout->addWidget(m_view, 1);
    vlayout->addWidget(childWdiget, 5);
    return childWdiget;
}
