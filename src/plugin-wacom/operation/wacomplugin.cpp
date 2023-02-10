/*
 * Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>
 *
 * Maintainer: duanhongyu <duanhongyu@uniontech.com>
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

#include "wacomplugin.h"

#include "wacommodel.h"
#include "widgets/widgetmodule.h"

#include <wacommodule.h>

/*数位板*/

using namespace DCC_NAMESPACE;

WacomPlugin::WacomPlugin()
    : PluginInterface()
    , m_moduleRoot(nullptr)
{
}

WacomPlugin::~WacomPlugin()
{
    m_moduleRoot = nullptr;
}

QString WacomPlugin::name() const
{
    return QStringLiteral("wacom");
}

ModuleObject *WacomPlugin::module()
{
    if (m_moduleRoot)
        return m_moduleRoot;

    m_moduleRoot = new WacomModule;
    return m_moduleRoot;
}

QString WacomPlugin::location() const
{
    return "14";
}
