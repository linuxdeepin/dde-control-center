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

QWidget *VListLayout::layoutModule(dccV23::ModuleObject *const module, QWidget *const parent, const QList<ModuleObject *> &children)
{
    Q_UNUSED(children)
    m_module = module;
    QHBoxLayout *hlayout = new QHBoxLayout(parent);
    parent->setLayout(hlayout);

    m_model = new ModuleDataModel(parent);
    m_model->setData(module);
    m_view = new DListView(parent);
    QWidget *widget = new QWidget(parent);
    QVBoxLayout *vlayout = new QVBoxLayout;
    m_hlayout = new QHBoxLayout;
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

    for (auto tmpChild : module->childrens()) {
        auto page = tmpChild->page();
        if (page) {
            if (tmpChild->extra()) {
                m_hlayout->addWidget(page);
                m_extraModules.append(tmpChild);
            }
        }
        tmpChild->active();
    }

    auto onClicked = [](const QModelIndex &index) {
        ModuleObject *obj = static_cast<ModuleObject *>(index.internalPointer());
        if (obj && !LayoutBase::IsDisabled(obj))
            obj->trigger();
    };

    QObject::connect(m_view, &DListView::activated, m_view, &DListView::clicked);
    QObject::connect(m_view, &DListView::clicked, m_view, onClicked);
    auto addModuleSlot = [this](ModuleObject *const tmpChild) {
        addChild(tmpChild);
    };
    // 监听子项的添加、删除、状态变更，动态的更新界面
    QObject::connect(module, &ModuleObject::insertedChild, m_view, addModuleSlot);
    QObject::connect(module, &ModuleObject::appendedChild, m_view, addModuleSlot);
    QObject::connect(module, &ModuleObject::removedChild, m_view, [this](ModuleObject *const childModule) { removeChild(childModule); });
    QObject::connect(module, &ModuleObject::childStateChanged, m_view, [this](ModuleObject *const tmpChild, uint32_t flag, bool state) {
        if (LayoutBase::IsHidenFlag(flag)) {
            if (state)
                removeChild(tmpChild);
            else
                addChild(tmpChild);
        } else if (LayoutBase::IsDisabledFlag(flag)) {
            int index = m_extraModules.indexOf(tmpChild);
            if (-1 != index) {
                m_hlayout->itemAt(index)->widget()->setDisabled(state);
            }
        }
    });
    QObject::connect(m_view, &QWidget::destroyed, module, [module] {
        for (auto tmpChild : module->childrens()) {
            tmpChild->deactive();
        }
        module->deactive();
    });

    return childWidget;
}

void VListLayout::removeChild(ModuleObject *const childModule)
{
    int index = m_extraModules.indexOf(childModule);
    if (index != -1) {
        QLayoutItem *item = m_hlayout->takeAt(index);
        item->widget()->deleteLater();
        delete item;
        m_extraModules.removeAt(index);
    }
}

void VListLayout::addChild(ModuleObject *const childModule)
{
    if (LayoutBase::IsHiden(childModule) || !childModule->extra() || m_extraModules.contains(childModule))
        return;

    int index = 0;
    for (auto &&child : m_module->childrens()) {
        if (child == childModule)
            break;
        if (!LayoutBase::IsHiden(child) && child->extra())
            index++;
    }
    auto newPage = childModule->page();
    if (newPage) {
        m_hlayout->insertWidget(index, newPage);
        newPage->setDisabled(LayoutBase::IsDisabled(childModule));
        m_extraModules.insert(index, childModule);
    }
}
