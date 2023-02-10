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
