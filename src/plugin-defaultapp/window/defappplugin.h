// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DEFAPPPLUGIN_H
#define DEFAPPPLUGIN_H

#include "defappworker.h"
#include "defappworkerold.h"

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

    DefAppWorkerOld *oldWork() { return m_oldwork; }

    DefAppModel *model() { return m_model; }

    bool isOldInterface() { return m_isOldInterface; }

protected:
    virtual void active() override;

private:
    DefAppModel *m_model;
    DefAppWorker *m_work;
    DefAppWorkerOld *m_oldwork;
    ModuleObject *m_defApps;
    bool m_isOldInterface;
};

#endif // DEFAPPPLUGIN_H
