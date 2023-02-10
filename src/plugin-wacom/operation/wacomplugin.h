//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef WacomPLUGIN_H
#define WacomPLUGIN_H

#include "interface/pagemodule.h"
#include "interface/plugininterface.h"

#include <QWidget>

class WacomModel;

class WacomPlugin : public DCC_NAMESPACE::PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.deepin.dde.ControlCenter.Wacom" FILE "WacomPlugin.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    explicit WacomPlugin();
    ~WacomPlugin() override;

    virtual QString name() const override;
    virtual DCC_NAMESPACE::ModuleObject *module() override;
    virtual QString location() const override;

private:
    DCC_NAMESPACE::ModuleObject *m_moduleRoot;
};


#endif // DatetimePLUGIN_H
