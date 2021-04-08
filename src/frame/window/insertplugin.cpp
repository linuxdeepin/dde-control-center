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
#include "insertplugin.h"

#include <QGSettings>

#include <DStandardItem>

const QString ModuleDirectory = "/usr/lib/dde-control-center/modules";

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE

QPointer<InsertPlugin> InsertPlugin::INSTANCE = nullptr;

InsertPlugin::InsertPlugin(QObject *obj, FrameProxyInterface *frameProxy)
{
    QDir moduleDir(ModuleDirectory);
    if (!moduleDir.exists()) {
        qDebug() << "module directory not exists";
        return;
    }

    auto moduleList = moduleDir.entryInfoList();
    for (auto i : moduleList) {
        QString path = i.absoluteFilePath();

        if (!QLibrary::isLibrary(path))
            continue;

        qDebug() << "loading module: " << i;

        QPluginLoader loader(path);
        const QJsonObject &meta = loader.metaData().value("MetaData").toObject();
        if (!compareVersion(meta.value("api").toString(), "1.0.0")) {
            qDebug() << "plugin's version is too low";
            continue;
        }

        QObject *instance = loader.instance();
        if (!instance) {
            qDebug() << loader.errorString();
            continue;
        }

        instance->setParent(obj);

        auto *module = qobject_cast<ModuleInterface *>(instance);
        qDebug() << "load plugin Name;" << module->name() << module->displayName();
        module->setFrameProxy(frameProxy);

        Plugin plugin;
        plugin.path = module->path();
        plugin.follow = module->follow();
        plugin.enabled = module->enabled();

        m_allModules.push_back({plugin, {instance, module->name()}});
    }
}

bool InsertPlugin::needPushPlugin(QString moduleName)
{
    m_currentPlugins.clear();

    for (auto pluginSetting : m_allModules) {
        if (pluginSetting.first.path == moduleName) {
            m_currentPlugins << pluginSetting;
        }
    }

    return !m_currentPlugins.isEmpty();
}

InsertPlugin *InsertPlugin::instance(QObject *obj, FrameProxyInterface *FrameProxy)
{
    if (INSTANCE.isNull())
        INSTANCE = new InsertPlugin(obj, FrameProxy);

    return INSTANCE;
}

/**
 * @brief dccV20::InsertPlugin::pushPlugin 加载一级菜单插件
 * @param modules 一级菜单所有模块，将插件添加到其中
 */
void dccV20::InsertPlugin::pushPlugin(QList<QPair<dccV20::ModuleInterface *, QString>> &modules)
{
    // 一级菜单插件配置mainwindow
    for (int i = 0; i < m_currentPlugins.size(); i++) {
        auto *module = qobject_cast<ModuleInterface *>(m_currentPlugins.at(i).second.first);

        // 查看插件是否定义为可用
        if (!m_currentPlugins.at(i).first.enabled)
            continue;

        // index类型,字符串就插入模块名字的后面(这个目前不方便汉化,直接配置对应模块的名字),数字就直接插入对应的位置，空值默认添加到最后
        bool ok;
        int index = m_currentPlugins.at(i).first.follow.toInt(&ok);

        // 类型为字符串时
        if (!ok) {
            // 字符串为空时，默认置底
            if (m_currentPlugins.at(i).first.follow.isEmpty()) {
                modules.append({module, module->displayName()});
                return;
            }

            // 遍历modules查找插入位置
            auto res = std::find_if(modules.begin(), modules.end(), [=](const QPair<ModuleInterface *, QString> &data) -> bool {
                return data.first->name() == m_currentPlugins.at(i).first.follow;
            });

            // 若未找到则不添加插件
            if (res != modules.end()) {
                modules.insert(modules.indexOf(*res) + 1, {module, module->displayName()});
            } else {
                qWarning() << "insert module failed, no module named " << module->follow();
                modules.append({module, module->displayName()});
            }
        } else {
            // 为数字时直接插入到指定位置
            modules.insert(index, {module, module->displayName()});
        }
    }
}

/**
 * @brief dccV20::InsertPlugin::pushPlugin
 * @param Model 二级菜单列表
 * @param itemList
 */
void dccV20::InsertPlugin::pushPlugin(QStandardItemModel *Model, QList<dccV20::ListSubItem> &itemList)
{
    for (int i = 0; i < m_currentPlugins.size(); i++) {
        QByteArray normalizedSignature = QMetaObject::normalizedSignature("active()");
        int methodIndex = m_currentPlugins.at(i).second.first->metaObject()->indexOfMethod(normalizedSignature);
        // 找不到对应激活的方法
        if (methodIndex == -1) {
            continue;
        }

        // 查看插件是否定义为可用
        if (!m_currentPlugins.at(i).first.enabled)
            continue;

        auto *module = qobject_cast<ModuleInterface *>(m_currentPlugins.at(i).second.first);
        // 调用模块初始化函数
        module->preInitialize(false);
        module->initialize();

        DStandardItem *item = new DStandardItem;
        item->setIcon(module->icon());
        item->setText(module->displayName());

        // active方法
        QMetaMethod metaMethod = m_currentPlugins.at(i).second.first->metaObject()->method(methodIndex);

        bool ok;
        int index = m_currentPlugins.at(i).first.follow.toInt(&ok);

        //二级菜单插件的位置，为非数字默认置底
        if (ok) {
            if (index > Model->rowCount()) {
                itemList.append({module->name(), module->displayName(),
                                 metaMethod, m_currentPlugins.at(i).second.first});
                Model->appendRow(item);
            } else {
                itemList.insert(index - 1, {module->name(), module->displayName(),
                                        metaMethod, m_currentPlugins.at(i).second.first});
                Model->insertRow(index - 1, item);
            }
        } else {
            bool isLoad = false;
            for (int k = 0; k < Model->rowCount(); k++) {
                if (Model->item(k)->text() == m_currentPlugins.at(i).first.follow) {
                    itemList.insert(k + 1, {module->name(), module->displayName(),
                                            metaMethod, m_currentPlugins.at(i).second.first});
                    Model->insertRow(k + 1, item);
                    isLoad = true;
                    break;
                }
            }
            if (!isLoad) {
                itemList.append({module->name(), module->displayName(),
                                 metaMethod, m_currentPlugins.at(i).second.first});
                Model->appendRow(item);
            }
        }
    }
}
