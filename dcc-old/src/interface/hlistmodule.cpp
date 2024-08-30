//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "hlistmodule.h"
#include "moduledatamodel.h"
#include "tabitemdelegate.h"
#include "tabview.h"
#include "pagemodule.h"

#include <DFrame>
#include <QVBoxLayout>

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE

namespace DCC_NAMESPACE {
class HListModulePrivate
{
public:
    explicit HListModulePrivate(HListModule *parent)
        : q_ptr(parent)
        , m_view(nullptr)
        , m_layout(nullptr)
    {
    }

    void onCurrentModuleChanged(ModuleObject *child)
    {
        if (!m_layout)
            return;
        if (m_layout->count() > 1) {
            QLayoutItem *item = m_layout->takeAt(1);
            delete item->widget();
            delete item;
        }
        if (child) {
            PageModule *module = qobject_cast<PageModule *>(child);
            if(module) {
                module->setContentsMargins(60, 0, 60, 0);
                module->setMaximumWidth(DCC_PAGEMODULE_MAX_WIDTH + 60 * 2);
            }
            m_layout->addWidget(child->activePage(), 60);
            ModuleDataModel *model = static_cast<ModuleDataModel *>(m_view->model());
            m_view->setCurrentIndex(model->index(child));
        }
    }

    QWidget *page()
    {
        Q_Q(HListModule);
        QWidget *parentWidget = new QWidget();
        m_layout = new QVBoxLayout(parentWidget);
        m_layout->setContentsMargins(0, 10, 0, 10);
        parentWidget->setLayout(m_layout);
        QObject::connect(parentWidget, &QObject::destroyed, m_layout, [this]() { m_layout = nullptr; });

        m_view = new TabView;

        TabItemDelegate *delegate = new TabItemDelegate(m_view);
        ModuleDataModel *model = new ModuleDataModel(m_view);
        model->setModuleObject(q);
        QObject::connect(q, &HListModule::currentModuleChanged, m_layout, [this](ModuleObject *child) {
            onCurrentModuleChanged(child);
        });
        m_view->setModel(model);
        m_view->setItemDelegate(delegate);

        m_layout->addWidget(m_view);

        auto onClicked = [](const QModelIndex &index) {
            ModuleObject *obj = static_cast<ModuleObject *>(index.internalPointer());
            if (obj)
                obj->trigger();
        };

        QObject::connect(m_view, &TabView::activated, m_view, &TabView::clicked);
        QObject::connect(m_view, &TabView::clicked, m_view, onClicked);

        onCurrentModuleChanged(q->currentModule());
        return parentWidget;
    }

private:
    HListModule *q_ptr;
    Q_DECLARE_PUBLIC(HListModule)

    TabView *m_view;
    QVBoxLayout *m_layout;
};
}

HListModule::HListModule(QObject *parent)
    : ModuleObject(parent)
    , DCC_INIT_PRIVATE(HListModule)
{
}

HListModule::HListModule(const QString &name, const QString &displayName, QObject *parent)
    : ModuleObject(name, displayName, parent)
    , DCC_INIT_PRIVATE(HListModule)
{
}

HListModule::HListModule(const QString &name, const QStringList &contentText, QObject *parent)
    : ModuleObject(name, contentText, parent)
    , DCC_INIT_PRIVATE(HListModule)
{
}

HListModule::HListModule(const QString &name, const QString &displayName, const QStringList &contentText, QObject *parent)
    : ModuleObject(name, displayName, contentText, parent)
    , DCC_INIT_PRIVATE(HListModule)
{
}

HListModule::HListModule(const QString &name, const QString &displayName, const QVariant &icon, QObject *parent)
    : ModuleObject(name, displayName, icon, parent)
    , DCC_INIT_PRIVATE(HListModule)
{
}

HListModule::HListModule(const QString &name, const QString &displayName, const QString &description, QObject *parent)
    : ModuleObject(name, displayName, description, parent)
    , DCC_INIT_PRIVATE(HListModule)
{
}

HListModule::HListModule(const QString &name, const QString &displayName, const QString &description, const QVariant &icon, QObject *parent)
    : ModuleObject(name, displayName, description, icon, parent)
    , DCC_INIT_PRIVATE(HListModule)
{
}

HListModule::HListModule(const QString &name, const QString &displayName, const QString &description, const QIcon &icon, QObject *parent)
    : ModuleObject(name, displayName, description, icon, parent)
    , DCC_INIT_PRIVATE(HListModule)
{
}

HListModule::HListModule(const QString &name, const QString &displayName, const QString &description, const QStringList &contentText, const QVariant &icon, QObject *parent)
    : ModuleObject(name, displayName, description, contentText, icon, parent)
    , DCC_INIT_PRIVATE(HListModule)
{
}

HListModule::HListModule(const ModuleInitContext &message, QObject *parent)
   : ModuleObject(message, parent)
   , DCC_INIT_PRIVATE(HListModule)
{
}

HListModule::~HListModule()
{
}

QWidget *HListModule::page()
{
    Q_D(HListModule);
    return d->page();
}
