//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "bluetoothplugin.h"
#include "bluetoothmodule.h"

using namespace DCC_NAMESPACE;

BluetoothPlugin::BluetoothPlugin(QObject *parent)
    : PluginInterface(parent)
    , m_moduleRoot(nullptr)
{
}

BluetoothPlugin::~BluetoothPlugin()
{
    m_moduleRoot = nullptr;
}

QString BluetoothPlugin::name() const
{
    return QStringLiteral("bluetooth");
}

ModuleObject *BluetoothPlugin::module()
{
    if (m_moduleRoot)
        return m_moduleRoot;

    m_moduleRoot = new BluetoothModule;
    return m_moduleRoot;
}

QString BluetoothPlugin::location() const
{
    return "8";
}
