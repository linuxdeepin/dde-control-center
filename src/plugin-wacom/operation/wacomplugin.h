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

#ifndef WacomPLUGIN_H
#define WacomPLUGIN_H

#include "interface/pagemodule.h"
#include "interface/plugininterface.h"

#include <QWidget>

DCC_USE_NAMESPACE

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
    virtual ModuleObject *module() override;
    virtual QString location() const override;

private:
    DCC_NAMESPACE::ModuleObject *m_moduleRoot;
};


#endif // DatetimePLUGIN_H
