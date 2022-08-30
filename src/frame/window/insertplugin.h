// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef INSERTPLUGIN_H
#define INSERTPLUGIN_H

#include <QObject>
#include <QDir>
#include <QLibrary>
#include <QPluginLoader>
#include <QStandardItemModel>
#include <QJsonObject>

#include "interface/moduleinterface.h"
#include "window/utils.h"

namespace DCC_NAMESPACE
{

    struct Plugin
    {
        QString path;   // 插件级别及二级菜单插件所属模块
        QString follow; // 插件插入位置，可以字符串或者数字
        bool enabled;   // 插件是否处于可用状态
    };

    class InsertPlugin : public QObject
    {
        Q_OBJECT
    public:
        InsertPlugin(QObject *parent, FrameProxyInterface *interface);
        // 查询改模块是否需要加载插件
        bool updatePluginInfo(QString moduleName);
        // 一级菜单插入插件
        void pushPlugin(QList<QPair<ModuleInterface *, QString>> &modules);
        // 二级菜单插入插件
        void pushPlugin(QStandardItemModel *Model, QList<ListSubItem> &itemList);

        QStringList availPages(const QString &moduleName);
        // 获取单例
        static InsertPlugin *instance(QObject *obj = nullptr, FrameProxyInterface *interface = nullptr);
        // 初始化插件搜索数据
        void preInitialize(QString moduleName);
        // 获取对应displayName的插件对象
        ModuleInterface *pluginInterface(const QString &displayName);

    private:
        static QPointer<InsertPlugin> INSTANCE;
        // 保存加载的所有插件
        QList<QPair<Plugin, QPair<QObject *, QString>>> m_allModules;
        // 保存插入到某个模块的所有插件
        QList<QPair<Plugin, QPair<QObject *, QString>>> m_currentPlugins;
    };
}
#endif // INSERTPLUGIN_H
