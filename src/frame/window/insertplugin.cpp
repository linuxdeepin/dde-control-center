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

    QString pluginSettings = "";

    QGSettings setting("com.deepin.dde.control-center", "/com/deepin/dde/control-center/");
    if (setting.keys().contains("pluginSetting")) {
        pluginSettings =  setting.get("plugin-setting").toString();
    }

    m_pluginSettingsObject = QJsonDocument::fromJson(pluginSettings.toLocal8Bit()).object();
    if (m_pluginSettingsObject.isEmpty()) {
        return;
    }

    auto moduleList = moduleDir.entryInfoList();
    for (auto i : moduleList) {
        QString path = i.absoluteFilePath();

        if (!QLibrary::isLibrary(path)) continue;

        qDebug() << "loading module: " << i;

        QPluginLoader loader(path);
        QObject *instance = loader.instance();
        if (!instance) {
            qDebug() << loader.errorString();
            continue;
        }

        instance->setParent(obj);

        auto *module = qobject_cast<ModuleInterface *>(instance);
        qDebug() << "load plugin Name;" << module->name() <<  module->displayName();
        module->setFrameProxy(frameProxy);
        m_allModules.push_back({instance, module->name()});
    }
}

bool InsertPlugin::needPushPlugin(QString moduleName)
{
    if (m_pluginSettingsObject.isEmpty())
        return false;

    m_pushModuleName = moduleName;

    return m_pluginSettingsObject.contains(moduleName);
}

InsertPlugin *InsertPlugin::instance(QObject *obj, FrameProxyInterface *FrameProxy)
{
    if (INSTANCE.isNull())
        INSTANCE = new InsertPlugin(obj, FrameProxy);

    return INSTANCE;
}

void dccV20::InsertPlugin::pushPlugin(QList<QPair<dccV20::ModuleInterface *, QString> > &modules)
{
    //社区版不加载插件
    if (DCC_NAMESPACE::IsCommunitySystem) return;

    //一级菜单插件配置mainwindow
    if (!m_pluginSettingsObject.contains("mainwindow")) return;

    auto modulePluginSetting = m_pluginSettingsObject.value("mainwindow");
    for (int i = 0 ; i < m_allModules.size(); i++) {
        //没有对应插件的配置就直接跳过
        if (modulePluginSetting[m_allModules.at(i).second].isUndefined()) continue;
        auto pluginJson = modulePluginSetting[m_allModules.at(i).second].toObject();
        auto *module = qobject_cast<ModuleInterface *>(m_allModules.at(i).first);
        //index类型,字符串就插入模块名字的后面(这个目前不方便汉化,直接配置对应模块的名字),数字就直接插入对应的位置
        if (!pluginJson.value("enable").toBool())  continue;

        if (pluginJson.value("index").isString()) {
            auto res = std::find_if(modules.begin(), modules.end(), [ = ](const QPair<ModuleInterface *, QString> &data)->bool{
                return data.first->name() == pluginJson.value("index").toString();

            });

            if (res != modules.end()) {
                modules.insert(modules.indexOf(*res) + 1, {module, module->displayName()});
            }
        } else if (pluginJson.value("index").isDouble()) {
            modules.insert((int)pluginJson.value("index").toDouble(), {module, module->displayName()});
        } else {
            qDebug() << "insert fail " << module->displayName();
        }
    }
}

void dccV20::InsertPlugin::pushPlugin(QStandardItemModel *Model, QList<dccV20::ListSubItem> &itemList)
{
    if (! m_pluginSettingsObject.contains(m_pushModuleName)) return;

    auto modulePluginSetting = m_pluginSettingsObject.value(m_pushModuleName);

    for (int i = 0 ; i < m_allModules.size(); i++) {
        if (modulePluginSetting[m_allModules.at(i).second].isUndefined()) continue;
        auto modulejson = modulePluginSetting[m_allModules.at(i).second].toObject();

        QByteArray normalizedSignature = QMetaObject::normalizedSignature("active()");
        int methodIndex = m_allModules.at(i).first->metaObject()->indexOfMethod(normalizedSignature);
        //找不到对应初始化的方法
        if (methodIndex == -1) {
            continue;
        }

        if (!modulejson.value("enable").toBool())  continue;

        auto *module = qobject_cast<ModuleInterface *>(m_allModules.at(i).first);
        module->initialize();

        DStandardItem *item = new DStandardItem;
        item->setIcon(module->icon());
        item->setText(module->displayName());

        //插件的做多显示界面,如果是四级的话需要特殊初始化
        if (modulejson.value("maxmenu").toInt() == 4) {
            module->preInitialize(false, FrameProxyInterface::PushType::CoverTop);
        } else {
            module->preInitialize(false, FrameProxyInterface::PushType::Normal);
        }

        QMetaMethod metaMethod = m_allModules.at(i).first->metaObject()->method(methodIndex);

        //二级菜单插件的位置
        if (modulejson.value("index").isUndefined()) {
            itemList.append({module->name(), module->displayName(),
                             metaMethod, m_allModules.at(i).first});
            Model->appendRow(item);
        } else {
            itemList.insert(modulejson.value("index").toInt(), {module->name(), module->displayName(),
                                                                metaMethod, m_allModules.at(i).first
                                                               });
            Model->insertRow(modulejson.value("index").toInt(), item);
        }
    }
}
