/*
 * Copyright (C) 2021 ~ 2023 Deepin Technology Co., Ltd.
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
#ifndef ADAPTERV20TOV23PLUGIN_H
#define ADAPTERV20TOV23PLUGIN_H

#include "interface/moduleobject.h"
#include "interface/plugininterface.h"
class QEvent;
class QTimer;

class FrameProxyV20;
class PluginManagerV20;
class AdapterV20toV23Module;

class AdapterV20toV23Root : public DCC_NAMESPACE::ModuleObject
{
    Q_OBJECT
public:
    explicit AdapterV20toV23Root();
    ~AdapterV20toV23Root();

    bool loadFinished() const;
    ModuleObject *moduleRoot() const;

public Q_SLOTS:
    void timerTask();
    void pushModule();
    void insertModule(bool append);

private:
    void registerDBus();

private:
    enum Status{
        WaitParent,
        GetPaths,
        LoadPlugin,
        InsertModule,
        InsertAllModule, // 对于找不到位置的插到最后
        LoadFinished,
    };
    ModuleObject *m_root;
    QTimer *m_timer;
    FrameProxyV20 *m_prameProxy;
    PluginManagerV20 *m_pluginManagerV20;
    Status m_status;
    QStringList m_pluginPaths;
    QList<AdapterV20toV23Module *> m_modules;
    int m_tryCount;

};

class AdapterV20toV23Plugin : public DCC_NAMESPACE::PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.deepin.dde.ControlCenter.AdapterV20toV23" FILE "plugin-adapterv20tov23.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    explicit AdapterV20toV23Plugin(QObject *parent = nullptr);
    ~AdapterV20toV23Plugin();

    virtual QString name() const override;
    virtual DCC_NAMESPACE::ModuleObject *module() override;
    virtual int location() const override;

    virtual QList<DCC_NAMESPACE::LayoutFactoryBase *> layoutFactory() override;

private:
    DCC_NAMESPACE::ModuleObject *m_moduleRoot;
};

#endif // ADAPTERV20TOV23PLUGIN_H
