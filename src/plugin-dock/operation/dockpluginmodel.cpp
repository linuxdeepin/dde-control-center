//SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "dockpluginmodel.h"

DockPluginModel::DockPluginModel(QObject *parent)
: QAbstractItemModel(parent)
{

}

QModelIndex DockPluginModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0 || row >= rowCount() || parent.isValid() || column != 0)
        return QModelIndex();

    return createIndex(row, 0);
}

QModelIndex DockPluginModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)
    return QModelIndex();
}

int DockPluginModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_dockItemInfos.size();
}

int DockPluginModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QVariant DockPluginModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    
    if (row < 0 || row >= m_dockItemInfos.size()) {
        return {};
    }

    switch (role) {
    case Qt::DisplayRole:
        return m_dockItemInfos[row].displayName;
    case PluginNameRole:
        return m_dockItemInfos[row].name;
    case PlugindisplayNameRole:
        return m_dockItemInfos[row].displayName;
    case PluginItemKeyRole:
        return m_dockItemInfos[row].itemKey;
    case PluginSettingKeyRole:
        return m_dockItemInfos[row].settingKey;
    case PluginIconKeyRole:
        return m_dockItemInfos[row].dcc_icon;
    case PluginVisibleRole:
        return m_dockItemInfos[row].visible;
    }
    return {};
}

QHash<int, QByteArray> DockPluginModel::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractItemModel::roleNames();;
    roles[PluginNameRole] = "name";
    roles[PlugindisplayNameRole] = "displayName";
    roles[PluginItemKeyRole] = "key";
    roles[PluginSettingKeyRole] = "settingKey";
    roles[PluginIconKeyRole] = "icon";
    roles[PluginVisibleRole] = "visible";
    return roles;
}

void DockPluginModel::setPluginVisible(const QString &pluginName, bool visible)
{
    for (int i = 0; i < m_dockItemInfos.size(); ++i) {
        if (pluginName == m_dockItemInfos[i].itemKey) {
            m_dockItemInfos[i].visible = visible;
            emit dataChanged(index(i, 0), index(i, 0));
            return;
        }
    }
}

void DockPluginModel::resetData(const DockItemInfos &pluginInfos)
{
    beginResetModel();
    m_dockItemInfos = pluginInfos;
    endResetModel();
}
