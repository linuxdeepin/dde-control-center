/*
 * Copyright (C) 2019 ~ 2020 Untion Tech Technology Co., Ltd.
 *
 * Author:     niecheng <niejiashan@163.com>
 *
 * Maintainer: niecheng <niejiashan@163.com>
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

namespace DCC_NAMESPACE {

class InsertPlugin: public QObject
{
    Q_OBJECT
public:
    InsertPlugin(QObject *parent, FrameProxyInterface *interface);
    //查询改模块是否需要加载插件
    bool needPushPlugin(QString moduleName);
    //一级菜单插入插件
    void pushPlugin(QList<QPair<ModuleInterface *, QString>> &modules);
    //二级菜单插入插件
    void pushPlugin(QStandardItemModel *Model, QList<ListSubItem> &itemList);
    //获取单例
    static InsertPlugin *instance(QObject *obj = nullptr, FrameProxyInterface *interface = nullptr);

private:
    static QPointer<InsertPlugin> INSTANCE;
    //保存加载的所有插件
    QList<QPair<QObject *, QString>> m_allModules;

    QJsonObject m_pluginSettingsObject;
    QString m_pushModuleName = "";
};
}
#endif // INSERTPLUGIN_H
