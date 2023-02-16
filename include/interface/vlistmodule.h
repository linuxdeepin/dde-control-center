//Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef VLISTMODULE_H
#define VLISTMODULE_H
#include "interface/moduleobject.h"

namespace DCC_NAMESPACE {
class VListModulePrivate;
class VListModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit VListModule(QObject *parent = nullptr);
    VListModule(const QString &name, const QString &displayName = {}, QObject *parent = nullptr);
    VListModule(const QString &name, const QStringList &contentText, QObject *parent = nullptr);
    VListModule(const QString &name, const QString &displayName, const QStringList &contentText, QObject *parent = nullptr);
    VListModule(const QString &name, const QString &displayName, const QVariant &icon, QObject *parent = nullptr);
    VListModule(const QString &name, const QString &displayName, const QString &description, QObject *parent = nullptr);
    VListModule(const QString &name, const QString &displayName, const QString &description, const QVariant &icon, QObject *parent = nullptr);
    VListModule(const QString &name, const QString &displayName, const QString &description, const QIcon &icon, QObject *parent = nullptr);
    VListModule(const QString &name, const QString &displayName, const QString &description, const QStringList &contentText, const QVariant &icon, QObject *parent = nullptr);
    ~VListModule() override;

    QWidget *page() override;
    inline DCC_MODULE_TYPE getClassID() const override { return VLISTLAYOUT; }

    DCC_DECLARE_PRIVATE(VListModule)
};
}
#endif // VLISTMODULE_H
