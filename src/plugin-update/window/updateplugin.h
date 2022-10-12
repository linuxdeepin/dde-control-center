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

#ifndef UPDATEPLUGIN_H
#define UPDATEPLUGIN_H

#include "interface/hlistmodule.h"
#include "interface/plugininterface.h"


class UpdateWorker;
class UpdateModel;
class UpdatePlugin : public DCC_NAMESPACE::PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.deepin.dde.ControlCenter.Update" FILE "update.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)

public:
    explicit UpdatePlugin() {}

    virtual QString name() const override;
    virtual DCC_NAMESPACE::ModuleObject *module() override;
    virtual QString location() const override;
};

// 一级页面
class UpdateModule : public DCC_NAMESPACE::HListModule
{
    Q_OBJECT
public:
    explicit UpdateModule(QObject *parent = nullptr);
    ~UpdateModule();

    UpdateWorker *work() { return m_work; }
    UpdateModel *model() { return m_model; }

protected:
    virtual void active() override;
    void syncUpdatablePackagesChanged(const bool isUpdatablePackages);

private:
    UpdateModel *m_model;
    UpdateWorker *m_work;
};

// 检查更新
class checkUpdateModule : public DCC_NAMESPACE::ModuleObject
{
    Q_OBJECT
public:
    explicit checkUpdateModule(UpdateModel *model, UpdateWorker *worker, QObject *parent = nullptr)
        : ModuleObject(parent), m_model(model), m_worker(worker)  {}
    virtual QWidget *page() override;

private:
    UpdateModel  *m_model;
    UpdateWorker *m_worker;
};

// 控件module
class UpdateTitleModule : public DCC_NAMESPACE::ModuleObject
{
    Q_OBJECT
public:
    UpdateTitleModule(const QString &name, const QString &title, QObject *parent = nullptr);
    virtual QWidget *page() override;
};

class SwitchWidgetModule : public DCC_NAMESPACE::ModuleObject
{
    Q_OBJECT
public:
    SwitchWidgetModule(const QString &name, const QString &title, QObject *parent = nullptr);
    virtual QWidget *page() override;

private:

};

#endif // UPDATEPLUGIN_H
