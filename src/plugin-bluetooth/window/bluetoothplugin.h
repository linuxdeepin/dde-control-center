//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef BLUETOOTHPLUGIN_H
#define BLUETOOTHPLUGIN_H

#include "interface/moduleobject.h"
#include "interface/plugininterface.h"

class BluetoothPlugin : public DCC_NAMESPACE::PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.deepin.dde.ControlCenter.Bluetooth" FILE "plugin-bluetooth.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    explicit BluetoothPlugin(QObject *parent = nullptr);
    ~BluetoothPlugin();

    virtual QString name() const override;
    virtual DCC_NAMESPACE::ModuleObject *module() override;
    virtual QString location() const override;

private:
    DCC_NAMESPACE::ModuleObject *m_moduleRoot;
};

#endif // BLUETOOTHPLUGIN_H
