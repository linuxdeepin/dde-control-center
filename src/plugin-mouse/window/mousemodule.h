//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"
#include "interface/hlistmodule.h"
#include "interface/plugininterface.h"

#include <QObject>

namespace DCC_NAMESPACE {
class MouseWidget;
class GeneralSettingWidget;
class MouseSettingWidget;
class TouchpadSettingWidget;
class TrackPointSettingWidget;

class MouseModel;
class MouseWorker;
class MouseDBusProxy;

class MousePlugin : public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.deepin.dde.ControlCenter.Mouse" FILE "MousePlugin.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    virtual QString name() const override;
    virtual ModuleObject *module() override;
    virtual QString location() const override;
};

//一级菜单
class MouseModule : public HListModule
{
    Q_OBJECT
public:
    explicit MouseModule(QObject *parent = nullptr);
    ~MouseModule();
    MouseWorker *work() { return m_worker; }
    MouseModel *model() { return m_model; }

private:
    MouseModel  *m_model;
    MouseWorker *m_worker;
    MouseDBusProxy *m_dbusProxy;
};

class GeneralSettingModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit GeneralSettingModule(MouseModel *model, MouseWorker *worker, QObject *parent = nullptr)
        : ModuleObject(parent), m_model(model), m_worker(worker)  {}
    virtual QWidget *page() override;

private:
    MouseModel  *m_model;
    MouseWorker *m_worker;
};

class TouchPadSettingModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit TouchPadSettingModule(MouseModel *model, MouseWorker *worker, ModuleObject *parent);
    virtual QWidget *page() override;

private:
    MouseModel  *m_model;
    MouseWorker *m_worker;
};

class TrackPointSettingModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit TrackPointSettingModule(MouseModel *model, MouseWorker *worker, ModuleObject *parent);
    virtual QWidget *page() override;

private:
    MouseModel  *m_model;
    MouseWorker *m_worker;
};

class MouseSettingModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit MouseSettingModule(MouseModel *model, MouseWorker *worker, QObject *parent = nullptr)
        : ModuleObject(parent), m_model(model), m_worker(worker) {}
    virtual QWidget *page() override;

private:
    MouseModel  *m_model;
    MouseWorker *m_worker;
};

}
