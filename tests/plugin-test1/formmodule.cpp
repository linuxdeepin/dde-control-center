//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "formmodule.h"

#include <QFormLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

FormModule::FormModule(const QString &name, const QString &displayName, QObject *parent)
    : ModuleObject(name, displayName, parent)
    , m_area(nullptr)
    , m_layout(nullptr)
{
    // 响应子类激活信号
    connect(this, &FormModule::currentModuleChanged, this, &FormModule::onCurrentModuleChanged);
}

QWidget *FormModule::page()
{
    // page函数在parentWidget上布局窗口并返回
    QWidget *parentWidget = new QWidget();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    parentWidget->setLayout(mainLayout);
    m_layout = new QFormLayout();
    // 在parentWidget析构后需要清理缓存数据，可以监听信号处理，或放deactive函数中
    connect(parentWidget, &QObject::destroyed, this, [this]() { clearData(); });

    QWidget *areaWidget = new QWidget();

    m_area = new QScrollArea(parentWidget);
    m_area->setFrameShape(QFrame::NoFrame);
    m_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_area->setWidgetResizable(true);
    areaWidget->setParent(m_area);
    m_area->setWidget(areaWidget);
    mainLayout->addWidget(m_area);

    areaWidget->setLayout(m_layout);

    for (auto &&tmpChild : childrens()) {
        auto page = tmpChild->activePage();
        if (page) {
            m_layout->addRow(tmpChild->displayName(), page);
            m_mapWidget.insert(tmpChild, page);
        }
    }

    // 监听子项的添加、删除、状态变更，动态的更新界面
    connect(this, &ModuleObject::insertedChild, areaWidget, [this](ModuleObject *const childModule) { onAddChild(childModule); });
    connect(this, &ModuleObject::removedChild, areaWidget, [this](ModuleObject *const childModule) { onRemoveChild(childModule); });
    connect(this, &ModuleObject::childStateChanged, areaWidget, [this](ModuleObject *const tmpChild, uint32_t flag, bool state) {
        if (ModuleObject::IsHiddenFlag(flag)) { // 显示隐藏同增加删除处理
            if (state)
                onRemoveChild(tmpChild);
            else
                onAddChild(tmpChild);
        }
    });
    // 处理子激活项
    onCurrentModuleChanged(currentModule());
    return parentWidget;
}
// 处理子激活项
void FormModule::onCurrentModuleChanged(dccV23::ModuleObject *child)
{
    // 激活子项处理是删除之前激活项的QWidget,并用当前激活项的activePage替代
    // 该例子中子项都为叶子节点，此处为滚动到对应项
    // 获取窗口坐标需要在窗口显示后，所以此处稍作延时
    QTimer::singleShot(10, m_area, [this, child]() {
        if (m_area && m_mapWidget.contains(child)) {
            QWidget *w = m_mapWidget.value(child);
            if (-1 != m_layout->indexOf(w)) {
                QPoint p = w->mapTo(w->parentWidget(), QPoint());
                m_area->verticalScrollBar()->setSliderPosition(p.y());
            }
        }
    });
}
// 动态的添加子项
void FormModule::onAddChild(dccV23::ModuleObject *const childModule)
{
    if (ModuleObject::IsHidden(childModule) || m_mapWidget.contains(childModule))
        return;

    int index = 0;
    for (auto &&child : childrens()) {
        if (child == childModule)
            break;
        if (!ModuleObject::IsHidden(child))
            index++;
    }
    auto newPage = childModule->activePage();
    if (newPage) {
        m_layout->insertRow(index, childModule->displayName(), newPage);
        m_mapWidget.insert(childModule, newPage);
    }
}
// 动态的删除子项
void FormModule::onRemoveChild(dccV23::ModuleObject *const childModule)
{
    if (m_mapWidget.contains(childModule)) {
        QWidget *w = m_mapWidget.value(childModule);
        int index = m_layout->indexOf(w);
        if (-1 != index) {
            w->deleteLater();
            delete m_layout->takeAt(index);
            m_mapWidget.remove(childModule);
            return;
        }
    }
}
// 清理缓存数据
void FormModule::clearData()
{
    m_layout = nullptr;
    m_area = nullptr;
    m_mapWidget.clear();
}
