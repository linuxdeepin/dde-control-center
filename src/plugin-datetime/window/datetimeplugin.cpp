// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "datetimeplugin.h"
#include "datetimemodel.h"
#include "datetimeworker.h"
#include "regionmodule.h"
#include "timesettingmodule.h"
#include "timezonemodule.h"

#include <DIconTheme>
DGUI_USE_NAMESPACE
using namespace DCC_NAMESPACE;

DatetimeModule::DatetimeModule(QObject *parent)
    : HListModule("datetime", tr("Time and Format"), DIconTheme::findQIcon("dcc_nav_datetime"), parent)
    , m_model(nullptr)
{
    m_model = new DatetimeModel(this);
    m_work = new DatetimeWorker(m_model, this);

    appendChild(new TimeSettingModule(m_model, m_work, this));
    appendChild(new TimezoneModule(m_model, m_work, this));
    appendChild(new RegionModule(m_model, m_work, this));
}

void DatetimeModule::active()
{
    m_work->activate();
}

DatetimePlugin::DatetimePlugin(QObject *parent)
    : PluginInterface(parent)
    , m_moduleRoot(nullptr)
{
}

DatetimePlugin::~DatetimePlugin()
{
    m_moduleRoot = nullptr;
}

QString DatetimePlugin::name() const
{
    return QStringLiteral("datetime");
}

ModuleObject *DatetimePlugin::module()
{
    if (m_moduleRoot)
        return m_moduleRoot;

    m_moduleRoot = new DatetimeModule;
    return m_moduleRoot;
}

QString DatetimePlugin::location() const
{
    return "12";
}
