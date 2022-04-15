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

#ifndef AUTHENTICATIONPLUGIN_H
#define AUTHENTICATIONPLUGIN_H

#include "interface/moduleobject.h"
#include "interface/plugininterface.h"

DCC_USE_NAMESPACE

class CharaMangerWorker;
class CharaMangerModel;
class AuthenticationPlugin : public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PluginInterface_iid FILE "authentication.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)public:
public:
    explicit AuthenticationPlugin() {}

    virtual QString name() const override;
    virtual ModuleObject *module() override;
};

// 一级页面
class AuthenticationModule : public ModuleObject
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
class FingerModule : public ModuleObject
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
class FaceModule : public ModuleObject
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
class IrisModule : public ModuleObject
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
