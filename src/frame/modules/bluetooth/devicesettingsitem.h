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

#ifndef DCC_BLUETOOTH_DEVICESETTINGSITEM_H
#define DCC_BLUETOOTH_DEVICESETTINGSITEM_H

#include "widgets/settingsitem.h"
#include "device.h"

namespace dcc {
namespace widgets {
class NormalLabel;
class NextButton;
class LoadingIndicator;
}

namespace bluetooth {

class DeviceSettingsItem : public dcc::widgets::SettingsItem
{
    Q_OBJECT
public:
    explicit DeviceSettingsItem(const Device *device);

    void setDevice(const Device *device);
    void setLoading(const bool &loading);

    const Device *device() const;

Q_SIGNALS:
    void requestConnectDevice(const Device *device) const;
    void requestShowDetail(const Device *device) const;

protected:
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void onDeviceStateChanged(const Device::State &state);
    void onDevicePairedChanged(const bool &paired);
    void onStopLoadingAnimation();

private:
    const Device *m_device;

    dcc::widgets::NormalLabel *m_titleLabel;
    dcc::widgets::NormalLabel *m_tipLabel;
    dcc::widgets::LoadingIndicator *m_loadingIndicator;
    dcc::widgets::NextButton *m_nextButton;
};

} // namespace bluetooth
} // namespace dcc

#endif // DCC_BLUETOOTH_DEVICESETTINGSITEM_H
