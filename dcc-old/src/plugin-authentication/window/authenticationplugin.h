//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef AUTHENTICATIONPLUGIN_H
#define AUTHENTICATIONPLUGIN_H

#include "interface/hlistmodule.h"
#include "interface/plugininterface.h"

class CharaMangerWorker;
class CharaMangerModel;
class AuthenticationPlugin : public DCC_NAMESPACE::PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.deepin.dde.ControlCenter.Authentication" FILE "authentication.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)public:
public:
    explicit AuthenticationPlugin() {}

    virtual QString name() const override;
    virtual DCC_NAMESPACE::ModuleObject *module() override;
    virtual QString location() const override;
};

// 一级页面
class AuthenticationModule : public DCC_NAMESPACE::HListModule
{
    Q_OBJECT
public:
    explicit AuthenticationModule(QObject *parent = nullptr);
    ~AuthenticationModule();

    CharaMangerWorker *work() { return m_work; }
    CharaMangerModel *model() { return m_model; }

protected:
    virtual void active() override;

private:
    CharaMangerModel *m_model;
    CharaMangerWorker *m_work;
};

// 指纹页面
class FingerModule : public DCC_NAMESPACE::ModuleObject
{
    Q_OBJECT
public:
    explicit FingerModule(CharaMangerModel *model, CharaMangerWorker *worker, QObject *parent = nullptr)
        : ModuleObject(parent), m_model(model), m_worker(worker)  {}

    virtual QWidget *page() override;

private:
    CharaMangerModel *m_model;
    CharaMangerWorker *m_worker;
};

// 人脸页面
class FaceModule : public DCC_NAMESPACE::ModuleObject
{
    Q_OBJECT
public:
    explicit FaceModule(CharaMangerModel *model, CharaMangerWorker *worker, QObject *parent = nullptr)
        : ModuleObject(parent), m_model(model), m_worker(worker)  {}

    virtual QWidget *page() override;

private:
    CharaMangerModel  *m_model;
    CharaMangerWorker *m_worker;
};

// 虹膜页面
class IrisModule : public DCC_NAMESPACE::ModuleObject
{
    Q_OBJECT
public:
    explicit IrisModule(CharaMangerModel *model, CharaMangerWorker *worker, QObject *parent = nullptr)
        : ModuleObject(parent), m_model(model), m_worker(worker)  {}

    virtual QWidget *page() override;

private:
    CharaMangerModel  *m_model;
    CharaMangerWorker *m_worker;
};


#endif // AUTHENTICATIONPLUGIN_H
