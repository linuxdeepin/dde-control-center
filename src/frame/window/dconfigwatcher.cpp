/*
 * Copyright (C) 2020 ~ 2021 Uniontech Technology Co., Ltd.
 *
 * Author:     Tianlu Shao <shaotianlu@uniontech.com>
 *
 * Maintainer: Tianlu Shao <shaotianlu@uniontech.com>
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

#include "dconfigwatcher.h"

#include <DConfig>

#include <QListView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QVariant>
#include <QWidget>
#include <QDebug>
#include <QMetaEnum>

using namespace DTK_NAMESPACE::Core;

/*
* 控制中心增加新的配置项时，需要注意：
* 1、配置项按照模块存放，如 更新模块的配置项存放在 dde.control-center.update.dconfig.json 文件中
* 2、二级菜单的配置项会和搜索关联，所以需要配置项的名称与二级菜单的文案保持一致
* 如： 鼠标模块 General 二级菜单，配置项目也需要定义为 General
*/


DConfigWatcher::DConfigWatcher(QObject *parent)
    : QObject(parent)
{
    //通过模块枚举加载所有的文件，并从文件中获取所有的dconfig对象
    QMetaEnum metaEnum = QMetaEnum::fromType<ModuleType>();
    for (int i = 0; i <  metaEnum.keyCount(); i++) {
        const QString fileName = QString("dde.control-center.%1.dconfig").arg(metaEnum.valueToKey(i));
        DConfig *config = new DConfig(fileName);
        if (!config->isValid()) {
            qWarning() << QString("DConfig is invalide, name:[%1], subpath[%2].").arg(config->name(), config->subpath());
            continue;
        } else {
            m_mapModulesConfig.insert(metaEnum.valueToKey(i), config);
            connect(config, &DConfig::valueChanged, this, [&](QString key) {
                auto moduleName = m_mapModulesConfig.key(static_cast<DConfig *>(sender())).toStdString().c_str();
                int type = QMetaEnum::fromType<ModuleType>().keyToValue(moduleName);
                onStatusModeChanged(static_cast<ModuleType>(type), key);
            });
        }
    }
}

DConfigWatcher *DConfigWatcher::instance()
{
    static DConfigWatcher w;
    return &w;
}

/**
 * @brief DConfigWatcher::bind 三级控件绑定gsettings
 * @param moduleType            模块类型
 * @param configName            key值
 * @param binder                控件指针
 */
void DConfigWatcher::bind(ModuleType moduleType, const QString &configName, QWidget *binder)
{
    QString moduleName;
    if (!existKey(moduleType, configName, moduleName))
        return;

    //添加key值到map中
    ModuleKey *key = new ModuleKey();
    key->key = configName;
    key->type = moduleType;
    //在包含的情况下去
    m_thirdMap.insert(key, binder);
    setStatus(moduleName, configName, binder);

    // 自动解绑
    connect(binder, &QObject::destroyed, this, [ = ] {
        if (m_thirdMap.values().contains(binder))
            erase(m_thirdMap.key(binder)->type, m_thirdMap.key(binder)->key);
    });
}

/**
 * @brief DConfigWatcher::bind 二级菜单绑定gsettings
 * @param moduleType            模块类型
 * @param configName            key值
 * @param viewer                listview指针
 * @param item                  item指针
 */
void DConfigWatcher::bind(ModuleType moduleType, const QString &configName, QListView *viewer, QStandardItem *item)
{
    QString moduleName;
    if (!existKey(moduleType, configName, moduleName))
        return;

    //添加key值到map中
    ModuleKey *key = new ModuleKey();
    key->key = configName;
    key->type = moduleType;
    //在包含的情况下去
    m_menuMap.insert(key, QPair<QListView *, QStandardItem *>(viewer, item));
    setStatus(moduleName, configName, viewer, item);

    // 自动解绑
    connect(viewer, &QListView::destroyed, this, [ = ] {
        erase(moduleType, configName);
    });
}

/**
 * @brief DConfigWatcher::erase 清楚map中已被析构的值
 * @param moduleType             模块类型
 * @param configName             key值
 */
void DConfigWatcher::erase(ModuleType moduleType, const QString &configName)
{
    auto lst = m_thirdMap.keys();
    for (auto k : lst) {
        if (k->key == configName && k->type == moduleType) {
            m_thirdMap.remove(k);
        }
    }

    auto lst1 = m_menuMap.keys();
    for (auto k : lst1) {
        if (k->key == configName && k->type == moduleType) {
            m_menuMap.remove(k);
        }
    }
}

/**
 * @brief DConfigWatcher::erase erase重载，指定删除特定key
 * @param moduleType             模块类型
 * @param configName             key值
 * @param binder                 控件指针
 */
