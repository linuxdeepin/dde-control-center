/*
 * Copyright (C) 2022 ~ 2022 Deepin Technology Co., Ltd.
 *
 * Author:     guoyao <guoyao@uniontech.com>
 *
 * Maintainer: guoyao <guoyao@uniontech.com>
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

#pragma once

#include "moduledata.h"

#include <QIcon>
#include <QWidget>
#include <QPointer>
#include <QDebug>

namespace DCC_NAMESPACE
{

/**
 * @brief ModuleObject作为规范每个Module的接口，每个Module必须提供其基本的信息
 */
class ModuleObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ModuleData* moduleData READ moduleData WRITE setModuleData NOTIFY moduleDataChanged)
public:
    enum class ChildType{
        MainIcon,  //主菜单图标模式显示
        MainList,  //主菜单列表模式显示
        HList,     //横向菜单列表显示
        VList,     //纵向菜单列表显示
        Page       //页面显示
    };
    ModuleObject(QObject *parent = nullptr);
    ModuleObject(const QString &name, const QString &displayName = {}, QObject *parent = nullptr);
    ModuleObject(const QString &name, const QStringList &contentText, QObject *parent = nullptr);
    ModuleObject(const QString &name, const QString &displayName, const QStringList &contentText, QObject *parent = nullptr);
    ModuleObject(const QString &name, const QString &displayName, const QIcon &icon, QObject *parent = nullptr);
    ModuleObject(const QString &name, const QString &displayName, const QString &description, const QIcon &icon = QIcon(), QObject *parent = nullptr);
    ModuleObject(const QString &name, const QString &displayName, const QString &description, const QStringList &contentText, const QIcon &icon = QIcon(), QObject *parent = nullptr);

    virtual ~ModuleObject() { deactive(); }

    /**
     * @brief 当进入模块时，active会被调用，如无需通知则可不实现
     */
    virtual void active() {}

    /**
     * @brief 当离开模块时，deactive会被调用，如无需通知则可不实现
     */
    virtual void deactive() {}

    /**
     * @brief 每次获取均需生产新的QWidget
     * @return 当ChildType为Page/PageList时，需返回相应的页面
     */
    virtual QWidget *page() { return nullptr; }


Q_SIGNALS:
    /**
     * @brief ModuleData 改变后必须发送此信号
     */
    void moduleDataChanged(ModuleData *);

    /**
     * @brief 添加child后触发
     */
    void appendedChild(ModuleObject *const module);
    /**
     * @brief 删除child前触发
     */
    void removedChild(ModuleObject *const module);
    /**
     * @brief 插入child后触发
     */
    void insertedChild(ModuleObject *const module);
    /**
     * @brief childrens() 改变后必须发送此信号
     */
    void childrenSizeChanged(const int size);
    /**
     * @brief 显示 child 可以发送此信号
     */
    void activeChild(const int index);

public:
    /**
     * @brief 元数据封装，方便调用
     * @return 元数据的Name
     */
    inline QString name() const {
        return moduleData()->Name;
    }

    /**
     * @brief 搜索子项，使用广度搜索
     * @param child 子项
     * @return 子项所在的层数，返回0表示module自己，-1表示未搜索到
     */
    int findChild(ModuleObject *const child);

    static int findChild(ModuleObject *const module, ModuleObject *const child);

    /**
     * @brief 子项类型，由类型决定view显示效果
     */
    inline ChildType childType() const { return m_childType; }

    /**
     * @brief 设置子项类型
     */
    inline void setChildType(const ChildType &t) {
        m_childType = t;
    }
    /**
     * @brief 模块元数据，基本信息
     * @return 数据内容
     */
    inline ModuleData *moduleData() const { return m_moduleData; }

    /**
     * @brief 设置模块元数据
     * @param data 元数据
     */
    void setModuleData(ModuleData *data);

    inline bool hasChildrens() { return !childrens().isEmpty(); }

    /**
     * @brief 子项，不可直接使用QList进行增删改操作，应使用appendChild、removeChild、insertChild
     * @return 子项列表
     */
    inline const QList<ModuleObject *> &childrens() { return m_childrens; }
    inline ModuleObject* children(const int index) const {
        if(index<0 || index >= m_childrens.size())
            return nullptr;
        return m_childrens.at(index);
    }
    inline int getChildrenNumber() const { return m_childrens.size(); }

    void appendChild(ModuleObject *const module);
    void removeChild(ModuleObject *const module);
    void removeChild(const int index);
    void insertChild(QList<ModuleObject *>::iterator before, ModuleObject *const module);
    void insertChild(const int index, ModuleObject *const module);

private:
    static int findChild(ModuleObject *const module, ModuleObject *const child, const int num);

private:
    ModuleData              *m_moduleData;
    QList<ModuleObject*> m_childrens;
    ChildType               m_childType;
};

} // namespace DCC_NAMESPACE
