/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
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

#ifndef BLUETOOTHMODULE_H
#define BLUETOOTHMODULE_H

#include "interfaces/moduleinterface.h"

class QDBusObjectPath;

namespace dcc {
namespace bluetooth {

class BluetoothWidget;
class BluetoothModel;
class BluetoothWorker;
class Device;
class Adapter;
class PinCodeDialog;

class BluetoothModule : public QObject, public ModuleInterface
{
    Q_OBJECT
public:
    explicit BluetoothModule(FrameProxyInterface *frame, QObject *parent = 0);

public Q_SLOTS:
    void showDeviceDetail(const Adapter *adapter, const Device *device);
    void showBluetoothDetail(const Adapter *adapter);

private:
    ~BluetoothModule();
    void initialize();
    void moduleActive();
    void moduleDeactive();
    void reset();
    void contentPopped(ContentWidget * const w);
    const QString name() const;
    ModuleWidget *moduleWidget();
    void showPage(const QString &pageName);

    void showPinCode(const QDBusObjectPath &device, const QString &code);
    void closePinCode(const QDBusObjectPath &device);

private:
    BluetoothWidget *m_bluetoothView;
    BluetoothModel *m_bluetoothModel;
    BluetoothWorker *m_bluetoothWorker;
    QMap<QDBusObjectPath, PinCodeDialog*> m_dialogs;
};

}
}

#endif // BLUETOOTHMODULE_H
