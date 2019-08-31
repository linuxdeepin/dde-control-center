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

#include "window/namespace.h"

#include <DListView>

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QStandardItemModel;
QT_END_NAMESPACE

namespace dcc {
namespace widgets {
class LoadingIndicator;
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
    explicit AdapterWidget(const dcc::bluetooth::Adapter *adapter);

    void setAdapter(const dcc::bluetooth::Adapter *adapter);
    const dcc::bluetooth::Adapter *adapter() const;
    void loadDetailPage();
private:
    void updateHeight();

public Q_SLOTS:
    void toggleSwitch(const bool &checked);

Q_SIGNALS:
    void requestSetToggleAdapter(const dcc::bluetooth::Adapter *adapter, const bool &toggled);
    void requestConnectDevice(const dcc::bluetooth::Device *device);
    void requestShowDetail(const dcc::bluetooth::Adapter *adapter, const dcc::bluetooth::Device *device);
    void requestSetAlias(const dcc::bluetooth::Adapter *adapter, const QString &alias);

private Q_SLOTS:
    void addDevice(const dcc::bluetooth::Device *device);
    void removeDevice(const QString &deviceId);

private:
    const dcc::bluetooth::Adapter *m_adapter;
    TitleEdit *m_titleEdit;
    dcc::widgets::SwitchWidget *m_switch;
    dcc::widgets::SettingsGroup *m_titleGroup;
    QLabel        *m_tip;
    dcc::widgets::LoadingIndicator *m_refreshIndicator;
    QList<DeviceSettingsItem *> m_deviceLists;
    QList<DeviceSettingsItem *> m_myDevices;
    dcc::widgets::SettingsGroup *m_myDevicesGroup;
    DTK_WIDGET_NAMESPACE::DListView *m_myDeviceListView;
    QStandardItemModel *m_myDeviceModel;
    dcc::widgets::SettingsGroup *m_otherDevicesGroup;
    DTK_WIDGET_NAMESPACE::DListView *m_otherDeviceListView;
    QStandardItemModel *m_otherDeviceModel;
};
}
}
