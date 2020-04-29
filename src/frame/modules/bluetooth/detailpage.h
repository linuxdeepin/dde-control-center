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

#ifndef DCC_BLUETOOTH_DETAILPAGE_H
#define DCC_BLUETOOTH_DETAILPAGE_H

#include "widgets/contentwidget.h"

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

Q_SIGNALS:
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
