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
#include "vlistlayout.h"

#include "moduledatamodel.h"
#include "tabitemdelegate.h"
#include "tabview.h"

#include <DFrame>
#include <DListView>
#include <QHBoxLayout>

DWIDGET_USE_NAMESPACE
DCC_USE_NAMESPACE

VListLayout::VListLayout()
    : m_view(nullptr)
    , m_model(nullptr)
{
}

void VListLayout::setCurrent(ModuleObject *const child)
{
    if (m_view && m_model) {
        m_view->setCurrentIndex(m_model->index(child));
    }
}

QWidget *VListLayout::layoutModule(dccV23::ModuleObject *const module, QWidget *const parent, ModuleObject * const child)
{
    QHBoxLayout *hlayout = new QHBoxLayout(parent);
    parent->setLayout(hlayout);

    m_model = new ModuleDataModel(parent);
    m_model->setData(module);
    m_view = new DListView(parent);
    QWidget *widget = new QWidget(parent);
    QVBoxLayout *vlayout = new QVBoxLayout;
    QHBoxLayout *m_hlayout = new QHBoxLayout;
    widget->setLayout(vlayout);
    vlayout->addWidget(m_view);
    vlayout->addLayout(m_hlayout);
    hlayout->addWidget(widget, 1);
    hlayout->addWidget(new DVerticalLine);

    QWidget *childWidget = new QWidget(parent);
    hlayout->addWidget(childWidget, 5);

    m_view->setModel(m_model);
    m_view->setFrameShape(QFrame::NoFrame);
    m_view->setAutoScroll(true);
    m_view->setDragEnabled(false);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setSpacing(0);
    m_view->setItemSpacing(2);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setCurrentIndex(m_model->index(child));

    for (auto tmpChild : module->childrens()) {
        auto page = tmpChild->page();
        if (page) {
            if (tmpChild->extra())
                m_hlayout->addWidget(page);
        }
        tmpChild->active();
    }

    auto onClicked = [](const QModelIndex &index) {
        ModuleObject *obj = static_cast<ModuleObject *>(index.internalPointer());
        if (obj)
            obj->trigger();
    };

    QObject::connect(m_view, &DListView::activated, m_view, &DListView::clicked);
    QObject::connect(m_view, &DListView::clicked, m_view, onClicked);
    QObject::connect(m_view, &DListView::destroyed, module, &ModuleObject::deactive);
    return childWidget;
}
