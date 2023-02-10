//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef POWERPLUGIN_H
#define POWERPLUGIN_H

#include "interface/hlistmodule.h"
#include "interface/plugininterface.h"

class PowerModel;
class PowerWorker;
class GeneralModule;

class PowerModule : public DCC_NAMESPACE::HListModule
{
    Q_OBJECT
public:
    explicit PowerModule(QObject *parent = nullptr);
    ~PowerModule() override { }
    virtual void active() override;
public Q_SLOTS:
    void onBatteryChanged(const bool &state);
    void onBatteryPercentageChanged(const double value);

private:
    PowerModel *m_model;
    PowerWorker *m_work;
    double m_nBatteryPercentage;
    DCC_NAMESPACE::ModuleObject *m_useElectric;
    DCC_NAMESPACE::ModuleObject *m_useBattery;
};

class PowerPlugin : public DCC_NAMESPACE::PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.deepin.dde.ControlCenter.Power" FILE "plugin-power.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    explicit PowerPlugin(QObject *parent = nullptr);
    ~PowerPlugin();

    virtual QString name() const override;
    virtual DCC_NAMESPACE::ModuleObject *module() override;
    virtual QString location() const override;

private:
    DCC_NAMESPACE::ModuleObject *m_moduleRoot;
};

#endif // POWERPLUGIN_H
