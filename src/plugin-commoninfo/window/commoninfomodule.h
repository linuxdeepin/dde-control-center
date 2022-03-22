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
class CommonInfoModel;
class CommonInfoWork;

class CommonInfoPlugin : public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PluginInterface_iid FILE "CommonInfoPlugin.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    virtual QString name() const override;
    virtual ModuleObject *module() override;
};

class CommonInfoModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit CommonInfoModule(QObject *parent = nullptr);
    ~CommonInfoModule();
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
    explicit DeveloperModeModule(CommonInfoModel *model, CommonInfoWork *worker, QObject *parent = nullptr)
        : ModuleObject(parent), m_model(model), m_worker(worker) {}
    virtual QWidget *page() override;
private:
    CommonInfoModel *m_model;
    CommonInfoWork *m_worker;
};

class UserExperienceProgramModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit UserExperienceProgramModule(CommonInfoModel *model, CommonInfoWork *worker, QObject *parent = nullptr)
        : ModuleObject(parent), m_model(model), m_worker(worker) {}
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
        : ModuleObject(parent), m_model(model), m_worker(worker) {}
    virtual QWidget *page() override;
private:
    CommonInfoModel *m_model;
    CommonInfoWork *m_worker;
};
} // namespace DCC_NAMESPACE
