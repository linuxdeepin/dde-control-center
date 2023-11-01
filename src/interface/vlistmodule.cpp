//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "moduledatamodel.h"
#include "vlistmodule.h"
#include "pagemodule.h"
#include "hlistmodule.h"

#include <DListView>
#include <DVerticalLine>
#include <QHBoxLayout>
#include <QSplitter>

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE

namespace DCC_NAMESPACE {
class VListModulePrivate
{
public:
    explicit VListModulePrivate(VListModule *parent)
        : q_ptr(parent)
        , m_view(nullptr)
        , m_splitter(nullptr)
        , m_childMargin(20)
        , m_splitterSize({})
    {
    }

    void onCurrentModuleChanged(ModuleObject *child)
    {
        if (!m_splitter)
            return;
        if (child) {
            QWidget *widget = nullptr;
            ModuleObject *activeModule = nullptr;
            if (child->extra() && child->hasChildrens()) {
                activeModule = child->children(0);
            } else {
                activeModule = child;
            }

            PageModule *module = qobject_cast<PageModule *>(activeModule);
            if (module) {
                module->setContentsMargins(m_childMargin, 0, m_childMargin, 0);
                module->setMaximumWidth(DCC_PAGEMODULE_MAX_WIDTH + m_childMargin * 2);
            }
            widget = child->activePage();
            if (widget) {
                QWidget *oldWidget = m_splitter->replaceWidget(1, widget);
                widget->setVisible(true);
                if (oldWidget)
                    delete oldWidget;
                ModuleDataModel *model = static_cast<ModuleDataModel *>(m_view->model());
                m_view->setCurrentIndex(model->index(child));
            }
        }
    }

    void onRemoveChild(DCC_NAMESPACE::ModuleObject *const childModule)
    {
        int index = m_extraModules.indexOf(childModule);
        if (index != -1) {
            QLayoutItem *item = m_hlayout->takeAt(index);
            item->widget()->deleteLater();
            delete item;
            m_extraModules.removeAt(index);
        }
    }

    void onAddChild(DCC_NAMESPACE::ModuleObject *const childModule)
    {
        if (ModuleObject::IsHidden(childModule) || !childModule->extra() || m_extraModules.contains(childModule))
            return;

        Q_Q(VListModule);
        int index = 0;
        for (auto &&child : q->childrens()) {
            if (child == childModule)
                break;
            if (!ModuleObject::IsHidden(child) && child->extra())
                index++;
        }
        auto newPage = childModule->activePage();
        if (newPage) {
            m_hlayout->insertWidget(index, newPage);
            m_extraModules.insert(index, childModule);
        }
    }

