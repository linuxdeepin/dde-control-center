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

#ifndef ADAPTERWIDGET_H
#define ADAPTERWIDGET_H

#include "widgets/contentwidget.h"
#include "adapter.h"
#include "TitleEdit.h"

namespace dcc {

namespace widgets {
class LoadingIndicator;
class SwitchWidget;
class SettingsGroup;
}

namespace bluetooth {
class DeviceSettingsItem;
class AdapterWidget : public ContentWidget
{
    Q_OBJECT
public:
    explicit AdapterWidget(const Adapter *adapter);

    void setAdapter(const Adapter *adapter);
    const Adapter *adapter() const;

public Q_SLOTS:
    void toggleSwitch(const bool &checked);

Q_SIGNALS:
    void requestSetToggleAdapter(const Adapter *adapter, const bool &toggled);
    void requestConnectDevice(const Device *device);
    void requestShowDetail(const Adapter *adapter, const Device *device);
    void requestSetAlias(const Adapter *adapter, const QString &alias);

private Q_SLOTS:
    void addDevice(const Device *device);
    void removeDevice(const QString &deviceId);

private:
    const Adapter *m_adapter;
    TitleEdit *m_titleEdit;
    widgets::SwitchWidget *m_switch;
    widgets::SettingsGroup *m_titleGroup;
    widgets::SettingsGroup *m_myDevicesGroup;
    widgets::SettingsGroup *m_otherDevicesGroup;
    QLabel        *m_tip;
    widgets::LoadingIndicator *m_refreshIndicator;
    QList<DeviceSettingsItem*> m_deviceLists;
    QList<DeviceSettingsItem*> m_myDevices;
};

}
}

#endif // ADAPTERWIDGET_H
