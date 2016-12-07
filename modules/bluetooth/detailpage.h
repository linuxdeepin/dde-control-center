/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DCC_BLUETOOTH_DETAILPAGE_H
#define DCC_BLUETOOTH_DETAILPAGE_H

#include "contentwidget.h"

class QPushButton;

namespace dcc {
namespace bluetooth {

class Device;
class Adapter;

class DetailPage : public ContentWidget
{
    Q_OBJECT
public:
    explicit DetailPage(const Adapter *adapter, const Device *device);

signals:
    void requestIgnoreDevice(const Adapter *adapter, const Device *device);
    void requestDisconnectDevice(const Device *device);

private:
    const Adapter *m_adapter;
    const Device *m_device;

    QPushButton *m_ignoreButton;
    QPushButton *m_disconnectButton;
};

} // namespace bluetooth
} // namespace dcc

#endif // DCC_BLUETOOTH_DETAILPAGE_H
