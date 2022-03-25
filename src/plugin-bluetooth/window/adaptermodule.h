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
DCC_END_NAMESPACE

class BluetoothWorker;
class QCheckBox;

class AdapterModule : public QObject
{
    Q_OBJECT
public:
    explicit AdapterModule(const BluetoothAdapter *adapter, BluetoothModel *model, BluetoothWorker *work, QObject *parent = nullptr);
    virtual ~AdapterModule();
    const QList<DCC_NAMESPACE::ModuleObject *> &ModuleList() const;
    void deactive();

public Q_SLOTS:
    void initBluetoothTitle(DCC_NAMESPACE::SettingsGroup *settingsGrp);
    void initDeviceListView(DTK_WIDGET_NAMESPACE::DListView *deviceListView);
    void initAnonymousCheckBox(QWidget *w);
    void initOtherDeviceListView(DTK_WIDGET_NAMESPACE::DListView *otherDeviceListView);
    void toggleDiscoverableSwitch(const bool checked);

Q_SIGNALS:
    void requestSetToggleAdapter(const BluetoothAdapter *adapter, const bool &toggled);
    void requestConnectDevice(const BluetoothDevice *device, const BluetoothAdapter *adapter);
    void requestDisconnectDevice(const BluetoothDevice *device);
    void requestSetAlias(const BluetoothAdapter *adapter, const QString &alias);
    void requestSetDevAlias(const BluetoothDevice *device, const QString &devAlias);
    void notifyLoadFinished();
    void notifyRemoveDevice();
    void requestRefresh(const BluetoothAdapter *adapter);
    void requestDiscoverable(const BluetoothAdapter *adapter, const bool &discoverable);
    void requestSetDisplaySwitch(const bool &on);
    void requestIgnoreDevice(const BluetoothAdapter *adapter, const BluetoothDevice *device);

private Q_SLOTS:
    void onPowerStatus(bool bPower, bool bDiscovering);

    void contextMenu(const BluetoothAdapter *adapter, const BluetoothDevice *device);

private:
    void setAdapter(const BluetoothAdapter *adapter);
    bool getSwitchState();

protected:
    QList<DCC_NAMESPACE::ModuleObject *> m_moduleList;

    QLabel *m_tip;
    DCC_NAMESPACE::TitleEdit *m_titleEdit;
    const BluetoothAdapter *m_adapter;

    BluetoothModel *m_model;
    BluetoothWorker *m_work;

    DCC_NAMESPACE::SwitchWidget *m_powerSwitch;
    QCheckBox *m_showAnonymousCheckBox;
    DCC_NAMESPACE::TitleLabel *m_myDevicesTitle;
    DTK_WIDGET_NAMESPACE::DListView *m_myDeviceListView;
    DCC_NAMESPACE::TitleLabel *m_otherDevicesTitle;
    DTK_WIDGET_NAMESPACE::DSpinner *m_spinner;
    QPointer<DTK_WIDGET_NAMESPACE::DSpinner> m_spinnerBtn;
    DTK_WIDGET_NAMESPACE::DListView *m_otherDeviceListView;
    DTK_WIDGET_NAMESPACE::DIconButton *m_refreshBtn;
    DCC_NAMESPACE::SwitchWidget *m_discoverySwitch;
};

#endif // ADAPTERMODULE_H
