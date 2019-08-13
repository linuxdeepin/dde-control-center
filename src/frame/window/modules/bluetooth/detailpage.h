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

#pragma once

#include "window/namespace.h"
#include "widgets/contentwidget.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QLineEdit;
class QPushButton;
QT_END_NAMESPACE

namespace dcc {
namespace bluetooth {
class Device;
class Adapter;
}
}

namespace DCC_NAMESPACE {
namespace bluetooth {
class DetailPage : public dcc::ContentWidget
{
    Q_OBJECT
public:
    explicit DetailPage(const dcc::bluetooth::Adapter *adapter, const dcc::bluetooth::Device *device);

Q_SIGNALS:
    void requestIgnoreDevice(const dcc::bluetooth::Adapter *adapter, const dcc::bluetooth::Device *device);
    void requestDisconnectDevice(const dcc::bluetooth::Device *device);
    void requestBack();
    void requestSetDevAlias(const dcc::bluetooth::Device *device, QString &devAlias);

private Q_SLOTS:
    void onDeviceNameChanged();

private:
    const dcc::bluetooth::Adapter *m_adapter;
    const dcc::bluetooth::Device *m_device;
    QPushButton *m_ignoreButton;
    QPushButton *m_disconnectButton;
    QPushButton *m_backButton;
    QLabel      *m_devNameLabel;
    QLineEdit   *m_editDevName;
};
}
}
