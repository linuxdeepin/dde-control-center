//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PERSONALIZATIONPLUGIN_H
#define PERSONALIZATIONPLUGIN_H

#include "interface/hlistmodule.h"
#include "interface/plugininterface.h"

class PersonalizationModel;
class PersonalizationWorker;
class GeneralModule;

class PersonalizationModule : public DCC_NAMESPACE::HListModule
{
    Q_OBJECT
public:
    explicit PersonalizationModule(QObject *parent = nullptr);
    ~PersonalizationModule() override { }
    virtual void active() override;
public Q_SLOTS:
    void onBatteryChanged(const bool &state);
    void onBatteryPercentageChanged(const double value);

private:
    PersonalizationModel *m_model;
    PersonalizationWorker *m_work;
    double m_nBatteryPercentage;
    DCC_NAMESPACE::ModuleObject *m_useElectric;
    DCC_NAMESPACE::ModuleObject *m_useBattery;
};

class PersonalizationPlugin : public DCC_NAMESPACE::PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.deepin.dde.ControlCenter.Personalization" FILE "plugin-personalization.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    explicit PersonalizationPlugin(QObject *parent = nullptr);
    ~PersonalizationPlugin();

    virtual QString name() const override;
    virtual DCC_NAMESPACE::ModuleObject *module() override;
    virtual QString location() const override;

private:
    DCC_NAMESPACE::ModuleObject *m_moduleRoot;
};

#endif // PERSONALIZATIONPLUGIN_H
