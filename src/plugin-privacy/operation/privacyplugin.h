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
#ifndef PRIVACYPLUGIN_H
#define PRIVACYPLUGIN_H

#include "interface/vlistmodule.h"
#include "interface/plugininterface.h"

#include <QWidget>

DCC_USE_NAMESPACE

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
    virtual ModuleObject *module() override;
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
