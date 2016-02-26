/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

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
