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

#ifndef VPNCONTROLPAGE_H
#define VPNCONTROLPAGE_H

#include "vpnlistmodel.h"

#include <QWidget>

class VpnControlPage : public QWidget
{
    Q_OBJECT

public:
    explicit VpnControlPage(dde::network::NetworkModel *model, QWidget *parent = 0);

signals:
    void mouseLeaveView() const;
    void requestActivateConnection(const QString &devPath, const QString &uuid) const;
    void requestDisconnect(const QString &uuid) const;

private:
    bool eventFilter(QObject *watched, QEvent *event);

private slots:
    void onItemClicked(const QModelIndex &index) const;

private:
    VpnListModel *m_listModel;
};

#endif // VPNCONTROLPAGE_H