    QWidget *page()
    {
        Q_Q(VListModule);
        m_splitter = new QSplitter(Qt::Horizontal);
        QObject::connect(m_splitter, &QObject::destroyed, q, [this]() { m_splitter = nullptr; });

        DListView *view = new DListView(m_splitter);
        QWidget *widget = new QWidget(m_splitter);
        QVBoxLayout *vlayout = new QVBoxLayout;
        m_hlayout = new QHBoxLayout;
        widget->setLayout(vlayout);
        vlayout->addWidget(view);
        vlayout->addLayout(m_hlayout);
        widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        widget->setMinimumWidth(158);
        widget->setMaximumWidth(300);
        m_splitter->addWidget(widget);
        m_emptyWidget = new QWidget(m_splitter);
        m_splitter->addWidget(m_emptyWidget);
        m_splitter->setChildrenCollapsible(false);

        ModuleDataModel *model = new ModuleDataModel(view);
        model->setModuleObject(q);
        QObject::connect(q, &VListModule::currentModuleChanged, m_splitter, [this](ModuleObject *child) {
            onCurrentModuleChanged(child);
        });

        view->setModel(model);
        view->setFrameShape(QFrame::NoFrame);
        view->setAutoScroll(true);
        view->setDragEnabled(false);
        view->setSpacing(5);
        view->setItemSpacing(0);
        view->setSelectionMode(QAbstractItemView::SingleSelection);
        m_view = view;

        for (auto tmpChild : q->childrens()) {
            if (tmpChild->extra()) {
                auto page = tmpChild->activePage();
                if (page) {
                    m_hlayout->addWidget(page);
                    m_extraModules.append(tmpChild);
                }
            }
        }

        auto onClicked = [](const QModelIndex &index) {
            ModuleObject *obj = static_cast<ModuleObject *>(index.internalPointer());
            if (obj && !ModuleObject::IsDisabled(obj))
                obj->trigger();
        };

        QObject::connect(m_view, &DListView::activated, m_view, &DListView::clicked);
        QObject::connect(m_view, &DListView::clicked, m_view, onClicked);
        QObject::connect(q, &ModuleObject::insertedChild, m_view, [this](ModuleObject *const childModule) { onAddChild(childModule); });
        QObject::connect(q, &ModuleObject::removedChild, m_view, [this](ModuleObject *const childModule) { onRemoveChild(childModule); });
        QObject::connect(q, &ModuleObject::childStateChanged, m_view, [this](ModuleObject *const tmpChild, uint32_t flag, bool state) {
            if (ModuleObject::IsHiddenFlag(flag)) {
                if (state)
                    onRemoveChild(tmpChild);
                else
                    onAddChild(tmpChild);
            }
        });
        QObject::connect(m_splitter, &QSplitter::splitterMoved, m_splitter, [this]([[maybe_unused]] int pos, [[maybe_unused]] int index) {
            m_splitterSize = m_splitter->sizes();
        });
        m_childMargin = 0;//20;
        if (qobject_cast<HListModule *>(q->getParent())) {
            m_childMargin = 0;//10;
            m_view->setContentsMargins(10, 0, 10, 10);
        }
        onCurrentModuleChanged(q->currentModule());
        if (m_splitterSize.isEmpty()) {
            m_splitterSize = {200, 600};
        }
        m_splitter->setSizes(m_splitterSize);
        return m_splitter;
    }

private:
    VListModule *q_ptr;
    Q_DECLARE_PUBLIC(VListModule)

    QAbstractItemView *m_view;
    QSplitter *m_splitter;
    QWidget *m_emptyWidget;
    QHBoxLayout *m_hlayout;
    QList<DCC_NAMESPACE::ModuleObject *> m_extraModules;
    int m_childMargin;
    QList<int> m_splitterSize;
};
}

VListModule::VListModule(QObject *parent)
    : ModuleObject(parent)
    , DCC_INIT_PRIVATE(VListModule)
{
}

VListModule::VListModule(const QString &name, const QString &displayName, QObject *parent)
    : ModuleObject(name, displayName, parent)
    , DCC_INIT_PRIVATE(VListModule)
{
}

VListModule::VListModule(const QString &name, const QStringList &contentText, QObject *parent)
    : ModuleObject(name, contentText, parent)
    , DCC_INIT_PRIVATE(VListModule)
{
}

VListModule::VListModule(const QString &name, const QString &displayName, const QStringList &contentText, QObject *parent)
    : ModuleObject(name, displayName, contentText, parent)
    , DCC_INIT_PRIVATE(VListModule)
{
}

VListModule::VListModule(const QString &name, const QString &displayName, const QVariant &icon, QObject *parent)
    : ModuleObject(name, displayName, icon, parent)
    , DCC_INIT_PRIVATE(VListModule)
{
}

VListModule::VListModule(const QString &name, const QString &displayName, const QString &description, QObject *parent)
    : ModuleObject(name, displayName, description, parent)
    , DCC_INIT_PRIVATE(VListModule)
{
}

VListModule::VListModule(const QString &name, const QString &displayName, const QString &description, const QVariant &icon, QObject *parent)
    : ModuleObject(name, displayName, description, icon, parent)
    , DCC_INIT_PRIVATE(VListModule)
{
}

VListModule::VListModule(const QString &name, const QString &displayName, const QString &description, const QIcon &icon, QObject *parent)
    : ModuleObject(name, displayName, description, icon, parent)
    , DCC_INIT_PRIVATE(VListModule)
{
}

VListModule::VListModule(const QString &name, const QString &displayName, const QString &description, const QStringList &contentText, const QVariant &icon, QObject *parent)
    : ModuleObject(name, displayName, description, contentText, icon, parent)
    , DCC_INIT_PRIVATE(VListModule)
{
}

VListModule::VListModule(const ModuleInitContext &message, QObject *parent)
    : ModuleObject(message, parent)
    , DCC_INIT_PRIVATE(VListModule)
{
}

VListModule::~VListModule()
{
}

QWidget *VListModule::page()
{
    Q_D(VListModule);
    return d->page();
}
