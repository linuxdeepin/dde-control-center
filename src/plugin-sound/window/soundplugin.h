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

#ifndef SOUNDPLUGIN_H
#define SOUNDPLUGIN_H

#include "interface/moduleobject.h"
#include "interface/plugininterface.h"

DCC_USE_NAMESPACE

class SoundModel;
class SoundWorker;
class SoundPlugin : public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PluginInterface_iid FILE "sound.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    explicit SoundPlugin() {}

    virtual QString name() const override;
    virtual ModuleObject *module() override;
};

// 一级页面
class SoundModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit SoundModule(QObject *parent = nullptr);
    ~SoundModule();

    SoundWorker *work() { return m_work; }
    SoundModel *model() { return m_model; }

protected:
    virtual void active() override;

private:
    SoundModel *m_model;
    SoundWorker *m_work;
};

// 三级详情页
class OutputModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit OutputModule(SoundModel *model, SoundWorker *worker, QObject *parent = nullptr)
        : ModuleObject(parent), m_model(model), m_worker(worker)  {}

    virtual QWidget *page() override;

private:
    SoundModel  *m_model;
    SoundWorker *m_worker;
};

class InputModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit InputModule(SoundModel *model, SoundWorker *worker, QObject *parent = nullptr)
        : ModuleObject(parent), m_model(model), m_worker(worker)  {}

    virtual QWidget *page() override;

private:
    SoundModel  *m_model;
    SoundWorker *m_worker;
};

class SoundEffectsModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit SoundEffectsModule(SoundModel *model, SoundWorker *worker, QObject *parent = nullptr)
        : ModuleObject(parent), m_model(model), m_worker(worker)  {}

    virtual QWidget *page() override;

protected:
    virtual void active() override;

private:
    SoundModel  *m_model;
    SoundWorker *m_worker;
};

// 对应标题
class DeviceTitleModule : public DCC_NAMESPACE::ModuleObject
{
    Q_OBJECT
public:
    DeviceTitleModule(const QString &name, const QString &title, QObject *parent = nullptr);
    virtual QWidget *page() override;
};


// 输入设备
class InputDeviceModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit InputDeviceModule(SoundModel *model, SoundWorker *worker, QObject *parent = nullptr)
        : ModuleObject(parent), m_model(model), m_worker(worker)  {}

    virtual QWidget *page() override;

private:
    SoundModel  *m_model;
    SoundWorker *m_worker;
};

// 输出设备
class OutputDeviceModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit OutputDeviceModule(SoundModel *model, SoundWorker *worker, QObject *parent = nullptr)
        : ModuleObject(parent), m_model(model), m_worker(worker)  {}

    virtual QWidget *page() override;

private:
    SoundModel  *m_model;
    SoundWorker *m_worker;
};

#endif // SOUNDPLUGIN_H