void DConfigWatcher::erase(ModuleType moduleType, const QString &configName, QWidget *binder)
{
    auto lst = m_thirdMap.keys();
    for (auto k : lst) {
        if (k->key == configName && k->type == moduleType) {
            m_thirdMap.remove(k, binder);
        }
    }
}

/**
 * @brief DConfigWatcher::insertState 插入二级菜单初始状态
 * @param moduleType                   模块类型
 * @param key                          key值
 */
void DConfigWatcher::insertState(ModuleType moduleType, const QString &key)
{
    QString moduleName;
    if (!existKey(moduleType, key, moduleName))
        return;

    ModuleKey *keys = new ModuleKey();
    keys->key = key;
    keys->type = moduleType;
    m_menuState.insert(keys, m_mapModulesConfig[moduleName]->value(key).toBool());
}

/**
 * @brief DConfigWatcher::setStatus 设置三级控件状态
 * @param moduleType                 模块类型
 * @param configName                 key值
 * @param binder                     控件指针
 */
void DConfigWatcher::setStatus(QString &moduleName, const QString &configName, QWidget *binder)
{
    if (!binder)
        return;

    const QString setting = m_mapModulesConfig[moduleName]->value(configName).toString();

    if ("Enabled" == setting) {
        binder->setEnabled(true);
        binder->update();
    } else if ("Disabled" == setting) {
        binder->setEnabled(false);
        binder->update();
    }

    binder->setVisible("Hidden" != setting);
}

/**
 * @brief DConfigWatcher::setStatus 设置二级菜单状态
 * @param moduleName                 模块名称
 * @param configName              key值
 * @param viewer                     listview指针
 * @param item                       item指针
 */
void DConfigWatcher::setStatus(QString &moduleName, const QString &configName, QListView *viewer, QStandardItem *item)
{
    bool visible = m_mapModulesConfig[moduleName]->value(configName).toBool();
    viewer->setRowHidden(item->row(), !visible);

    if (visible)
        Q_EMIT requestShowSecondMenu(item->row());
    else
        Q_EMIT requestUpdateSecondMenu(item->row(), configName);
}

/**
 * @brief DConfigWatcher::getStatus 获取三级控件状态
 * @param moduleType                 模块类型
 * @param configName                 key值
 * @return
 */
const QString DConfigWatcher::getStatus(ModuleType moduleType, const QString &configName)
{
    return m_mapModulesConfig[QMetaEnum::fromType<ModuleType>().valueToKey(moduleType)]->value(configName).toString();
}

/**
 * @brief DConfigWatcher::getMenuState
 * @return second menu state
 */
QMap<DConfigWatcher::ModuleKey *, bool> DConfigWatcher::getMenuState()
{
    return m_menuState;
}

/**
 * @brief 设置控件对应的显示类型
 * @param moduleType                 模块类型
 * @param key                        key值
 */
void DConfigWatcher::onStatusModeChanged(ModuleType moduleType, const QString &key)
{
    QString moduleName;
    if (!existKey(moduleType, key, moduleName))
        return;

    // 重新设置控件对应的显示类型
    for (auto mapUnit = m_thirdMap.begin(); mapUnit != m_thirdMap.end(); ++mapUnit) {
        if (key == mapUnit.key()->key && moduleType == mapUnit.key()->type) {
            setStatus(moduleName, key, mapUnit.value());
        }
    }

    for (auto nameKey : m_menuMap.keys()) {
        if (key == nameKey->key && moduleType == nameKey->type) {
            setStatus(moduleName, key, m_menuMap.value(nameKey).first, m_menuMap.value(nameKey).second);
            break;
        }
    }
    insertState(moduleType, key);
    ModuleKey *keys = new ModuleKey();
    keys->key = key;
    keys->type = moduleType;
    Q_EMIT requestUpdateSearchMenu(moduleName + key, m_menuState.value(keys));
}


/**
 * @brief 判断对应的key值是否存在
 * @param moduleType                 模块类型
 * @param key                        key值
 * @param moduleName                 模块名称
 * @return key is exist
 */
bool DConfigWatcher::existKey(ModuleType moduleType, const QString &key, QString &moduleName)
{
    moduleName = QMetaEnum::fromType<ModuleType>().valueToKey(moduleType);
    if (m_mapModulesConfig.keys().contains(moduleName)) {
        if (m_mapModulesConfig[moduleName]->keyList().contains(key)) {
            return true;
        }
    }
    return false;
}

DConfig *DConfigWatcher::getModulesConfig(ModuleType moduleType)
{
    QMetaEnum metaEnum = QMetaEnum::fromType<ModuleType>();
    QString key = metaEnum.key(moduleType);
    if (m_mapModulesConfig.contains(key)) {
        return m_mapModulesConfig.value(key);
    }
    return nullptr;
}
