// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "interface/moduleinterface.h"

#include <QMap>
#include <QObject>

class QDBusObjectPath;

namespace dcc {
namespace bluetooth {
class BluetoothModel;
class BluetoothWorker;
class Device;
class Adapter;
class PinCodeDialog;
}
}

namespace DCC_NAMESPACE {
namespace bluetooth {
class BluetoothWidget;
class BluetoothModule : public QObject, public ModuleInterface
{
    Q_OBJECT
public:
    explicit BluetoothModule(FrameProxyInterface *frame, QObject *parent = nullptr);
    void preInitialize(bool sync = false , FrameProxyInterface::PushType = FrameProxyInterface::PushType::Normal) override;
    void initialize() override;
    void reset() override;
    void active() override;
    const QString name() const override;
    const QString displayName() const override;
    void contentPopped(QWidget *const w) override;
    int load(const QString &path) override;
    void addChildPageTrans() const override;

private:
    void initSearchData() override;

public Q_SLOTS:
    void showPinCode(const QDBusObjectPath &device, const QString &code);
    void closePinCode(const QDBusObjectPath &device);
    void showDeviceDetail(const dcc::bluetooth::Adapter *adapter, const dcc::bluetooth::Device *device);
    void popPage();

private:
    BluetoothWidget *m_bluetoothWidget;
    dcc::bluetooth::BluetoothModel *m_bluetoothModel;
    dcc::bluetooth::BluetoothWorker *m_bluetoothWorker;
    QMap<QDBusObjectPath, dcc::bluetooth::PinCodeDialog *> m_dialogs;
};
}
}
