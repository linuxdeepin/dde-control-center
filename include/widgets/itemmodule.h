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
#ifndef ITEMMODULE_H
#define ITEMMODULE_H
#include "interface/moduleobject.h"

namespace DCC_NAMESPACE {
class ItemModulePrivate;

class ItemModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit ItemModule(QObject *parent = nullptr);
    ItemModule(const QString &name, const QString &displayName, bool isTitle = true);

    template<typename FunT>
    ItemModule(const QString &name,
               const QString &displayName,
               FunT callback,
               bool leftVisible = true)
        : ItemModule(name, displayName, false)
    {
        setLeftVisible(leftVisible);
        setRightWidget(callback);
    }

    template<class classT, class WidgetT>
    ItemModule(const QString &name,
               const QString &displayName,
               classT *receiver,
               WidgetT *(classT::*callback)(ModuleObject *),
               bool leftVisible = true)
        : ItemModule(name, displayName, false)
    {
        setLeftVisible(leftVisible);
        setRightWidget(receiver, callback);
    }

    template<class classT, class WidgetT>
    ItemModule(const QString &name,
               const QString &displayName,
               classT *receiver,
               WidgetT *(classT::*callback)(),
               bool leftVisible = true)
        : ItemModule(name, displayName, false)
    {
        setLeftVisible(leftVisible);
        setRightWidget(receiver, callback);
    }

    ~ItemModule() override;

    void setTitleItem(bool isTitle);
    void setBackground(bool has);
    void setWordWrap(bool on);
    bool wordWrap() const;
    void setLeftVisible(bool visible);
    bool clickable() const;
    void setClickable(const bool clickable);

    template<typename FunT>
    void setRightWidget(FunT callback)
    {
        auto fun = [](FunT callback, ModuleObject *module) {
            return (callback)(module);
        };
        setCallback(std::bind(fun, callback, this));
    }

    template<class classT, class WidgetT>
    void setRightWidget(classT *receiver, WidgetT *(classT::*callback)(ModuleObject *))
    {
        auto fun = [](classT *receiver,
                      WidgetT *(classT::*callback)(ModuleObject *),
                      ModuleObject *module) {
            return (receiver->*callback)(module);
        };
        setCallback(std::bind(fun, receiver, callback, this));
    }

    template<class classT, class WidgetT>
    void setRightWidget(classT *receiver, WidgetT *(classT::*callback)())
    {
        auto fun = [](classT *receiver, WidgetT *(classT::*callback)()) {
            return (receiver->*callback)();
        };
        setCallback(std::bind(fun, receiver, callback));
    }

    QWidget *page() override;

    inline DCC_MODULE_TYPE getClassID() const override { return ITEM; }

Q_SIGNALS:
    void clicked(QWidget *widget);

private:
    void setCallback(std::function<QWidget *()> callback);

    DCC_DECLARE_PRIVATE(ItemModule)
};
} // namespace DCC_NAMESPACE
#endif // ITEMMODULE_H
