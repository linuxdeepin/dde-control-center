/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     lq <longqi_cm@deepin.com>
 *
 * Maintainer: lq <longqi_cm@deepin.com>
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

#ifndef DISPLAYCONTROLMODEL_H_V20
#define DISPLAYCONTROLMODEL_H_V20

#include "window/namespace.h"

#include <QAbstractListModel>
#include <DStyleOption>

namespace dcc {

namespace display {
class DisplayModel;
}

}

namespace DCC_NAMESPACE {

class DisplayControlModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit DisplayControlModel(dcc::display::DisplayModel *model, QObject *parent = 0);

    enum DisplayRole {
        ItemTitleRole = Qt::DisplayRole,
        UnusedRole = Dtk::UserRole,
        ItemDescriptionRole,
        ItemTypeRole,
        ItemNameRole,
        ItemIsLastRole,
        ItemSelectedRole,
        ItemIconRole,
        ItemConfigNameRole,
    };

    enum ItemType {
        Duplicate,
        Extend,
        Specified,
        Custom,
        NewConfig,
    };

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

private:
    ItemType optionType(const int index) const;
    const QPixmap optionIcon(const int index) const;
    const QString optionName(const int index) const;
    const QString optionDescription(const int index) const;

private Q_SLOTS:
    void onDisplayModeChanged(const int mode);

private:
    QModelIndex m_selectedIndex;

    dcc::display::DisplayModel *m_displayModel;
};

}
Q_DECLARE_METATYPE(DCC_NAMESPACE::DisplayControlModel::ItemType)

#endif // DISPLAYCONTROLMODEL_H_V20
