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
#include "personalizationplugin.h"
#include "personalizationmodel.h"
#include "personalizationworker.h"
#include "personalizationgeneralmodule.h"
#include "personalizationthememodule.h"
#include "personalizationfontsmodule.h"

#include <QLabel>

DCC_USE_NAMESPACE
//DCORE_USE_NAMESPACE
const QString gsetting_showSuspend = "showSuspend";
const QString gsetting_showHiberante = "showHibernate";
const QString gsetting_showShutdown = "showShutdown";

PersonalizationModule::PersonalizationModule(QObject *parent)
    : ModuleObject("Personalization", tr("Personalization"), tr("Personalization"), QIcon::fromTheme("dcc_nav_personalization"), parent)
    , m_model(nullptr)
    , m_nBatteryPercentage(100.0)
    , m_useElectric(nullptr)
    , m_useBattery(nullptr)
{
    setChildType(ModuleObject::ChildType::HList);

    m_model = new PersonalizationModel(this);
    m_work = new PersonalizationWorker(m_model, this);

    appendChild(new PersonalizationGeneralModule(m_model, m_work, this));
//    "personalizationIconTheme", tr("Icon Theme"), QIcon::fromTheme("dcc_Icon_theme"),
    ModuleObject *obj =new PersonalizationThemeModule(m_model->getIconModel(), m_work, this);
    ModuleData * data = obj->moduleData();
    data->Name = "personalizationIconTheme";
    data->Icon = QIcon::fromTheme("dcc_Icon_theme");
    data->DisplayName = tr("Icon Theme");
    obj->setModuleData(data);
    appendChild(obj);
    obj =new PersonalizationThemeModule(m_model->getMouseModel(), m_work, this);
    data = obj->moduleData();
    data->Name = "personalizationCursorTheme";
    data->Icon = QIcon::fromTheme("dcc_cursor_theme");
    data->DisplayName = tr("Cursor Theme");
    obj->setModuleData(data);
    appendChild(obj);
    appendChild(new PersonalizationFontsModule(m_model, m_work, this));
}

void PersonalizationModule::active()
{
    m_work->active(); //refresh data
    m_work->refreshTheme();
//    m_work->refreshFont();
}

void PersonalizationModule::onBatteryChanged(const bool &state)
{
}
//done: 遗留问题，控制中心不应该发电量低通知
void PersonalizationModule::onBatteryPercentageChanged(const double value)
{
}

PersonalizationPlugin::PersonalizationPlugin(QObject *parent)
    : PluginInterface(parent)
    , m_moduleRoot(nullptr)
{
}

PersonalizationPlugin::~PersonalizationPlugin()
{
    m_moduleRoot = nullptr;
}

QString PersonalizationPlugin::name() const
{
    return QStringLiteral("Personalization");
}

ModuleObject *PersonalizationPlugin::module()
{
    if (m_moduleRoot)
        return m_moduleRoot;

    m_moduleRoot = new PersonalizationModule;
    return m_moduleRoot;
}

int PersonalizationPlugin::location() const
{
    return 5;
}
