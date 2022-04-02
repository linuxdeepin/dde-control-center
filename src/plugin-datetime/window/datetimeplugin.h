/*
* Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
*
* Author:     caixiangrong <caixiangrong@uniontech.com>
*
* Maintainer: caixiangrong <caixiangrong@uniontech.com>
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
#ifndef DATETIMEPLUGIN_H
#define DATETIMEPLUGIN_H

#include "interface/moduleobject.h"
#include "interface/plugininterface.h"

class DatetimeModel;
class DatetimeWorker;

class DatetimeModule : public DCC_NAMESPACE::ModuleObject
{
    Q_OBJECT
public:
    explicit DatetimeModule(QObject *parent = nullptr);
    ~DatetimeModule() override {}
    virtual void active() override;

private:
    DatetimeModel *m_model;
    DatetimeWorker *m_work;
    DCC_NAMESPACE::ModuleObject *m_timeSettings;
    DCC_NAMESPACE::ModuleObject *m_timezoneList;
    DCC_NAMESPACE::ModuleObject *m_timeFormat;
};

class DatetimePlugin : public DCC_NAMESPACE::PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PluginInterface_iid FILE "plugin-datetime.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    explicit DatetimePlugin(QObject *parent = nullptr);
    ~DatetimePlugin() override;

    virtual QString name() const override;
    virtual DCC_NAMESPACE::ModuleObject *module() override;

private:
    DCC_NAMESPACE::ModuleObject *m_moduleRoot;
};

#endif // DatetimePLUGIN_H
