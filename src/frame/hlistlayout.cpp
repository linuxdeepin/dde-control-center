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
#include "hlistlayout.h"
#include "moduledatamodel.h"
#include "tabitemdelegate.h"
#include "tabview.h"

#include <DFrame>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE
DCC_USE_NAMESPACE

HListLayout::HListLayout()
    : m_view(nullptr)
    , m_model(nullptr)
{
}

int HListLayout::layoutType() const
{
    return 2;
}

void HListLayout::setCurrent(ModuleObject *const child)
{
    if (m_view && m_model) {
        m_view->setCurrentIndex(m_model->index(child));
    }
}

QWidget *HListLayout::layoutModule(dccV23::ModuleObject *const module, QWidget *const parent, const int index)
{
    QVBoxLayout *vlayout = new QVBoxLayout(parent);
    parent->setLayout(vlayout);

    DFrame *dframeTab = new DFrame(parent);
    QHBoxLayout *hlayout = new QHBoxLayout(dframeTab);
    hlayout->setMargin(3);
    hlayout->setSpacing(0);
    m_view = new TabView(parent);
    hlayout->addWidget(m_view);
    m_model = new ModuleDataModel(m_view);
    TabItemDelegate *delegate = new TabItemDelegate(m_view);
    m_model->setData(module);
    m_view->setModel(m_model);
    m_view->setItemDelegate(delegate);
    vlayout->addWidget(dframeTab, 1, Qt::AlignCenter);
    m_view->setCurrentIndex(m_model->index(index == -1 ? 0 : index, 0));
    QWidget *childWdiget = new QWidget(parent);

    auto onClicked = [=](const QModelIndex &index) {
        ModuleObject *obj = static_cast<ModuleObject *>(index.internalPointer());
        if (obj)
            obj->trigger();
    };

    QObject::connect(m_view, &TabView::activated, m_view, &TabView::clicked);
    QObject::connect(m_view, &TabView::clicked, m_view, onClicked);
    QObject::connect(m_view, &TabView::destroyed, module, &ModuleObject::deactive);
    vlayout->addWidget(childWdiget, 6);
    return childWdiget;
}
