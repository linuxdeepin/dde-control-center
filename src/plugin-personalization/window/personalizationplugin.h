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
    Q_PLUGIN_METADATA(IID "com.deepin.dde.ControlCenter.Personalization" FILE
                          "plugin-personalization.json")
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
