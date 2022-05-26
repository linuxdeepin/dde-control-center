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
#ifndef LAYOUTBASE_H
#define LAYOUTBASE_H

#include <QList>
#include "interface/moduleobject.h"

class QWidget;
class QBoxLayout;
DCC_BEGIN_NAMESPACE
/**
 * @brief The LayoutBase class
 * 布局类，实现基于module布局，需动态处理module的删除、添加、隐藏、禁用等设置
 */
class LayoutBase
{
public:
    /**
     * @brief autoExpand 自动展开项
     * @param module 当前ModuleObject
     * @param child 子项 nullptr 未指定具体项
     * @return nullptr 不展开 其他 展开为其他项
     */
    virtual DCC_NAMESPACE::ModuleObject *autoExpand(DCC_NAMESPACE::ModuleObject *const module, DCC_NAMESPACE::ModuleObject *const child);
    /**
     * @brief setCurrent 设置当前项
     * @param child 当前项
     */
    virtual void setCurrent(DCC_NAMESPACE::ModuleObject *const child) = 0;
    /**
     * @brief layoutModule 完成module布局
     * @param module 需要布局的module
     * @param parent 用到的QWidget
     * @param index 子项索引
     * @return 子项的QWidget
     */
    virtual QWidget *layoutModule(DCC_NAMESPACE::ModuleObject *const module, QWidget *const parent, DCC_NAMESPACE::ModuleObject *const child) = 0;
    /**
     * @brief IsVisible 返回module是否显示，判断了配置项和程序设置项
     * @param module
     * @return
     */
    static bool IsHiden(DCC_NAMESPACE::ModuleObject *const module);
    // 是否为隐藏标志
    static bool IsHidenFlag(uint32_t flag);
    /**
     * @brief IsEnabled 返回module是否可用，判断了配置项和程序设置项
     * @param module
     * @return
     */
    static bool IsDisabled(DCC_NAMESPACE::ModuleObject *const module);
    // 是否为禁用标志
    static bool IsDisabledFlag(uint32_t flag);
};

class LayoutFactoryBase
{
public:
    virtual DCC_LAYOUT_TYPE type() = 0;
    virtual LayoutBase *createLayout() = 0;
};

template<DCC_LAYOUT_TYPE TYPE, typename T>
class LayoutFactory : public LayoutFactoryBase
{
public:
    virtual DCC_LAYOUT_TYPE type() { return TYPE; }
    virtual LayoutBase *createLayout() { return new T(); }
};

DCC_END_NAMESPACE
#endif // LAYOUTBASE_H
