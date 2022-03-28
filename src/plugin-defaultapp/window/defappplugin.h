/*
* Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
*
* Author:     duanhongyu <duanhongyu@uniontech.com>
*
* Maintainer: duanhongyu <duanhongyu@uniontech.com>
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

#ifndef DEFAPPPLUGIN_H
#define DEFAPPPLUGIN_H

#include "interface/moduleobject.h"
#include "interface/plugininterface.h"

DCC_USE_NAMESPACE

class DefAppModel;
class DefAppWorker;
// 默认程序插件
class DefAppPlugin : public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PluginInterface_iid FILE "defaultapp.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    explicit DefAppPlugin() {}

    virtual QString name() const override;
    virtual ModuleObject *module() override;
};


// 一级菜单
class DefAppModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit DefAppModule(QObject *parent = nullptr);
    ~DefAppModule() override;
    virtual void active() override{}
    DefAppWorker *work() { return m_work; }
    DefAppModel *model() { return m_model; }

private:
    DefAppModel *m_model;
    DefAppWorker *m_work;
    ModuleObject *m_defApps;

};

// 二级按钮菜单
class DefAppsButtonModule : public DCC_NAMESPACE::ModuleObject
{
    Q_OBJECT
public:
    explicit DefAppsButtonModule(DefAppWorker::DefaultAppsCategory category,
                               const QString &name, const QString &displayName, const QIcon &icon,
                               DefAppModel *model, DefAppWorker *work);
    ~DefAppsButtonModule();

    virtual QWidget *page() override;
    virtual QWidget *extraButton() override;

Q_SIGNALS:
    void onButtonClicked();

private:
    DefAppWorker::DefaultAppsCategory m_category;
    DefAppModel *m_model;
    DefAppWorker *m_work;
};

// 三级菜单
class DefappDetailModule : public DCC_NAMESPACE::ModuleObject
{
    Q_OBJECT
public:
    DefappDetailModule(DefAppWorker::DefaultAppsCategory category, DefAppModel *model, DefAppWorker *work);
    virtual QWidget *page() override;

private:
    DefAppWorker::DefaultAppsCategory m_category;
    DefAppModel *m_model;
    DefAppWorker *m_work;
};

#endif // DEFAPPPLUGIN_H
