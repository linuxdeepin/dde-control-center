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
#include "pagelayout.h"
#include "moduledatamodel.h"
#include "tabitemdelegate.h"
#include "tabview.h"

#include <DFrame>
#include <DListView>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QScrollBar>

DWIDGET_USE_NAMESPACE
DCC_USE_NAMESPACE

PageLayout::PageLayout()
    : m_vlayout(nullptr)
    , m_hlayout(nullptr)
    , m_area(nullptr)
    , m_module(nullptr)
{
}

void PageLayout::setCurrent(ModuleObject *const child)
{
    if (m_mapWidget.contains(child)) {
        QWidget *w = m_mapWidget.value(child);
        if (-1 != m_vlayout->indexOf(w)) {
            QPoint p = w->mapTo(w->parentWidget(), QPoint());
            m_area->verticalScrollBar()->setSliderPosition(p.y());
        }
    }
}

QWidget *PageLayout::getPage(QWidget *const widget, const QString &title)
{
    //    if (!widget)
    //        return nullptr;
    //    QLabel *titleLbl = new QLabel(title, this);
    //    QWidget *page = new QWidget(this);
    //    QVBoxLayout *vLayout = new QVBoxLayout(page);
    //    page->setLayout(vLayout);
    //    vLayout->addWidget(titleLbl, 0, Qt::AlignTop);
    //    vLayout->addWidget(widget, 1, Qt::AlignTop);
    //    if (title.isEmpty()) {
    //        titleLbl->setVisible(false);
    //    }
    // todo：此处会造成listview显示不全，待解决
    Q_UNUSED(title)
    return widget;
}

void PageLayout::removeChild(ModuleObject *const childModule)
{
    if (m_mapWidget.contains(childModule)) {
        QWidget *w = m_mapWidget.value(childModule);
        int index = m_vlayout->indexOf(w);
        if (-1 != index) {
            w->deleteLater();
            delete m_vlayout->takeAt(index);
            m_mapWidget.remove(childModule);
            return;
        }
        index = m_hlayout->indexOf(w);
        if (-1 != index) {
            w->deleteLater();
            delete m_hlayout->takeAt(index);
            m_mapWidget.remove(childModule);
        }
    }
}

void PageLayout::addChild(ModuleObject *const childModule)
{
    if (LayoutBase::IsHiden(childModule))
        return;

    bool isExtra = childModule->extra();
    int index = 0;
    for (auto &&child : m_module->childrens()) {
        if (child == childModule)
            break;
        if (!LayoutBase::IsHiden(child) && child->extra() == isExtra)
            index++;
    }
    auto newPage = childModule->page();
    if (newPage) {
        if (isExtra)
            m_hlayout->insertWidget(index, newPage);
        else
            m_vlayout->insertWidget(index, newPage);

        newPage->setDisabled(LayoutBase::IsDisabled(childModule));
        m_mapWidget.insert(childModule, newPage);
    }
}

QWidget *PageLayout::layoutModule(ModuleObject *const module, QWidget *const parent, const QList<ModuleObject *> &children)
{
    Q_UNUSED(children)
    m_module = module;
    m_area = new QScrollArea(parent);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    parent->setLayout(mainLayout);
    mainLayout->addWidget(m_area);
    m_hlayout = new QHBoxLayout();
    mainLayout->addLayout(m_hlayout);
    m_area->setFrameShape(QFrame::NoFrame);
    m_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_area->setWidgetResizable(true);

    QWidget *areaWidget = new QWidget(m_area);
    m_area->setWidget(areaWidget);
    m_vlayout = new QVBoxLayout(areaWidget);
    areaWidget->setLayout(m_vlayout);

    for (auto tmpChild : module->childrens()) {
        if (LayoutBase::IsHiden(tmpChild))
            continue;
        auto page = getPage(tmpChild->page(), tmpChild->displayName());
        if (page) {
            if (tmpChild->extra())
                m_hlayout->addWidget(page);
            else {
                m_vlayout->addWidget(page, 0, Qt::AlignTop);
            }
            m_mapWidget.insert(tmpChild, page);
            page->setDisabled(LayoutBase::IsDisabled(tmpChild));
        }
        tmpChild->active();
    }
    if (module->childrens().count() > 1)
        m_vlayout->addStretch(1);

    auto addModuleSlot = [this](ModuleObject *const tmpChild) {
        addChild(tmpChild);
    };
    // 监听子项的添加、删除、状态变更，动态的更新界面
    QObject::connect(module, &ModuleObject::insertedChild, areaWidget, addModuleSlot);
    QObject::connect(module, &ModuleObject::appendedChild, areaWidget, addModuleSlot);
    QObject::connect(module, &ModuleObject::removedChild, areaWidget, [this](ModuleObject *const childModule) { removeChild(childModule); });
    QObject::connect(module, &ModuleObject::childStateChanged, areaWidget, [this](ModuleObject *const tmpChild, uint32_t flag, bool state) {
        if (LayoutBase::IsHidenFlag(flag)) {
            if (state)
                removeChild(tmpChild);
            else
                addChild(tmpChild);
        } else if (LayoutBase::IsDisabledFlag(flag)) {
            if (m_mapWidget.contains(tmpChild)) {
                m_mapWidget.value(tmpChild)->setDisabled(state);
            }
        }
    });
    QObject::connect(areaWidget, &QWidget::destroyed, module, [module] {
        for (auto tmpChild : module->childrens()) {
            tmpChild->deactive();
        }
        module->deactive();
    });
    return nullptr;
}
