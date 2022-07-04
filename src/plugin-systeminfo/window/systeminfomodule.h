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
#include "interface/hlistmodule.h"
#include "interface/plugininterface.h"

class HostNameItem;

namespace DCC_NAMESPACE {
class SystemInfoModel;
class SystemInfoWork;
class TitleValueItem;
class LogoItem;
class TitleAuthorizedItem;
class SystemInfoPlugin : public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.deepin.dde.ControlCenter.SystemInfo" FILE "SystemInfoPlugin.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    virtual QString name() const override;
    virtual ModuleObject *module() override;
    virtual QString location() const override;
};

class SystemInfoModule : public HListModule
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

protected:
    virtual void active() override;

private:
    void initChildModule();
    void initLogoModule(LogoItem *item);
    void initHostnameModule(HostNameItem *item);
    void initOSNameModule(TitleValueItem *item);
    void initVersionModule(TitleValueItem *item);
    void initEditionModule(TitleValueItem *item);
    void initTypeModule(TitleValueItem *item);
    void initAuthorizationModule(TitleAuthorizedItem *item);
    void initKernelModule(TitleValueItem *item);
    void initProcessorModule(TitleValueItem *item);
    void initMemoryModule(TitleValueItem *item);

private:
    SystemInfoModel *m_model;
    SystemInfoWork *m_work;
};

}
