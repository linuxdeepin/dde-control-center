//SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "layoutsmodel.h"
#include "keyboardwork.h"
#include "keyboardcontroller.h"

namespace dccV25 {

LayoutsModel::LayoutsModel(QObject *parent)
    : QAbstractListModel{ parent }
{
}

int LayoutsModel::rowCount(const QModelIndex &) const
{
    KeyboardWorker *worker = dynamic_cast<KeyboardWorker *>(parent());
    if (!worker)
        return 0;

    const QList<MetaData> &datas = worker->getDatas();

    return datas.size();
}

QVariant LayoutsModel::data(const QModelIndex &index, int role) const
{
    KeyboardWorker *worker = dynamic_cast<KeyboardWorker *>(parent());
    if (!worker)
        return QVariant();

    QList<MetaData> datas = worker->getDatas();

    if (!index.isValid() || index.row() >= datas.size())
        return QVariant();

    const MetaData &data = datas.value(index.row());

    switch (role) {
    case Qt::DisplayRole:
        return data.text();
    case SearchedTextRole:
        return (data.pinyin().at(0) == ' ' ? data.pinyin().mid(1) : data.pinyin()) + data.key() + data.text();
    case IdRole:
        return data.key();
    case SectionRole:
        return data.pinyin().left(1).toUpper();
    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> LayoutsModel::roleNames() const
{
    QHash<int, QByteArray> names = QAbstractListModel::roleNames();
    names[SearchedTextRole] = "searchedText";
    names[IdRole] = "id";
    names[SectionRole] = "section";
    return names;
}

LayoutFilterModel::LayoutFilterModel(QObject *parent)
    :QSortFilterProxyModel(parent)
{

}

bool LayoutFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    KeyboardController *controller = dynamic_cast<KeyboardController *>(parent());
    if (!controller)
        return false;

    QModelIndex modelIndex = this->sourceModel()->index(sourceRow, 0, sourceParent);
    if (!modelIndex.isValid())
        return false;

    bool accepted =  QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
    return accepted && !controller->userLayoutsContains(modelIndex.data(LayoutsModel::IdRole).toString());
}

}
