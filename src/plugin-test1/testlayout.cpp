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
#include "testlayout.h"

#include <QFormLayout>
#include <QScrollArea>
#include <QScrollBar>

DCC_USE_NAMESPACE

testLayout::testLayout()
    : m_layout(nullptr)
    , m_module(nullptr)
    , m_area(nullptr)
{
}
// 设置并定位到当前项，如果是ListView则setCurrentIndex，如果是滚动区域，则需要滚动到对应项
void testLayout::setCurrent(ModuleObject *const child)
{
    int index = m_modules.indexOf(child);
    if (index == -1)
        return;
    QWidget *w = m_layout->itemAt(index, QFormLayout::FieldRole)->widget();
    QPoint p = w->mapTo(w->parentWidget(), QPoint());
    m_area->verticalScrollBar()->setSliderPosition(p.y());
}

QWidget *testLayout::layoutModule(ModuleObject *const module, QWidget *const parent, const QList<ModuleObject *> &children)
{
    Q_UNUSED(children)
    // 所保存的变量尽量与parent关联，该类会在界面切换时delete
    m_module = module;
    m_area = new QScrollArea(parent);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    parent->setLayout(mainLayout);
    mainLayout->addWidget(m_area);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    QHBoxLayout *m_hlayout = new QHBoxLayout();
    m_hlayout->setMargin(0);
    m_hlayout->setSpacing(0);
    mainLayout->addLayout(m_hlayout);
    m_area->setFrameShape(QFrame::NoFrame);
    m_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_area->setWidgetResizable(true);

    QWidget *areaWidget = new QWidget(m_area);
    m_area->setWidget(areaWidget);
    m_layout = new QFormLayout;
    m_layout->setMargin(0);
    m_layout->setSpacing(0);
    areaWidget->setLayout(m_layout);

    for (auto tmpChild : module->childrens()) {
        // 隐藏、禁用判断使用IsHiden、IsDisabled，该函数包含设置项和配置项结果
        if (LayoutBase::IsHiden(tmpChild))
            continue;
        auto page = tmpChild->page();
        if (page) {
            // 有用到ModuleObject的基本信息时需要监听moduleDataChanged信号去动态修改
            m_layout->addRow(tmpChild->displayName(), page);
            m_modules.append(tmpChild);
            page->setDisabled(LayoutBase::IsDisabled(tmpChild));
        }
        tmpChild->active();
    }
    auto addModuleSlot = [this](ModuleObject *const tmpChild) {
        addChild(tmpChild);
    };

    // 监听子项的添加、删除、状态变更，动态的更新界面
    QObject::connect(module, &ModuleObject::insertedChild, m_area, addModuleSlot);
    QObject::connect(module, &ModuleObject::appendedChild, m_area, addModuleSlot);
    QObject::connect(module, &ModuleObject::removedChild, m_area, [this](ModuleObject *const childModule) { removeChild(childModule); });
    QObject::connect(module, &ModuleObject::childStateChanged, m_area, [this](ModuleObject *const tmpChild, uint32_t flag, bool state) {
        if (LayoutBase::IsHidenFlag(flag)) {
            if (state)
                removeChild(tmpChild);
            else
                addChild(tmpChild);
        } else if (LayoutBase::IsDisabledFlag(flag)) {
            int index = m_modules.indexOf(tmpChild);
            m_layout->itemAt(index, QFormLayout::FieldRole)->widget()->setDisabled(state);
        }
    });
    QObject::connect(areaWidget, &QWidget::destroyed, module, [module] {
        for (auto tmpChild : module->childrens()) {
            tmpChild->deactive();
        }
        module->deactive();
    });
    // 如果子项需要继续布局则需返回一个QWidget,否则返回空
    return nullptr;
}
// 动态删除子项
void testLayout::removeChild(ModuleObject *const childModule)
{
    int index = m_modules.indexOf(childModule);
    if (index != -1) {
        m_layout->removeRow(index);
        m_modules.removeAt(index);
    }
}
// 动态添加子项
void testLayout::addChild(ModuleObject *const childModule)
{
    if (LayoutBase::IsHiden(childModule))
        return;

    int index = 0;
    for (auto &&child : m_module->childrens()) {
        if (child == childModule)
            break;
        if (!LayoutBase::IsHiden(child))
            index++;
    }
    auto newPage = childModule->page();
    if (newPage) {
        m_layout->insertRow(index, childModule->displayName(), newPage);
        newPage->setDisabled(LayoutBase::IsDisabled(childModule));
        m_modules.insert(index, childModule);
    }
}
