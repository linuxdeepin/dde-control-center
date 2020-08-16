/*
 * Copyright (C) 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang<andywang_cm@deepin.com>
 *
 * Maintainer: andywang<andywang_cm@deepin.com>
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
#include "widgets/contentwidget.h"
#include <QMap>

namespace dcc {
namespace bluetooth {
class BluetoothModel;
class BluetoothWorker;
class Device;
class Adapter;
class PinCodeDialog;
}
}

namespace dcc {
namespace widgets {
class TranslucentFrame;
}
}

namespace DCC_NAMESPACE {
namespace bluetooth {
class AdapterWidget;
class BluetoothWidget : public dcc::ContentWidget
{
    Q_OBJECT

public:
    explicit BluetoothWidget(dcc::bluetooth::BluetoothModel *model);
    virtual ~BluetoothWidget();
    void setModel(dcc::bluetooth::BluetoothModel *model);
    void loadDetailPage();
Q_SIGNALS:
    void showBluetoothDetail(const dcc::bluetooth::Adapter *adapter) const;
    void requestModuleVisible(const bool visible) const;

    void requestSetToggleAdapter(const dcc::bluetooth::Adapter *adapter, const bool &toggled);
    void requestConnectDevice(const dcc::bluetooth::Device *device);
    void requestDisconnectDevice(const dcc::bluetooth::Device *device);
    void requestShowDetail(const dcc::bluetooth::Adapter *adapter, const dcc::bluetooth::Device *device);
    void requestSetAlias(const dcc::bluetooth::Adapter *adapter, const QString &alias);
    void showDeviceDetail(const dcc::bluetooth::Adapter *adapter, const dcc::bluetooth::Device *device);
    void requestDiscoverable(const dcc::bluetooth::Adapter *adapter, const bool &discoverable);

public Q_SLOTS:
    void addAdapter(const dcc::bluetooth::Adapter *adapter);
    void removeAdapter(const dcc::bluetooth::Adapter *adapter);
    void requestRefresh(const dcc::bluetooth::Adapter *adapter);

private:
    void setVisibleState();
    AdapterWidget *getAdapter(const dcc::bluetooth::Adapter *adapter);
    void updateWidget();

private:
    dcc::bluetooth::BluetoothModel *m_model;
    dcc::bluetooth::BluetoothWorker *m_bluetoothWorker;
    QMap<const dcc::bluetooth::Adapter *, AdapterWidget *> m_valueMap;
    dcc::widgets::TranslucentFrame *m_tFrame;
};
}
}
