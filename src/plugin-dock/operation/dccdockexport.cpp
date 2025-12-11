// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dccfactory.h"
#include "operation/dockpluginmodel.h"
#include <qtypes.h>

#include "dccdockexport.h"

#include <QFile>
#include <QIcon>
#include <QDir>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusObjectPath>
#include <QDebug>
#include <QDBusArgument>
#include <QDBusMetaType>

#include <DConfig>
#include <DIconTheme>
#include <DWindowManagerHelper>

// 显示模式定义
#define CUSTOM_MODE 0
#define MERGE_MODE 1
#define EXTEND_MODE 2
#define SINGLE_MODE 3

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
, m_dconfig(Dtk::Core::DConfig::create("org.deepin.dde.shell", "org.deepin.ds.dock.taskmanager", QString(), this))
, m_displayInter(nullptr)
, m_displayMode(EXTEND_MODE)
, m_monitorCount(0)
, m_combineApp(true)
{
    if (m_dconfig && m_dconfig->isValid()) {
        const bool noTaskGrouping = m_dconfig->value("noTaskGrouping", false).toBool();
        m_combineApp = !noTaskGrouping;

        connect(m_dconfig, &Dtk::Core::DConfig::valueChanged, this, [this](const QString &key) {
            if (key == QLatin1String("noTaskGrouping")) {
                const bool noTaskGrouping = m_dconfig->value("noTaskGrouping", false).toBool();
                bool combine = !noTaskGrouping;
                if (m_combineApp != combine) {
                    m_combineApp = combine;
                    Q_EMIT combineAppChanged(m_combineApp);
                }
            }
        });
    }

    initData();
    initDisplayModeConnection();

    connect(m_dockDbusProxy, &DockDBusProxy::pluginVisibleChanged, m_pluginModel, &DockPluginModel::setPluginVisible);
    // if it has no blur effect, dcc do not need to show multitask-view plugin
    connect(DWindowManagerHelper::instance(), &DWindowManagerHelper::hasBlurWindowChanged, this, &DccDockExport::initData);
}

DccDockExport::~DccDockExport()
{
    if (m_displayInter) {
        delete m_displayInter;
        m_displayInter = nullptr;
    }
}

void DccDockExport::initData()
{
    auto dciPaths = DIconTheme::dciThemeSearchPaths();
    dciPaths.push_back(PLUGIN_ICON_DIR);
    DIconTheme::setDciThemeSearchPaths(dciPaths);
    loadPluginData();
}

void DccDockExport::loadPluginData()
{
    QDBusPendingReply<DockItemInfos> pluginInfos = m_dockDbusProxy->plugins();
    auto infos = pluginInfos.value();
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

bool DccDockExport::combineApp() const
{
    return m_combineApp;
}

void DccDockExport::initDisplayModeConnection()
{
    // 创建DBus接口连接
    m_displayInter = new QDBusInterface("org.deepin.dde.Display1",
                                       "/org/deepin/dde/Display1",
                                       "org.deepin.dde.Display1",
                                       QDBusConnection::sessionBus(),
                                       this);
    
    if (!m_displayInter->isValid()) {
        qWarning() << "Display DBus interface is not valid!";
        return;
    }

    // 获取当前显示模式
    QVariant displayModeVar = m_displayInter->property("DisplayMode");
    if (displayModeVar.isValid()) {
        uchar mode = displayModeVar.toUInt();
        if (m_displayMode != mode) {
            m_displayMode = mode;
            Q_EMIT displayModeChanged();
        }
    }

    QVariant monitorsVar = m_displayInter->property("Monitors");
    if (monitorsVar.isValid()) {
        QList<QDBusObjectPath> monitors;
        if (monitorsVar.userType() == qMetaTypeId<QDBusArgument>()) {
             const QDBusArgument &arg = monitorsVar.value<QDBusArgument>();
             arg >> monitors;
        } else {
             monitors = qvariant_cast<QList<QDBusObjectPath>>(monitorsVar);
        }
        int count = monitors.count();

        if (m_monitorCount != count) {
            m_monitorCount = count;
            Q_EMIT monitorCountChanged();
        }
    }

    QDBusConnection::sessionBus().connect("org.deepin.dde.Display1",
                                       "/org/deepin/dde/Display1",
                                       "org.freedesktop.DBus.Properties",
                                       "PropertiesChanged",
                                       this,
                                       SLOT(onPropertiesChanged(QString,QMap<QString,QVariant>,QStringList)));
}

int DccDockExport::displayMode() const
{
    return m_displayMode;
}

int DccDockExport::monitorCount() const
{
    return m_monitorCount;
}

void DccDockExport::setCombineApp(bool value)
{
    if (m_combineApp == value)
        return;

    m_combineApp = value;

    if (!m_dconfig || !m_dconfig->isValid())
        return;

    const bool noTaskGrouping = !value;
    m_dconfig->setValue("noTaskGrouping", noTaskGrouping);
    Q_EMIT combineAppChanged(m_combineApp);
}

void DccDockExport::onDisplayModeChanged(uint mode)
{
    int newMode = static_cast<int>(mode);
    if (m_displayMode != newMode) {
        m_displayMode = newMode;
        Q_EMIT displayModeChanged();
    }
}

void DccDockExport::onPropertiesChanged(const QString &/*interfaceName*/, const QMap<QString, QVariant> &changedProperties, const QStringList &/*invalidatedProperties*/)
{
    if (changedProperties.contains("DisplayMode")) {
        uint newMode = changedProperties["DisplayMode"].toUInt();
        onDisplayModeChanged(newMode);
    }
    
    if (changedProperties.contains("Monitors")) {
        QVariant monitorsVar = changedProperties["Monitors"];
        QList<QDBusObjectPath> monitors;
        if (monitorsVar.userType() == qMetaTypeId<QDBusArgument>()) {
             const QDBusArgument &arg = monitorsVar.value<QDBusArgument>();
             arg >> monitors;
        } else {
             monitors = qvariant_cast<QList<QDBusObjectPath>>(monitorsVar);
        }
        int count = monitors.count();

        if (m_monitorCount != count) {
            m_monitorCount = count;
            Q_EMIT monitorCountChanged();
        }
    }
}

DCC_FACTORY_CLASS(DccDockExport)

#include "dccdockexport.moc"
