//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "widgets/horizontalmodule.h"

#include <QMap>
#include <QWidget>
#include <QHBoxLayout>

using namespace DCC_NAMESPACE;

namespace DCC_NAMESPACE {
class HorizontalModulePrivate
{
public:
    explicit HorizontalModulePrivate(HorizontalModule *object)
        : q_ptr(object)
        , m_stretchType(HorizontalModule::NoStretch)
        , m_spacing(-1)
    {
    }
    void insertModule(ModuleObject *const module, int stretch, Qt::Alignment alignment)
    {
        m_mapModules.insert(module, { stretch, alignment });
    }
    void removeModule(ModuleObject *const module)
    {
        m_mapModules.remove(module);
    }
    QPair<int, Qt::Alignment> layoutParam(ModuleObject *const module)
    {
        if (m_mapModules.contains(module))
            return m_mapModules.value(module);
        return { 0, Qt::Alignment() };
    }

    QWidget *page()
    {
        Q_Q(HorizontalModule);
        QWidget *w = new QWidget();
        m_mapWidget.clear();
        QObject::connect(w, &QObject::destroyed, q, [this]() { m_mapWidget.clear(); });

        m_layout = new QHBoxLayout(w);
        m_layout->setMargin(0);
        m_layout->setSpacing(m_spacing);
        if (m_stretchType & HorizontalModule::LeftStretch)
            m_layout->addStretch();
        for (auto &&tmpChild : q->childrens()) {
            QWidget *childPage = tmpChild->activePage();
            if (childPage) {
                QPair<int, Qt::Alignment> param = layoutParam(tmpChild);
                m_layout->addWidget(childPage, param.first, param.second);
                m_mapWidget.insert(tmpChild, childPage);
            }
        }
        if (m_stretchType & HorizontalModule::RightStretch)
            m_layout->addStretch();

        // 监听子项的添加、删除、状态变更，动态的更新界面
        QObject::connect(q, &ModuleObject::insertedChild, w, [this](ModuleObject *const childModule) { onAddChild(childModule); });
        QObject::connect(q, &ModuleObject::removedChild, w, [this](ModuleObject *const childModule) { onRemoveChild(childModule); });
        QObject::connect(q, &ModuleObject::childStateChanged, w, [this](ModuleObject *const tmpChild, uint32_t flag, bool state) {
            if (ModuleObject::IsHiddenFlag(flag)) {
                if (state)
                    onRemoveChild(tmpChild);
                else
                    onAddChild(tmpChild);
            }
        });
        return w;
    }

private:
    void onRemoveChild(DCC_NAMESPACE::ModuleObject *const childModule)
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
    void onAddChild(DCC_NAMESPACE::ModuleObject *const childModule)
    {
        if (ModuleObject::IsHidden(childModule) || m_mapWidget.contains(childModule))
            return;

        Q_Q(HorizontalModule);
        bool isExtra = childModule->extra();
        int index = (m_stretchType & HorizontalModule::LeftStretch) ? 1 : 0;
        for (auto &&child : q->childrens()) {
            if (child == childModule)
                break;
            if (!ModuleObject::IsHidden(child) && child->extra() == isExtra)
                index++;
        }
        auto newPage = childModule->activePage();
        if (newPage) {
            QPair<int, Qt::Alignment> param = layoutParam(childModule);
            m_layout->insertWidget(index, newPage, param.first, param.second);
            m_mapWidget.insert(childModule, newPage);
        }
    }

public:
    HorizontalModule *q_ptr;
    Q_DECLARE_PUBLIC(HorizontalModule)
    QMap<ModuleObject *, QPair<int, Qt::Alignment>> m_mapModules;
    QMap<ModuleObject *, QWidget *> m_mapWidget;
    QHBoxLayout *m_layout;
    HorizontalModule::StretchType m_stretchType;
    int m_spacing;
};
}

HorizontalModule::HorizontalModule(const QString &name, const QString &displayName, QObject *parent)
    : ModuleObject(name, displayName, parent)
    , DCC_INIT_PRIVATE(HorizontalModule)
{
}

HorizontalModule::~HorizontalModule()
{
}

void HorizontalModule::setStretchType(StretchType stretchType)
{
    Q_D(HorizontalModule);
    d->m_stretchType = stretchType;
}

void HorizontalModule::setSpacing(const int spacing)
{
    Q_D(HorizontalModule);
    d->m_spacing = spacing;
}

void HorizontalModule::appendChild(ModuleObject *const module)
{
    appendChild(module, 0, Qt::Alignment());
}

void HorizontalModule::insertChild(QList<ModuleObject *>::iterator before, ModuleObject *const module)
{
    insertChild(before, module, 0, Qt::Alignment());
}

void HorizontalModule::insertChild(const int index, ModuleObject *const module)
{
    insertChild(index, module, 0, Qt::Alignment());
}

void HorizontalModule::removeChild(ModuleObject *const module)
{
    Q_D(HorizontalModule);
    d->removeModule(module);
    ModuleObject::removeChild(module);
}

void HorizontalModule::removeChild(const int index)
{
    Q_D(HorizontalModule);
    d->removeModule(children(index));
    ModuleObject::removeChild(index);
}

void HorizontalModule::appendChild(ModuleObject *const module, int stretch, Qt::Alignment alignment)
{
    if (childrens().contains(module))
        return;

    Q_D(HorizontalModule);
    d->insertModule(module, stretch, alignment);
    ModuleObject::appendChild(module);
}

void HorizontalModule::insertChild(QList<ModuleObject *>::iterator before, ModuleObject *const module, int stretch, Qt::Alignment alignment)
{
    if (childrens().contains(module))
        return;

    Q_D(HorizontalModule);
    d->insertModule(module, stretch, alignment);
    ModuleObject::insertChild(before, module);
}

void HorizontalModule::insertChild(const int index, ModuleObject *const module, int stretch, Qt::Alignment alignment)
{
    if (childrens().contains(module))
        return;

    Q_D(HorizontalModule);
    d->insertModule(module, stretch, alignment);
    ModuleObject::insertChild(index, module);
}

QWidget *HorizontalModule::page()
{
    Q_D(HorizontalModule);
    return d->page();
}
