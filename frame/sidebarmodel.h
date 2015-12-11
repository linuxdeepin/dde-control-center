#ifndef SIDEBARMODEL_H
#define SIDEBARMODEL_H

#include "pluginsmanager.h"

#include <QAbstractListModel>

class SidebarModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ItemDataRole {
        PluginMetaData = Qt::UserRole + 1,
        PluginName,
        DefaultIcon,
        HoverIcon,
    };

public:
    SidebarModel(QObject *parent = nullptr);

    void setSelectedPluginId(const QString &id);

    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;

signals:
    // if selected plugin removed, need to notify view to switch another
    void switchToModel(const ModuleMetaData &meta) const;

private:
    void loadIcons();

private:
    PluginsManager *m_pluginManager;

    QString m_selectedPluginId;
    QList<ModuleMetaData> m_pluginsList;
};

#endif // SIDEBARMODEL_H
