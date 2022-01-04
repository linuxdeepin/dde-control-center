/*
 * Copyright (C) 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <andywang_cm@deepin.com>
 *
 * Maintainer: andywang <andywang_cm@deepin.com>
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

#pragma once

#include "interface/namespace.h"
#include "modules/bluetooth/bluetoothmodel.h"

#include <DListView>
#include <DSpinner>
#include <DIconButton>
#include <DCheckBox>

#include <QWidget>
#include <QPointer>
#include <QTime>
#include <QTimer>

#include <com_deepin_daemon_bluetooth.h>

using BluetoothInter = com::deepin::daemon::Bluetooth;

const int interval = 30;
const int minHeight = 150;
const int maxHeight = 10000;

DWIDGET_USE_NAMESPACE

QT_BEGIN_NAMESPACE
class QLabel;
class QStandardItemModel;
QT_END_NAMESPACE

class TitleLabel;

namespace dcc {
namespace widgets {
class SwitchWidget;
class SettingsGroup;
}

namespace bluetooth {
class Device;
class Adapter;
class DeviceSettingsItem;
}
}

namespace DCC_NAMESPACE {
namespace bluetooth {
class TitleEdit;
class DeviceSettingsItem;
class AdapterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AdapterWidget(const dcc::bluetooth::Adapter *adapter, dcc::bluetooth::BluetoothModel *model);
    virtual ~AdapterWidget();
    void setAdapter(const dcc::bluetooth::Adapter *adapter);
    const dcc::bluetooth::Adapter *adapter() const;
    void loadDetailPage();
    bool getSwitchState();
    void setMyDevicesVisible(bool visible);
    void setOtherDevicesVisible(bool visible);

private:
    void initUI();
    void initConnect();
    void categoryDevice(DeviceSettingsItem *deviceItem, const bool paired);

public Q_SLOTS:
    void toggleSwitch(const bool checked);
    void toggleDiscoverableSwitch(const bool checked);
    void refreshAudioDeviceStatu(const dcc::bluetooth::Device::State &state, bool paired);

Q_SIGNALS:
    void requestSetToggleAdapter(const dcc::bluetooth::Adapter *adapter, const bool &toggled);
    void requestConnectDevice(const dcc::bluetooth::Device *device, const dcc::bluetooth::Adapter *adapter);
    void requestDisconnectDevice(const dcc::bluetooth::Device *device);
    void requestShowDetail(const dcc::bluetooth::Adapter *adapter, const dcc::bluetooth::Device *device);
    void requestSetAlias(const dcc::bluetooth::Adapter *adapter, const QString &alias);
    void notifyLoadFinished();
    void notifyRemoveDevice();
    void requestRefresh(const dcc::bluetooth::Adapter *adapter);
    void requestDiscoverable(const dcc::bluetooth::Adapter *adapter, const bool &discoverable);
    void requestSetDisplaySwitch(const bool &on);

private Q_SLOTS:
    void addDevice(const dcc::bluetooth::Device *device);
    void removeDevice(const QString &deviceId);
    void onPowerStatus(bool bPower, bool bDiscovering);

private:
    QLabel *m_tip;
    TitleEdit *m_titleEdit;
    QLabel *m_hideAnonymousLabel;
    const dcc::bluetooth::Adapter *m_adapter;
    dcc::widgets::SwitchWidget *m_powerSwitch;
    DCheckBox *m_showAnonymousCheckBox;
    QList<QPointer<DeviceSettingsItem>> m_deviceLists;
    QList<QPointer<DeviceSettingsItem>> m_myDevices;
    TitleLabel *m_myDevicesGroup;
    DTK_WIDGET_NAMESPACE::DListView *m_myDeviceListView;
    QStandardItemModel *m_myDeviceModel;
    TitleLabel *m_otherDevicesGroup;
    DTK_WIDGET_NAMESPACE::DSpinner *m_spinner;
    QPointer<DTK_WIDGET_NAMESPACE::DSpinner> m_spinnerBtn;
    DTK_WIDGET_NAMESPACE::DListView *m_otherDeviceListView;
    QStandardItemModel *m_otherDeviceModel;
    DTK_WIDGET_NAMESPACE::DIconButton *m_refreshBtn;
    dcc::bluetooth::BluetoothModel *m_model;
    dcc::widgets::SwitchWidget *m_discoverySwitch;
};
}
}
