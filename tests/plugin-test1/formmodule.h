//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef FORMMODULE_H
#define FORMMODULE_H

#include "interface/moduleobject.h"

class QFormLayout;
class QScrollArea;
class FormModule : public DCC_NAMESPACE::ModuleObject
{
    Q_OBJECT
public:
    explicit FormModule(const QString &name, const QString &displayName = {}, QObject *parent = nullptr);

    QWidget *page() override;

private Q_SLOTS:
    void onCurrentModuleChanged(ModuleObject *child);

private:
    void onAddChild(DCC_NAMESPACE::ModuleObject *const childModule);
    void onRemoveChild(DCC_NAMESPACE::ModuleObject *const childModule);
    void clearData();

private:
    QMap<ModuleObject *, QWidget *> m_mapWidget;
    QScrollArea *m_area;
    QFormLayout *m_layout;
};

#endif // FORMMODULE_H
