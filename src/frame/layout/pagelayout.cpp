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
    , m_area(nullptr)
{
}

void PageLayout::setCurrent(ModuleObject *const child)
{
    for (auto &&it : m_mapWidget) {
        if (it.first == child) {
            int index = m_vlayout->indexOf(it.second);
            if (index == -1)
                return;
            QWidget *w = it.second;
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
    //todo：此处会造成listview显示不全，待解决
    Q_UNUSED(title)
    return widget;
}

int PageLayout::getScrollPos(ModuleObject *child)
{
    int pos = 0;
    for (auto &it : m_mapWidget) {
        if (it.first == child)
            break;
        pos += it.second->height();
    }
    return pos;
}

QWidget *PageLayout::layoutModule(dccV23::ModuleObject *const module, QWidget *const parent, const QList<ModuleObject *> &children)
{
    Q_UNUSED(children)
    m_area = new QScrollArea(parent);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    parent->setLayout(mainLayout);
    mainLayout->addWidget(m_area);
    QHBoxLayout *m_hlayout = new QHBoxLayout();
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
                m_mapWidget.append({ tmpChild, page });
            }
            page->setDisabled(LayoutBase::IsDisabled(tmpChild));
        }
        tmpChild->active();
    }
    if (module->childrens().count() > 1)
        m_vlayout->addStretch(1);

    auto addChild = [this, module](ModuleObject *const childModule) {
        int index = module->childrens().indexOf(childModule);
        auto newPage = getPage(childModule->page(), childModule->displayName());
        if (newPage) {
            m_vlayout->insertWidget(index, newPage);
        }
    };
    QObject::connect(module, &ModuleObject::insertedChild, m_area, addChild);
    QObject::connect(module, &ModuleObject::appendedChild, m_area, addChild);

    QObject::connect(areaWidget, &QWidget::destroyed, module, [module] {
        for (auto tmpChild : module->childrens()) {
            tmpChild->deactive();
        }
        module->deactive();
    });
    return nullptr;
}
