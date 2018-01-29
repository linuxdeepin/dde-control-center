/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
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

#ifndef NAVWIDGET_H
#define NAVWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>

#include <com_deepin_daemon_bluetooth.h>
#include <com_deepin_daemon_inputdevice_wacom.h>

using BluetoothInter = com::deepin::daemon::Bluetooth;
using WacomInter = com::deepin::daemon::inputdevice::Wacom;

class NavWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NavWidget(QWidget *parent = 0);

signals:
    void requestModule(const QString &module) const;

private:
    void onDevicesChanged();
    void refershGridLayout();
    void setTipsText(const QString &text);

    void leaveEvent(QEvent *e);

private:
    QLabel *m_tipsLabel;
    QGridLayout *m_gridLayout;
    QTimer *m_deviceRefreshDelay;

    BluetoothInter *m_bluetoothInter;
    WacomInter *m_wacomInter;

    QStringList m_moduleList;
};

#endif // NAVWIDGET_H
