// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractItemModel>
#include <QHash>
#include <QList>

struct DockItemInfo
{
    QString name;
    QString displayName;
    QString itemKey;
    QString settingKey;
    QString dcc_icon;
    bool visible;
};

typedef QList<DockItemInfo> DockItemInfos;

class DockPluginModel : public QAbstractItemModel
{
    Q_OBJECT
    enum DockPluginTypeRole {
        PluginNameRole = Qt::UserRole + 1,
        PlugindisplayNameRole,
        PluginItemKeyRole,
        PluginSettingKeyRole,
        PluginIconKeyRole,
        PluginVisibleRole
    };

public:
    explicit DockPluginModel(QObject *parent = nullptr);
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setPluginVisible(const QString &pluginName, bool visible);
    void resetData(const DockItemInfos &pluginInfos);

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    DockItemInfos m_dockItemInfos;
};
