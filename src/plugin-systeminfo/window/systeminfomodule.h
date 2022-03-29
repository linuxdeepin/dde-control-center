/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
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

QT_BEGIN_NAMESPACE
#include <QObject>
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
class SystemInfoModel;
class SystemInfoWork;
class SystemInfoPlugin : public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PluginInterface_iid FILE "SystemInfoPlugin.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    virtual QString name() const override;
    virtual ModuleObject *module() override;
};

class SystemInfoModule : public ModuleObject
{
    Q_OBJECT
public:
    enum SystemType {
        Default = -1,
        AboutThisPC,
        EditionLicense,
        EndUserLicenseAgreement,
        PrivacyPolicy,
        MaxType
    };

public:
    explicit SystemInfoModule(QObject *parent = nullptr);
    ~SystemInfoModule();
    SystemInfoWork *work() { return m_work; }
    SystemInfoModel *model() { return m_model; }

protected:
    virtual void active() override;

private:
    SystemInfoWork *m_work;
    SystemInfoModel *m_model;
};

class NativeInfoModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit NativeInfoModule(SystemInfoModel *model, SystemInfoWork *work, QObject *parent = nullptr);
    virtual QWidget *page() override;

private:
    SystemInfoModel *m_model;
    SystemInfoWork *m_work;
};

class PrivacyPolicyModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit PrivacyPolicyModule(QObject *parent = nullptr): ModuleObject(parent) {}
    virtual QWidget *page() override;
};

class UserLicenseModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit UserLicenseModule(QObject *parent = nullptr): ModuleObject(parent) {}
    virtual QWidget *page() override;
};

class VersionProtocolModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit VersionProtocolModule(QObject *parent = nullptr): ModuleObject(parent) {}
    virtual QWidget *page() override;
};

}
