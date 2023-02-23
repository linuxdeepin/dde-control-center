// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DEFAPPPLUGIN_H
#define DEFAPPPLUGIN_H

#include "defappworker.h"
#include "interface/pagemodule.h"
#include "interface/plugininterface.h"
#include "interface/vlistmodule.h"

class DefAppModel;

// 默认程序插件
class DefAppPlugin : public DCC_NAMESPACE::PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.deepin.dde.ControlCenter.DefApp" FILE "defaultapp.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    explicit DefAppPlugin() { }

    virtual QString name() const override;
    virtual DCC_NAMESPACE::ModuleObject *module() override;
    virtual QString location() const override;
};

// 一级菜单
class DefAppModule : public DCC_NAMESPACE::VListModule
{
    Q_OBJECT
public:
    explicit DefAppModule(QObject *parent = nullptr);
    ~DefAppModule() override;

    DefAppWorker *work() { return m_work; }

    DefAppModel *model() { return m_model; }

protected:
    virtual void active() override;

private:
    DefAppModel *m_model;
    DefAppWorker *m_work;
    ModuleObject *m_defApps;
};

// 二级按钮菜单
class DefAppsButtonModule : public DCC_NAMESPACE::PageModule
{
    Q_OBJECT
public:
    explicit DefAppsButtonModule(DefAppWorker::DefaultAppsCategory category,
                                 const QString &name,
                                 const QString &displayName,
                                 const QString &icon,
                                 DefAppModel *model,
                                 DefAppWorker *work);
    ~DefAppsButtonModule();

    //    virtual QWidget *page() override;

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
    DefappDetailModule(DefAppWorker::DefaultAppsCategory category,
                       DefAppModel *model,
                       DefAppWorker *work);
    virtual QWidget *page() override;

private:
    DefAppWorker::DefaultAppsCategory m_category;
    DefAppModel *m_model;
    DefAppWorker *m_work;
};

#endif // DEFAPPPLUGIN_H
