/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#ifndef WIFIPAGE_H
#define WIFIPAGE_H

#include "wifipage.h"

#include <QWidget>

namespace dde {
namespace network {
class NetworkModel;
}
}

class WifiPage : public QWidget
{
    Q_OBJECT

public:
    explicit WifiPage(dde::network::NetworkModel *model, QWidget *parent = 0);

signals:
    void mouseLeaveView() const;
    void requestPage(const QString &module, const QString &page, bool animation) const;
    void requestDeviceApList(const QString &devPath) const;
    void requestDeactivateConnection(const QString &uuid) const;
    void requestActivateAccessPoint(const QString &devPath, const QString &apPath, const QString &uuid) const;

private:
    bool eventFilter(QObject *watched, QEvent *event);

private slots:
    void onItemClicked(const QModelIndex &index);

private:
    dde::network::NetworkModel *m_networkModel;
};

#endif // WIFIPAGE_H
