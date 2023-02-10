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
#include "datetimeplugin.h"

#include "datetimemodel.h"
#include "datetimeworker.h"
#include "formatsettingmodule.h"
#include "timesettingmodule.h"
#include "timezonemodule.h"

using namespace DCC_NAMESPACE;

DatetimeModule::DatetimeModule(QObject *parent)
    : HListModule("datetime",
                  tr("Date and Time"),
                  tr("Date and Time"),
                  QIcon::fromTheme("dcc_nav_datetime"),
                  parent)
    , m_model(nullptr)
{
    m_model = new DatetimeModel(this);
    m_work = new DatetimeWorker(m_model, this);

    appendChild(new TimeSettingModule(m_model, m_work, this));
    appendChild(new TimezoneModule(m_model, m_work, this));
    appendChild(new FormatSettingModule(m_model, m_work, this));
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
