//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "wacommodel.h"
#include "wacomplugin.h"
#include "widgets/widgetmodule.h"

#include "wacommodule.h"

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


