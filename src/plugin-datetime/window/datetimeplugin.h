//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DATETIMEPLUGIN_H
#define DATETIMEPLUGIN_H

#include "interface/hlistmodule.h"
#include "interface/plugininterface.h"

class DatetimeModel;
class DatetimeWorker;

class DatetimeModule : public DCC_NAMESPACE::HListModule
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
    Q_PLUGIN_METADATA(IID "com.deepin.dde.ControlCenter.Datetime" FILE "plugin-datetime.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    explicit DatetimePlugin(QObject *parent = nullptr);
    ~DatetimePlugin() override;

    virtual QString name() const override;
    virtual DCC_NAMESPACE::ModuleObject *module() override;
    virtual QString location() const override;

private:
    DCC_NAMESPACE::ModuleObject *m_moduleRoot;
};

#endif // DatetimePLUGIN_H
