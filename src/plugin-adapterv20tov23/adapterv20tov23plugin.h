//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

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
    explicit AdapterV20toV23Root(QObject *parent = nullptr);
    ~AdapterV20toV23Root();

    void init();
    bool loadFinished() const;
    ModuleObject *moduleRoot() const;

public Q_SLOTS:
    void timerTask();
    void pushModule();
    void insertModule(bool append);

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
    virtual QString follow() const override;
    virtual DCC_NAMESPACE::ModuleObject *module() override;

};

#endif // ADAPTERV20TOV23PLUGIN_H
