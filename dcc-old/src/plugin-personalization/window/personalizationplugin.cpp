//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "personalizationplugin.h"
#include "personalizationmodel.h"
#include "personalizationworker.h"
#include "personalizationthememodule.h"
#include "personalizationdesktopmodule.h"

#include <QLabel>
#include <DIconTheme>

DGUI_USE_NAMESPACE
using namespace DCC_NAMESPACE;

const QString gsetting_showSuspend = "showSuspend";
const QString gsetting_showHiberante = "showHibernate";
const QString gsetting_showShutdown = "showShutdown";

PersonalizationModule::PersonalizationModule(QObject *parent)
    : HListModule("personalization", tr("Personalization"), DIconTheme::findQIcon("dcc_nav_personalization"), parent)
    , m_model(nullptr)
    , m_nBatteryPercentage(100.0)
    , m_useElectric(nullptr)
    , m_useBattery(nullptr)
{
    m_model = new PersonalizationModel(this);
    m_work = new PersonalizationWorker(m_model, this);

    appendChild(new PersonalizationThemeModule(m_model, m_work, this));
    appendChild(new PersonalizationDesktopModule(m_model, m_work, this));
}

void PersonalizationModule::active()
{
    m_work->active(); // refresh data
    m_work->refreshTheme();
}

void PersonalizationModule::onBatteryChanged(const bool &state)
{
    Q_UNUSED(state)
}
// done: 遗留问题，控制中心不应该发电量低通知
void PersonalizationModule::onBatteryPercentageChanged(const double value)
{
    Q_UNUSED(value)
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

QString PersonalizationPlugin::location() const
{
    return "5";
}
