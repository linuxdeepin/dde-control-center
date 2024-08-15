// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCC_PLUGININTERFACE_H
#define DCC_PLUGININTERFACE_H

#include "namespace.h"

#include <QObject>

namespace DCC_NAMESPACE {
class ModuleObject;

class PluginInterface : public QObject
{
    Q_OBJECT
public:
    PluginInterface(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

    virtual ~PluginInterface() { }

    /**
     * @brief 插件最基本的元素由 ModuleObject 组成，所以最少有一个 ModuleObject
     * @brief 插件子项列表
     */
    virtual ModuleObject *module() = 0;

    /**
     * @brief 标识插件信息
     * @return 插件名称
     */
    virtual QString name() const { return QString(); }

    /**
     * @brief 插件必须知道其需要跟随的父ModuleObject的url ,默认为空则为一级插件
     * @return 跟随的父ModuleObject的url
     */
    virtual QString follow() const { return QString(); }

    /**
     * @brief 插件位置索引，相同索引则按加载顺序进行排序，先加载的往后顺延，默认追加到最后
     * @return 位置索引或前一个name
     */
    virtual QString location() const { return QString(); }
};

} // namespace DCC_NAMESPACE

Q_DECLARE_INTERFACE(DCC_NAMESPACE::PluginInterface, "org.deepin.dde.ControlCenter.Plugin/1.4")

#endif // DCC_PLUGININTERFACE_H
