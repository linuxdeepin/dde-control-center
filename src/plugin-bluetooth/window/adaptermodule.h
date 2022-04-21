/*
* Copyright (C) 2021 ~ 2023 Deepin Technology Co., Ltd.
*
* Author:     caixiangrong <caixiangrong@uniontech.com>
*
* Maintainer: caixiangrong <caixiangrong@uniontech.com>
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
#ifndef ADAPTERMODULE_H
#define ADAPTERMODULE_H

#include "interface/moduleobject.h"

#include "widgets/switchwidget.h"
#include "widgets/titlelabel.h"
#include "bluetoothmodel.h"
#include "titleedit.h"

#include <QObject>

DWIDGET_BEGIN_NAMESPACE
class DListView;
class DSpinner;
class DIconButton;
DWIDGET_END_NAMESPACE

DCC_BEGIN_NAMESPACE
class SettingsGroup;
class DCCListView;
DCC_END_NAMESPACE

class BluetoothWorker;
class QCheckBox;

class AdapterModule : public QObject
{
    Q_OBJECT
public:
    explicit AdapterModule(const BluetoothAdapter *adapter, BluetoothModel *model, BluetoothWorker *work, QObject *parent = nullptr);
    virtual ~AdapterModule();
    const QList<QPair<DCC_NAMESPACE::ModuleObject *, bool>> &ModuleList() const;

public Q_SLOTS:
    void initBluetoothTitle(DCC_NAMESPACE::SettingsGroup *settingsGrp);
    void initDeviceListView(DCC_NAMESPACE::DCCListView *deviceListView);
    void initAnonymousCheckBox(QWidget *w);
    void initOtherDeviceListView(DCC_NAMESPACE::DCCListView *otherDeviceListView);
    void toggleDiscoverableSwitch(const bool checked);
    void updateVisible(bool bPower, bool bDiscovering);

Q_SIGNALS:
    void requestSetToggleAdapter(const BluetoothAdapter *adapter, const bool &toggled);
    void requestConnectDevice(const BluetoothDevice *device, const BluetoothAdapter *adapter);
    void requestDisconnectDevice(const BluetoothDevice *device);
    void requestSetAlias(const BluetoothAdapter *adapter, const QString &alias);
    void requestSetDevAlias(const BluetoothDevice *device, const QString &devAlias);
    void notifyRemoveDevice();
    void requestRefresh(const BluetoothAdapter *adapter);
    void requestDiscoverable(const BluetoothAdapter *adapter, const bool &discoverable);
    void requestSetDisplaySwitch(const bool &on);
    void requestIgnoreDevice(const BluetoothAdapter *adapter, const BluetoothDevice *device);
    void visibleChanged();

private Q_SLOTS:
    void contextMenu(const BluetoothAdapter *adapter, const BluetoothDevice *device, DCC_NAMESPACE::DCCListView *view);
    void deviceChanged();

private:
    void setAdapter(const BluetoothAdapter *adapter);
    bool getSwitchState();

protected:
    QList<QPair<DCC_NAMESPACE::ModuleObject *, bool>> m_moduleList;
    QSet<const BluetoothDevice *> m_devices;

    DCC_NAMESPACE::TitleEdit *m_titleEdit;
    const BluetoothAdapter *m_adapter;
    bool m_hasPaired;

    BluetoothModel *m_model;
    BluetoothWorker *m_work;
};

#endif // ADAPTERMODULE_H
