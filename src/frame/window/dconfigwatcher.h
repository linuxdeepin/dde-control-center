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

#ifndef DCONFIGWATCHER_H
#define DCONFIGWATCHER_H

#include <QObject>
#include <QHash>
#include <QMap>

#include <DConfig>

class QListView;
class QStandardItem;

DCORE_USE_NAMESPACE

class DConfigWatcher : public QObject
{
    Q_OBJECT
public:
    enum ModuleType {
        mainwindow,
        authentication,
        accounts,
        bluetooth,
        cloudsync,
        datetime,
        display,
        defapp,
        mouse,
        notifiction,
        personalization,
        power,
        sound,
        keyboard,
        touchscreen,
        wacom,
        update,
        systeminfo,
        commoninfo
    };
    Q_ENUM(ModuleType)

    typedef struct ModuleKey {
        ModuleType type;
        QString key;

        ModuleKey()
        {
            this->type = ModuleType::mainwindow;
            this->key = "";
        }

        void operator=(ModuleKey &moduleKey)
        {
            type = moduleKey.type;
            key = moduleKey.key;
        }

        bool operator==(ModuleKey &moduleKey) const
        {
            return (type == moduleKey.type && key == moduleKey.key);
        }

        bool operator<(const ModuleKey &moduleKey) const
        {
            return (type < moduleKey.type);
        }
        bool operator>(const ModuleKey &moduleKey) const
        {
            return (type > moduleKey.type);
        }

    } ModuleKey;

    static DConfigWatcher *instance();

    void bind(ModuleType moduleType, const QString &configName, QWidget *binder);
    void bind(ModuleType moduleType, const QString &configName, QListView *viewer, QStandardItem *item);
    void erase(ModuleType moduleType, const QString &configName);
    void erase(ModuleType moduleType, const QString &configName, QWidget *binder);
    void insertState(ModuleType moduleType, const QString &);
    const QString getStatus(ModuleType moduleType, const QString &configName);
    DConfig *getModulesConfig(ModuleType moduleType);
    QMap<ModuleKey *, bool> getMenuState();

private:
    DConfigWatcher(QObject *parent = nullptr);

    void setStatus(QString &moduleName, const QString &configName, QWidget *binder);
    void setStatus(QString &moduleName, const QString &configName, QListView *viewer, QStandardItem *item);
    void onStatusModeChanged(ModuleType moduleType, const QString &key);
    bool existKey(ModuleType moduleType, const QString &key, QString &moduleName);
Q_SIGNALS:
    void requestUpdateSecondMenu(int);
    void requestUpdateSearchMenu(const QString &, bool);
    void requestShowSecondMenu(int); //显示第二级页面

private:
    QMultiHash<ModuleKey *, QWidget *> m_thirdMap; //三级菜单 map
    QMap<ModuleKey *, QPair<QListView *, QStandardItem *>> m_menuMap;  //二级菜单 map
    QMap<ModuleKey *, bool> m_menuState;
    QMap<QString, DConfig *> m_mapModulesConfig; //模块名称-配置 map
};

#endif // DCONFIGWATCHER_H
