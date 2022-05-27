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
     * @brief autoExpand 自动展开项 默认为自动展开第0项
     * @param module 当前ModuleObject
     * @param children 将显示的子孙项
     * @return nullptr 不展开 其他 展开为其他项
     */
    virtual DCC_NAMESPACE::ModuleObject *autoExpand(DCC_NAMESPACE::ModuleObject *const module, const QList<DCC_NAMESPACE::ModuleObject *> &children);
    /**
     * @brief checkUpdate 检查从oldChild转到newChild是否要更新布局，需要则layoutModule,setCurrent会被调用，否则只调用setCurrent 默认不更新布局
     * @param oldChild 原选中项
     * @param newChild 新选中项
     * @return true 需要更新 false 不需要更新
     */
    virtual bool checkUpdate(DCC_NAMESPACE::ModuleObject *const oldChild, DCC_NAMESPACE::ModuleObject *const newChild);
    /**
     * @brief setCurrent 设置当前项
     * @param child 当前项
     */
    virtual void setCurrent(DCC_NAMESPACE::ModuleObject *const child) = 0;
    /**
     * @brief layoutModule 完成module布局
     * @param module 需要布局的module
     * @param parent 用到的QWidget
     * @param children 将显示的子孙项
     * @return 子项的QWidget
     */
    virtual QWidget *layoutModule(DCC_NAMESPACE::ModuleObject *const module, QWidget *const parent, const QList<DCC_NAMESPACE::ModuleObject *> &children) = 0;
    /**
     * @brief IsVisible 返回module是否显示，判断了配置项和程序设置项
     * @param module
     * @return
     */
    static bool IsHiden(DCC_NAMESPACE::ModuleObject *const module);
    /**
     * @brief IsHidenFlag 判断标志是否为隐藏标志
     * @param flag 标志
     * @return true 有隐藏标志位 false 没有隐藏标志位
     */
    static bool IsHidenFlag(uint32_t flag);
    /**
     * @brief IsEnabled 返回module是否可用，判断了配置项和程序设置项
     * @param module
     * @return
     */
    static bool IsDisabled(DCC_NAMESPACE::ModuleObject *const module);
    /**
     * @brief IsDisabledFlag 判断标志是否为禁用标志
     * @param flag 标志
     * @return true 有禁用标志位 false 没有禁用标志位
     */
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
