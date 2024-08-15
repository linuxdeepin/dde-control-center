// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "interface/hlistmodule.h"
#include "interface/namespace.h"
#include "interface/plugininterface.h"
#include "pagemodule.h"

#include <QObject>

namespace DCC_NAMESPACE {
class CommonInfoModel;
class CommonInfoWork;

class CommonInfoPlugin : public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.deepin.dde.ControlCenter.CommonInfo" FILE "CommonInfoPlugin.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    virtual QString name() const override;
    virtual ModuleObject *module() override;
    virtual QString location() const override;
};

class CommonInfoModule : public HListModule
{
    Q_OBJECT
public:
    explicit CommonInfoModule(QObject *parent = nullptr);
    ~CommonInfoModule();
    virtual void active() override;

    CommonInfoWork *worker() { return m_worker; }

    CommonInfoModel *model() { return m_model; }

private:
    CommonInfoWork *m_worker;
    CommonInfoModel *m_model;
};

class DeveloperModeModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit DeveloperModeModule(CommonInfoModel *model,
                                 CommonInfoWork *worker,
                                 QObject *parent = nullptr)
        : ModuleObject(parent)
        , m_model(model)
        , m_worker(worker)
    {
    }

    virtual QWidget *page() override;

private:
    CommonInfoModel *m_model;
    CommonInfoWork *m_worker;
};

class PlyMouthModule : public PageModule
{
    Q_OBJECT
public:
    explicit PlyMouthModule(CommonInfoModel *model,
                            CommonInfoWork *worker,
                            QObject *parent = nullptr);

private slots:
    QWidget *initPlymouthScale(ModuleObject *module);
    QWidget *initPlyMouthDisplay(ModuleObject *module);

private:
    CommonInfoModel *m_model;
    CommonInfoWork *m_work;
};

class UserExperienceProgramModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit UserExperienceProgramModule(CommonInfoModel *model,
                                         CommonInfoWork *worker,
                                         QObject *parent = nullptr)
        : ModuleObject(parent)
        , m_model(model)
        , m_worker(worker)
    {
    }

    virtual QWidget *page() override;

private:
    CommonInfoModel *m_model;
    CommonInfoWork *m_worker;
};

class BootModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit BootModule(CommonInfoModel *model, CommonInfoWork *worker, QObject *parent = nullptr)
        : ModuleObject(parent)
        , m_model(model)
        , m_worker(worker)
    {
    }

    virtual QWidget *page() override;

private:
    CommonInfoModel *m_model;
    CommonInfoWork *m_worker;
};

} // namespace DCC_NAMESPACE
