// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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

    inline QMap<const dcc::bluetooth::Adapter *, AdapterWidget *> getAdapterMap() { return m_valueMap; }

Q_SIGNALS:
    void showBluetoothDetail(const dcc::bluetooth::Adapter *adapter) const;
    void requestModuleVisible(const bool visible) const;

    void requestSetToggleAdapter(const dcc::bluetooth::Adapter *adapter, const bool &toggled);
    void requestConnectDevice(const dcc::bluetooth::Device *device, const dcc::bluetooth::Adapter *adapter);
    void requestDisconnectDevice(const dcc::bluetooth::Device *device);
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
