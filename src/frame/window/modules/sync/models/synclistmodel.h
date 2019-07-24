/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     justforlxz <justforlxz@outlook.com>
 *
 * Maintainer: justforlxz <justforlxz@outlook.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SYNCLISTMODEL_H
#define SYNCLISTMODEL_H

#include "window/namespace.h"

#include <QAbstractListModel>

namespace dcc {
namespace cloudsync {
class SyncModel;
}
}

namespace DCC_NAMESPACE {
namespace sync {
class SyncListModel : public QAbstractListModel {
    Q_OBJECT
public:
    SyncListModel(QObject *parent = nullptr);
    void setModel(dcc::cloudsync::SyncModel* model);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    const QString translation(int index) const;

private:
    dcc::cloudsync::SyncModel* m_model;
};
}
}

#endif // !SYNCLISTMODEL_H
