/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DCC_BLUETOOTH_DEVICESETTINGSITEM_H
#define DCC_BLUETOOTH_DEVICESETTINGSITEM_H

#include "settingsitem.h"
#include "device.h"


namespace Dtk {
namespace Widget {
class DLoadingIndicator;
}
}

namespace dcc {
namespace widgets {
class NormalLabel;
class NextButton;
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

signals:
    void requestConnectDevice(const Device *device) const;
    void requestShowDetail(const Device *device) const;

protected:
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private slots:
    void onDeviceStateChanged(const Device::State &state);
    void onDevicePairedChanged(const bool &paired);

private:
    const Device *m_device;

    dcc::widgets::NormalLabel *m_titleLabel;
    dcc::widgets::NormalLabel *m_tipLabel;
    Dtk::Widget::DLoadingIndicator *m_loadingIndicator;
    dcc::widgets::NextButton *m_nextButton;
};

} // namespace bluetooth
} // namespace dcc

#endif // DCC_BLUETOOTH_DEVICESETTINGSITEM_H
