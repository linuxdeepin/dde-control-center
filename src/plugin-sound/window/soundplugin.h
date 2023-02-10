//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef SOUNDPLUGIN_H
#define SOUNDPLUGIN_H

#include "interface/hlistmodule.h"
#include "interface/plugininterface.h"

class SoundModel;
class SoundWorker;
class SoundPlugin : public DCC_NAMESPACE::PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.deepin.dde.ControlCenter.Sound" FILE "sound.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    explicit SoundPlugin() {}

    virtual QString name() const override;
    virtual DCC_NAMESPACE::ModuleObject *module() override;
    virtual QString location() const override;
};

// 一级页面
class SoundModule : public DCC_NAMESPACE::HListModule
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
class OutputModule : public DCC_NAMESPACE::ModuleObject
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

class InputModule : public DCC_NAMESPACE::ModuleObject
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

class SoundEffectsModule : public DCC_NAMESPACE::ModuleObject
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
class InputDeviceModule : public DCC_NAMESPACE::ModuleObject
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
class OutputDeviceModule : public DCC_NAMESPACE::ModuleObject
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
