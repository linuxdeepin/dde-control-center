// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

#include "grubanimationmodel.h"

GrubAnimationModel::GrubAnimationModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

void GrubAnimationModel::initData(const QList<GrubAnimationData> &data)
{
    m_grubAnimationDataList.clear();

    for (GrubAnimationData item : data) {
        m_grubAnimationDataList.append({item.imagePath, item.text, item.checkStatus, item.startAnimation, item.scale, item.plymouthScale});
    }
}

void GrubAnimationModel::addAnimation(const GrubAnimationData &data)
{
    int row = rowCount();
    beginInsertRows(QModelIndex(), row, row);
    m_grubAnimationDataList.append({data.imagePath, data.text, data.checkStatus, data.startAnimation, data.scale, data.plymouthScale});
    endInsertRows();
}

int GrubAnimationModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return m_grubAnimationDataList.count();
}

QVariant GrubAnimationModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_grubAnimationDataList.count())
        return QVariant();

    const GrubAnimationData &grubAnimationData = m_grubAnimationDataList[index.row()];
   // qDebug() << " ================== " << role << grubAnimationData.text << grubAnimationData.checkStatus;

    if (role == Qt::DisplayRole || role == ImagePath) {
        return grubAnimationData.imagePath;
    } else if (role == Text) {
        return grubAnimationData.text;
    } else if (role == CheckStatus) {
        return grubAnimationData.checkStatus;
    } else if (role == StartAnimation) {
        return grubAnimationData.startAnimation;
    } else if (role == Scale) {
        return grubAnimationData.scale;
    } else if (role == PlymouthScale) {
        return grubAnimationData.plymouthScale;
    }

    return QVariant();
}

bool GrubAnimationModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!

    endInsertRows();
    return true;
}

bool GrubAnimationModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!

    endRemoveRows();
    return true;
}

QHash<int, QByteArray> GrubAnimationModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ImagePath] = "imagePath";
    roles[Text] = "text";
    roles[CheckStatus] = "checkStatus";
    roles[StartAnimation] = "startAnimation";
    roles[Scale] = "scale";
    roles[PlymouthScale] = "plymouthScale";
    return roles;
}

void GrubAnimationModel::addModel(const GrubAnimationData &data)
{
    int row = rowCount();
    beginInsertRows(QModelIndex(), row, row);
    m_grubAnimationDataList.append(data);
    endInsertRows();
}

void GrubAnimationModel::updateCheckIndex(int plymouthScale, bool startAnimation)
{
    for (int index = 0; index < m_grubAnimationDataList.count(); index++) {
        m_grubAnimationDataList[index].checkStatus = false;
        m_grubAnimationDataList[index].startAnimation = startAnimation;
        if (plymouthScale == m_grubAnimationDataList[index].plymouthScale) {
            m_grubAnimationDataList[index].checkStatus = true;
        }
        QModelIndex modelIndex = createIndex(index, 0);
        Q_EMIT dataChanged(modelIndex, modelIndex, {CheckStatus, StartAnimation});
    }
}
