//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MAINMODULE_H
#define MAINMODULE_H
#include "interface/moduleobject.h"
#include <DMainWindow>

namespace DCC_NAMESPACE {
class MainModulePrivate;
class MainModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit MainModule(DTK_WIDGET_NAMESPACE::DMainWindow *parent = nullptr);
    ~MainModule() override;

    QWidget *page() override;
    ModuleObject *defultModule() override;
    inline DCC_MODULE_TYPE getClassID() const override { return MAINLAYOUT; }

    DCC_DECLARE_PRIVATE(MainModule)
};
}
#endif // MAINMODULE_H
