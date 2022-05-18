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

#ifndef UOSIDPLUGIN_H
#define UOSIDPLUGIN_H

#include "interface/moduleobject.h"
#include "interface/plugininterface.h"

DCC_USE_NAMESPACE

class SyncModel;
class SyncWorker;
class UosIDPlugin : public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PluginInterface_iid FILE "uosid.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)

public:
    explicit UosIDPlugin() {}

    virtual QString name() const override;
    virtual ModuleObject *module() override;
};

// 模块
class UosIDModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit UosIDModule(QObject *parent = nullptr);
    ~UosIDModule();

    SyncWorker *work() { return m_work; }
    SyncModel *model() { return m_model; }

protected:
    virtual void active() override;

private:
    SyncModel *m_model;
    SyncWorker *m_work;
};

// 界面
class UosIDDetailsModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit UosIDDetailsModule(SyncModel *model, SyncWorker *worker, QObject *parent = nullptr)
        : ModuleObject(parent), m_model(model), m_worker(worker)  {}

    virtual QWidget *page() override;

private:
    SyncModel  *m_model;
    SyncWorker *m_worker;
};

#endif // UOSIDPLUGIN_H
