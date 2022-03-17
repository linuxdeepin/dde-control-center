/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
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

#pragma once

#include "interface/namespace.h"
#include "interface/moduleobject.h"
#include "interface/plugininterface.h"

#include <QObject>

namespace DCC_NAMESPACE {
class MouseWidget;
class GeneralSettingWidget;
class MouseSettingWidget;
class TouchPadSettingWidget;
class TrackPointSettingWidget;

class MouseModel;
class MouseWorker;
class MouseDBusProxy;

class MousePlugin : public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PluginInterface_iid FILE "MousePlugin.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    virtual QString name() const override;
    virtual ModuleObject *module() override;
};

//一级菜单
class MouseModule : public ModuleObject
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
    explicit TouchPadSettingModule(MouseModel *model, MouseWorker *worker, QObject *parent = nullptr)
        : ModuleObject(parent), m_model(model), m_worker(worker)  {}
    virtual QWidget *page() override;

private:
    MouseModel  *m_model;
    MouseWorker *m_worker;
};

class TrackPointSettingModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit TrackPointSettingModule(MouseModel *model, MouseWorker *worker, QObject *parent = nullptr)
        : ModuleObject(parent), m_model(model), m_worker(worker)  {}
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
