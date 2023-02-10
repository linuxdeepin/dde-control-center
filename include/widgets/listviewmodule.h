//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LISTVIEWMODULE_H
#define LISTVIEWMODULE_H

#include "interface/moduleobject.h"
namespace DCC_NAMESPACE {
class ListViewModulePrivate;
class ListViewModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit ListViewModule(const QString &name, const QString &displayName, QObject *parent = nullptr);
    ~ListViewModule() override;

    QWidget *page() override;
    inline DCC_MODULE_TYPE getClassID() const override { return LISTVIEW; }

Q_SIGNALS:
    void clicked(ModuleObject *module);

    DCC_DECLARE_PRIVATE(ListViewModule)
};
}
#endif // LISTVIEWMODULE_H
