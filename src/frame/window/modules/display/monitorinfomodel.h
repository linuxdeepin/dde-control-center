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

#ifndef MONITORINFOMODEL_H
#define MONITORINFOMODEL_H

#include "../../namespace.h"

#include <QAbstractListModel>

namespace dcc {

namespace display {
class DisplayModel;
}
}


namespace DCC_NAMESPACE {

namespace display {

class MonitorInfoModel : public QAbstractListModel
{
    Q_OBJECT
public:
    MonitorInfoModel(dcc::display::DisplayModel *model, QObject *parent = nullptr);

private:
    enum class Role : int {
        monitorName,
        size,
        width,
        height,
        image
    };
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
private:
    QModelIndex m_selectedIndex;

    dcc::display::DisplayModel *m_model;
};

}

}

#endif // MONITORINFOMODEL_H
