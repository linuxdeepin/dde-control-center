// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HLISTMODULE_H
#define HLISTMODULE_H
#include "interface/moduleobject.h"

namespace DCC_NAMESPACE {
class HListModulePrivate;

class HListModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit HListModule(QObject *parent = nullptr);
    explicit HListModule(const QString &name,
                         const QString &displayName = {},
                         QObject *parent = nullptr);
    explicit HListModule(const QString &name,
                         const QStringList &contentText,
                         QObject *parent = nullptr);
    explicit HListModule(const QString &name,
                         const QString &displayName,
                         const QStringList &contentText,
                         QObject *parent = nullptr);
    explicit HListModule(const QString &name,
                         const QString &displayName,
                         const QVariant &icon,
                         QObject *parent = nullptr);
    explicit HListModule(const QString &name,
                         const QString &displayName,
                         const QString &description,
                         QObject *parent = nullptr);
    explicit HListModule(const QString &name,
                         const QString &displayName,
                         const QString &description,
                         const QVariant &icon,
                         QObject *parent = nullptr);
    explicit HListModule(const QString &name,
                         const QString &displayName,
                         const QString &description,
                         const QIcon &icon,
                         QObject *parent = nullptr);
    explicit HListModule(const QString &name,
                         const QString &displayName,
                         const QString &description,
                         const QStringList &contentText,
                         const QVariant &icon,
                         QObject *parent = nullptr);
    explicit HListModule(const ModuleInitContext &message, QObject *parent = nullptr);
    ~HListModule() override;

    QWidget *page() override;

    inline DCC_MODULE_TYPE getClassID() const override { return HLISTLAYOUT; }

    DCC_DECLARE_PRIVATE(HListModule)
};
} // namespace DCC_NAMESPACE
#endif // HLISTMODULE_H
