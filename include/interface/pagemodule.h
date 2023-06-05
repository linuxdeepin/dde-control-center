// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef PAGEMODULE_H
#define PAGEMODULE_H
#include "interface/moduleobject.h"

#define DCC_PAGEMODULE_MAX_WIDTH 1120

namespace DCC_NAMESPACE {
class PageModulePrivate;

class PageModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit PageModule(QObject *parent = nullptr);
    explicit PageModule(const QString &name,
                        const QString &displayName = {},
                        QObject *parent = nullptr);
    explicit PageModule(const QString &name,
                        const QStringList &contentText,
                        QObject *parent = nullptr);
    explicit PageModule(const QString &name,
                        const QString &displayName,
                        const QStringList &contentText,
                        QObject *parent = nullptr);
    explicit PageModule(const QString &name,
                        const QString &displayName,
                        const QVariant &icon,
                        QObject *parent = nullptr);
    explicit PageModule(const QString &name,
                        const QString &displayName,
                        const QString &description,
                        QObject *parent = nullptr);
    explicit PageModule(const QString &name,
                        const QString &displayName,
                        const QString &description,
                        const QVariant &icon,
                        QObject *parent = nullptr);
    explicit PageModule(const QString &name,
                        const QString &displayName,
                        const QString &description,
                        const QIcon &icon,
                        QObject *parent = nullptr);
    explicit PageModule(const QString &name,
                        const QString &displayName,
                        const QString &description,
                        const QStringList &contentText,
                        const QVariant &icon,
                        QObject *parent = nullptr);
    explicit PageModule(const ModuleInitContext &message, QObject *parent = nullptr);
    ~PageModule() override;

    int spacing() const;
    void setSpacing(const int spacing);
    void getContentsMargins(int *left, int *top, int *right, int *bottom) const;
    void setContentsMargins(int left, int top, int right, int bottom);
    int maximumWidth() const;
    void setMaximumWidth(int maxw);
    int minimumWidth() const;
    void setMinimumWidth(int minw);
    // 无滚动条
    bool noScroll();
    void setNoScroll(bool value = true);
    // 无下方弹簧
    bool noStretch();
    void setNoStretch(bool value = true);

    void appendChild(ModuleObject *const module) override;
    void insertChild(QList<ModuleObject *>::iterator before, ModuleObject *const module) override;
    void insertChild(const int index, ModuleObject *const module) override;
    void removeChild(ModuleObject *const module) override;
    void removeChild(const int index) override;

    void appendChild(ModuleObject *const module,
                     int stretch,
                     Qt::Alignment alignment = Qt::Alignment());
    void insertChild(QList<ModuleObject *>::iterator before,
                     ModuleObject *const module,
                     int stretch,
                     Qt::Alignment alignment = Qt::Alignment());
    void insertChild(const int index,
                     ModuleObject *const module,
                     int stretch,
                     Qt::Alignment alignment = Qt::Alignment());

    QWidget *page() override;

    DCC_DECLARE_PRIVATE(PageModule)
};
} // namespace DCC_NAMESPACE
#endif // PAGEMODULE_H
