/*
 * Copyright (C) 2021 ~ 2023 Deepin Technology Co., Ltd.
 *
 * Author:     caixiangrong <caixiangrong@uniontech.com>
 *
 * Maintainer: caixiangrong <caixiangrong@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef PAGEMODULE_H
#define PAGEMODULE_H
#include "interface/moduleobject.h"

DCC_BEGIN_NAMESPACE
class PageModulePrivate;
class PageModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit PageModule(QObject *parent = nullptr);
    PageModule(const QString &name, const QString &displayName = {}, QObject *parent = nullptr);
    PageModule(const QString &name, const QStringList &contentText, QObject *parent = nullptr);
    PageModule(const QString &name, const QString &displayName, const QStringList &contentText, QObject *parent = nullptr);
    PageModule(const QString &name, const QString &displayName, const QVariant &icon, QObject *parent = nullptr);
    PageModule(const QString &name, const QString &displayName, const QString &description, QObject *parent = nullptr);
    PageModule(const QString &name, const QString &displayName, const QString &description, const QVariant &icon, QObject *parent = nullptr);
    PageModule(const QString &name, const QString &displayName, const QString &description, const QIcon &icon, QObject *parent = nullptr);
    PageModule(const QString &name, const QString &displayName, const QString &description, const QStringList &contentText, const QVariant &icon, QObject *parent = nullptr);
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

    void appendChild(ModuleObject *const module, int stretch, Qt::Alignment alignment = Qt::Alignment());
    void insertChild(QList<ModuleObject *>::iterator before, ModuleObject *const module, int stretch, Qt::Alignment alignment = Qt::Alignment());
    void insertChild(const int index, ModuleObject *const module, int stretch, Qt::Alignment alignment = Qt::Alignment());

    QWidget *page() override;

    DCC_DECLARE_PRIVATE(PageModule)
};
DCC_END_NAMESPACE
#endif // PAGEMODULE_H
