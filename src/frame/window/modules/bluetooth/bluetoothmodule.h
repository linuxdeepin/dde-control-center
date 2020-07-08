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
    void preInitialize(bool sync = false) override;
    void initialize() override;
    void reset() override;
    void active() override;
    const QString name() const override;
    const QString displayName() const override;
    void contentPopped(QWidget *const w) override;
    int load(const QString &path) override;

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
