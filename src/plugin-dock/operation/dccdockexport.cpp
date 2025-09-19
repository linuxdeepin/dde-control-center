// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dccfactory.h"
#include "operation/dockpluginmodel.h"

#include "dccdockexport.h"
#include <QFile>
#include <QIcon>
#include <QDir>

#include <DIconTheme>
#include <DWindowManagerHelper>

constexpr auto PLUGIN_ICON_DIR = "/usr/share/dde-dock/icons/dcc-setting";
constexpr auto PLUGIN_ICON_PREFIX = "dcc-";
constexpr auto PLUGIN_ICON_DEFAULT = "dcc_dock_plug_in";

static const QMap<QString, QString> pluginIconMap = {
    {"AiAssistant",    "dcc_dock_assistant"}
    , {"show-desktop",   "dcc_dock_desktop"}
    , {"onboard",        "dcc_dock_keyboard"}
    , {"notifications",  "dcc_dock_notify"}
    , {"shutdown",       "dcc_dock_power"}
    , {"multitasking",   "dcc_dock_task"}
    , {"datetime",       "dcc_dock_time"}
    , {"system-monitor", "dcc_dock_systemmonitor"}
    , {"grand-search",   "dcc_dock_grandsearch"}
    , {"trash",          "dcc_dock_trash"}
    , {"shot-start-plugin",  "dcc_dock_shot_start_plugin"}
};

DGUI_USE_NAMESPACE;

DccDockExport::DccDockExport(QObject *parent) 
: QObject(parent)
, m_dockDbusProxy(new DockDBusProxy(this))
, m_pluginModel(new DockPluginModel(this))
{
    initData();

    connect(m_dockDbusProxy, &DockDBusProxy::pluginVisibleChanged, m_pluginModel, &DockPluginModel::setPluginVisible);
    // if it has no blur effect, dcc do not need to show multitask-view plugin
    connect(DWindowManagerHelper::instance(), &DWindowManagerHelper::hasBlurWindowChanged, this, &DccDockExport::initData);
}

void DccDockExport::initData()
{
    QDBusPendingReply<DockItemInfos> pluginInfos = m_dockDbusProxy->plugins();
    auto infos = pluginInfos.value();

    auto dciPaths = DIconTheme::dciThemeSearchPaths();
    dciPaths.push_back(PLUGIN_ICON_DIR);
    DIconTheme::setDciThemeSearchPaths(dciPaths);

    for (auto &info : infos) {
        QString pluginIconStr{};
        if (QFile::exists(QString(PLUGIN_ICON_DIR) + QDir::separator() + PLUGIN_ICON_PREFIX + info.name + ".dci")) {
            pluginIconStr = PLUGIN_ICON_PREFIX + info.name;
        } else if (QFile::exists(QString(PLUGIN_ICON_DIR) + QDir::separator() + info.name + ".dci")) {
            pluginIconStr =  info.name;
        } else if (QFile::exists(info.dcc_icon)) {
            pluginIconStr = info.dcc_icon;
        } else if (pluginIconMap.contains(info.itemKey)) {
            pluginIconStr = pluginIconMap.value(info.itemKey);
        }

        QIcon tmpIcon = QIcon::fromTheme(pluginIconStr);
        if (tmpIcon.isNull()) {
            pluginIconStr = PLUGIN_ICON_DEFAULT;
        }

        info.dcc_icon = pluginIconStr;
    }
    m_pluginModel->resetData(infos);
}

DCC_FACTORY_CLASS(DccDockExport)

#include "dccdockexport.moc"
