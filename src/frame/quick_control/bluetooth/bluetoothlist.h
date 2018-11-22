/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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

#ifndef BLUETOOTHLIST_H
#define BLUETOOTHLIST_H

#include "bluetooth/bluetoothmodel.h"
#include "bluetoothlistmodel.h"
#include "widgets/basiclistview.h"
#include <QWidget>

using dcc::bluetooth::BluetoothModel;
using dcc::widgets::BasicListView;

class BluetoothList : public QWidget
{
    Q_OBJECT
public:
    explicit BluetoothList(BluetoothModel *model, QWidget *parent = 0);

signals:
    void mouseLeaveView() const;
    void requestConnect(const Device *device) const;
    void requestDisConnect(const Device *device) const;
    void requestDetailPage(const QString &module, const QString &page, bool animation) const;
    void requestAdapterDiscoverable(const QString &id) const;

private:
    bool eventFilter(QObject *watched, QEvent *event);

private slots:
    void onItemClicked(const QModelIndex &index) const;

private:
    BluetoothListModel *m_model;
    BluetoothModel     *m_btModel;
};

#endif // BLUETOOTHLIST_H
