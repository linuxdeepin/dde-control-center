//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
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
    virtual void deactive() override;
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
