// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
    void requestUpdateSecondMenu(int, const QString &gsettingsName = QString());
    void requestUpdateSearchMenu(const QString &, bool);
    void requestShowSecondMenu(int); //显示第二级页面
    void notifyDConfigChanged(const QString &, const QString &);

private:
    QMultiHash<ModuleKey *, QWidget *> m_thirdMap; //三级菜单 map
    QMap<ModuleKey *, QPair<QListView *, QStandardItem *>> m_menuMap;  //二级菜单 map
    QMap<ModuleKey *, bool> m_menuState;
    QMap<QString, DConfig *> m_mapModulesConfig; //模块名称-配置 map
};

#endif // DCONFIGWATCHER_H
