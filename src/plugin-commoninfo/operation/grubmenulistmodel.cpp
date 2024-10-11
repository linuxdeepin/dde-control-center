// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

#include "grubmenulistmodel.h"

GrubMenuListModel::GrubMenuListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

void GrubMenuListModel::initData(const QList<GrubMenuData> &data)
{
    m_grubMenuDataList.clear();
    for (GrubMenuData item : data) {
        m_grubMenuDataList.append({item.text, item.checkStatus});
    }
}

void GrubMenuListModel::addGrubMenu(const GrubMenuData &data)
{
    int row = rowCount();
    beginInsertRows(QModelIndex(), row, row);
    m_grubMenuDataList.append({data.text, data.checkStatus});
    endInsertRows();
}

int GrubMenuListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return m_grubMenuDataList.count();
}

QVariant GrubMenuListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const GrubMenuData &grubMenuData = m_grubMenuDataList[index.row()];

    if (role == Qt::DisplayRole || role == Text) {
        return grubMenuData.text;
    } else if (role == CheckStatus) {
        return grubMenuData.checkStatus;
    }

    return QVariant();
}

QHash<int, QByteArray> GrubMenuListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Text] = "text";
    roles[CheckStatus] = "checkStatus";
    return roles;
}

void GrubMenuListModel::updateCheckIndex(const QString &text)
{
    for (int index = 0; index < m_grubMenuDataList.count(); index++) {
        m_grubMenuDataList[index].checkStatus = false;
        if (QString::compare(text, m_grubMenuDataList[index].text, Qt::CaseSensitive) == 0) {
            m_grubMenuDataList[index].checkStatus = true;
        }
        QModelIndex modelIndex = createIndex(index, 0);
        Q_EMIT dataChanged(modelIndex, modelIndex, {CheckStatus});
    }
}
