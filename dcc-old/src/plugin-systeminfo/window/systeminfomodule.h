//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"
#include "interface/hlistmodule.h"
#include "interface/plugininterface.h"
#include "window/userlicensewidget.h"
#include "window/versionprotocolwidget.h"

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
    void initGraphicsPlatformModule(TitleValueItem *item);
    void initGnuLicenseModule(VersionProtocolWidget *item);
    void initUserLicenseModule(UserLicenseWidget *item);

private:
    SystemInfoModel *m_model;
    SystemInfoWork *m_work;
};

}
