//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef PRIVACYPLUGIN_H
#define PRIVACYPLUGIN_H

#include "interface/vlistmodule.h"
#include "interface/plugininterface.h"

#include <QWidget>

class PrivacySecurityWorker;
class PrivacySecurityModel;

class PrivacyPlugin : public DCC_NAMESPACE::PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.deepin.dde.ControlCenter.Privacy" FILE "PrivacyPlugin.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    explicit PrivacyPlugin();
    ~PrivacyPlugin() override;

    virtual QString name() const override;
    virtual DCC_NAMESPACE::ModuleObject *module() override;
    virtual QString location() const override;

private:
    DCC_NAMESPACE::ModuleObject *m_moduleRoot;
};

// 一级
class PrivacyModule : public DCC_NAMESPACE::VListModule
{
    Q_OBJECT
public:
    explicit PrivacyModule(QObject *parent = nullptr);
    ~PrivacyModule() override {}
    virtual void active() override;

    PrivacySecurityWorker *work() { return m_work; }
    PrivacySecurityModel *model() { return m_model; }

private:
    PrivacySecurityModel *m_model;
    PrivacySecurityWorker *m_work;
};

#endif // DatetimePLUGIN_H
