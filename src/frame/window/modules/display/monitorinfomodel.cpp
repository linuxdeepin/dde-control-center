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

#include "monitorinfomodel.h"
#include "modules/display/displaymodel.h"

using namespace DCC_NAMESPACE::display;
using namespace dcc::display;

MonitorInfoModel::MonitorInfoModel(DisplayModel *model, QObject *parent)
    : QAbstractListModel(parent)
    , m_model(model)
{

}

int MonitorInfoModel::rowCount(const QModelIndex &parent) const
{
    return m_model->monitorList().size();
}

QVariant MonitorInfoModel::data(const QModelIndex &index, int role) const
{
    if (Qt::SizeHintRole == role)
        return QSize(0, 70);
    auto mons = m_model->monitorList();
    switch (Role(role)) {
    case Role::monitorName: return mons[index.row()]->name();
    case Role::width: return mons[index.row()]->currentMode().width();
    case Role::height: return mons[index.row()]->currentMode().height();
    case Role::size: return 1.0;
    case Role::image: return "path";
    default:
        break;;
    }
    return -1;
}
