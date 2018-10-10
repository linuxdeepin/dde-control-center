/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     listenerri <listenerri@gmail.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
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

#ifndef NAVMODEL_H
#define NAVMODEL_H

#include <QAbstractTableModel>
#include <com_deepin_daemon_bluetooth.h>
#include <com_deepin_daemon_inputdevice_wacom.h>

using BluetoothInter = com::deepin::daemon::Bluetooth;
using WacomInter = com::deepin::daemon::inputdevice::Wacom;

const QStringList MODULES = { "accounts", "display", "defapp", "personalization", "network",
                              "bluetooth", "sound", "datetime", "power", "mouse", "keyboard",
                              "wacom", "update", "systeminfo" };

class NavModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum NavDataRole{
        NavHoverRole = Qt::UserRole + 1,
        NavDisplayRole
    };

    NavModel(QObject *parent = Q_NULLPTR);

public:
    int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;

public Q_SLOTS:
    void removeModule(const QModelIndex &index);
    void removeModule(const QString &moduleName);
    void addModule(const QString &moduleName);
    void setHoverIndex(const QModelIndex &index);

public:
    QString transModuleName(const QString &moduleName) const;

private Q_SLOTS:
    void onBTValidChanged(const bool valid);
    void onWacomExistChanged(bool value);

private:
    QStringList validModuleList();

private:
    BluetoothInter *m_bluetoothInter;
    WacomInter *m_wacomInter;

    QStringList m_moduleList;
    QModelIndex m_hoverIndex;
};

#endif // NAVMODEL_H
