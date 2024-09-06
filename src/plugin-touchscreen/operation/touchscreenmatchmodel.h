// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef TOUCHSCREENMATCHMODEL_H
#define TOUCHSCREENMATCHMODEL_H

#include "touchscreenmodel.h"

#include <QAbstractItemModel>

class TouchScreenModel;

enum TouchScreenMatchRole
{
    IdRole = Qt::UserRole + 1,
    NameRole,
    DeviceNodeRole,
    SerialNumberRole,
    UUIDRole,
    ScreenNameRole,
};
class TouchScreenMatchModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit TouchScreenMatchModel(TouchScreenModel *parent);

    // Basic functionality:
    QHash<int, QByteArray> roleNames() const override;
    QModelIndex index(int row, int column, const QModelIndex &parentIndex = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    void init();
    void resetItems();

    QList<QPair<TouchscreenInfo_V2, QString>> m_touchScreenList;
    TouchScreenModel *m_touchScreenModel;
};
#endif // TOUCHSCREENMATCHMODEL_H
