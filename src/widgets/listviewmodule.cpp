//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "widgets/listviewmodule.h"

#include <widgets/dcclistview.h>
#include <widgets/modulelistmodel.h>
#include <widgets/moduleobjectitem.h>

#include <DStyledItemDelegate>

using namespace DCC_NAMESPACE;

namespace DCC_NAMESPACE {
class ListViewModulePrivate
{
public:
    explicit ListViewModulePrivate(ListViewModule *object)
        : q_ptr(object)
        , m_model(new ModuleListModel(object))
    {
    }

public:
    ListViewModule *q_ptr;
    Q_DECLARE_PUBLIC(ListViewModule)
    ModuleListModel *m_model;
};
}

ListViewModule::ListViewModule(const QString &name, const QString &displayName, QObject *parent)
    : ModuleObject(name, displayName, parent)
    , DCC_INIT_PRIVATE(ListViewModule)
{
}

ListViewModule::~ListViewModule()
{
}

QWidget *ListViewModule::page()
{
    Q_D(ListViewModule);
    DCCListView *view = new DCCListView();
    view->setModel(d->m_model);
    connect(view, &DCCListView::clicked, this, [this](const QModelIndex &index) {
        ModuleObject *module = static_cast<ModuleObject *>(index.internalPointer());
        if (!module || ModuleObject::IsDisabled(module))
            return;
        emit clicked(module);
        ModuleObjectItem *item = qobject_cast<ModuleObjectItem *>(module);
        if (item)
            emit item->clicked();
    });
    return view;
}
