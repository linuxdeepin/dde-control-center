#include "sidebarmodel.h"

#include <QPixmap>
#include <QVariant>

SidebarModel::SidebarModel(QObject *parent)
    : QAbstractListModel(parent),
      m_pluginManager(PluginsManager::getInstance(this))
{
    loadIcons();

    qRegisterMetaType<ModuleMetaData>("ModuleMetaData");

    connect(m_pluginManager, &PluginsManager::pluginInserted, this, &SidebarModel::loadIcons, Qt::QueuedConnection);
    connect(m_pluginManager, &PluginsManager::pluginRemoved, this, &SidebarModel::loadIcons, Qt::QueuedConnection);
}

void SidebarModel::setSelectedPluginId(const QString &id)
{
    if (!m_selectedPluginId.isEmpty())
    {
        const int count = m_pluginsList.count();
        for (int i(0); i != count; ++i)
        {
            if (id != m_pluginsList.at(i).id)
                continue;

            QModelIndex index = this->index(i);
            emit dataChanged(index, index);
            break;
        }
    }

    if (id == "home" || id == "shutdown")
        return;

    m_selectedPluginId = id;
}

QVariant SidebarModel::data(const QModelIndex &index, int role) const
{
    QVariant v;

    const ModuleMetaData plugin = m_pluginsList.at(index.row());
#ifdef QT_DEBUG
    const QString iconPath = "modules/icons/24/%1_%2.svg";
#else
    const QString iconPath = "../share/dde-control-center/modules/icons/24/%1_%2.svg";
#endif

    if (role == Qt::DisplayRole || role == PluginName)
        return plugin.name;

    if (role == DefaultIcon)
    {
        if (plugin.id == m_selectedPluginId)
            return QPixmap(iconPath.arg(plugin.id).arg("press"));
        else
            return QPixmap(iconPath.arg(plugin.id).arg("normal"));
    }

    if (role == HoverIcon)
    {
        if (plugin.id == m_selectedPluginId)
            return QPixmap(iconPath.arg(plugin.id).arg("press"));
        else
            return QPixmap(iconPath.arg(plugin.id).arg("hover"));
    }

    if (role == Qt::SizeHintRole)
        return QSize(24, 24 + 24);


    if (role == PluginMetaData)
    {
        v.setValue(plugin);
        return v;
    }

    return v;
}

int SidebarModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_pluginsList.count();
}

void SidebarModel::loadIcons()
{
    m_pluginsList = m_pluginManager->pluginsList();

    // meta for home button
    ModuleMetaData home {
        "home",
        "home",
        "Home"
    };
    Q_UNUSED(QT_TRANSLATE_NOOP("ModuleName", "Home"));
    m_pluginsList.insert(0, home);

    // meta for power button
    ModuleMetaData power {
        "power",
        "shutdown",
        "Power"
    };
    Q_UNUSED(QT_TRANSLATE_NOOP("ModuleName", "Power"));
    m_pluginsList.append(power);

    // for test
//    m_pluginsList.append(m_pluginManager->pluginsList());

    // refresh all data
    emit dataChanged(index(0), index(m_pluginsList.count()));
}

