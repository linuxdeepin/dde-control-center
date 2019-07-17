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

#include "bluetoothworker.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "pincodedialog.h"

namespace dcc {
namespace bluetooth {

BluetoothWorker::BluetoothWorker(BluetoothModel *model) :
    QObject(),
    m_bluetoothInter(new DBusBluetooth("com.deepin.daemon.Bluetooth", "/com/deepin/daemon/Bluetooth", QDBusConnection::sessionBus(), this)),
    m_model(model)
{
    connect(m_bluetoothInter, &DBusBluetooth::AdapterAdded, this, &BluetoothWorker::addAdapter);
    connect(m_bluetoothInter, &DBusBluetooth::AdapterRemoved, this, &BluetoothWorker::removeAdapter);
    connect(m_bluetoothInter, &DBusBluetooth::AdapterPropertiesChanged, this, &BluetoothWorker::onAdapterPropertiesChanged);
    connect(m_bluetoothInter, &DBusBluetooth::DeviceAdded, this, &BluetoothWorker::addDevice);
    connect(m_bluetoothInter, &DBusBluetooth::DeviceRemoved, this, &BluetoothWorker::removeDevice);
    connect(m_bluetoothInter, &DBusBluetooth::DevicePropertiesChanged, this, &BluetoothWorker::onDevicePropertiesChanged);
    connect(m_bluetoothInter, &DBusBluetooth::Cancelled, this, &BluetoothWorker::pinCodeCancel);

    connect(m_bluetoothInter, &DBusBluetooth::RequestAuthorization, this, [] (const QDBusObjectPath &in0) {
        qDebug() << "request authorization: " << in0.path();
    });

    connect(m_bluetoothInter, &DBusBluetooth::RequestConfirmation, this, &BluetoothWorker::requestConfirmation);

    connect(m_bluetoothInter, &DBusBluetooth::RequestPasskey, this, [] (const QDBusObjectPath &in0) {
        qDebug() << "request passkey: " << in0.path();
    });

    connect(m_bluetoothInter, &DBusBluetooth::RequestPinCode, this, [] (const QDBusObjectPath &in0) {
        qDebug() << "request pincode: " << in0.path();
    });

    connect(m_bluetoothInter, &DBusBluetooth::DisplayPasskey, this, [] (const QDBusObjectPath &in0, uint in1, uint in2) {
        qDebug() << "request display passkey: " << in0.path() << in1 << in2;

        PinCodeDialog *dialog = PinCodeDialog::instance(QString::number(in1), false);
        if (!dialog->isVisible()) {
            dialog->exec();
            QMetaObject::invokeMethod(dialog, "deleteLater", Qt::QueuedConnection);
        }
    });

    connect(m_bluetoothInter, &DBusBluetooth::DisplayPinCode, this, [] (const QDBusObjectPath &in0, const QString &in1) {
        qDebug() << "request display pincode: " << in0.path() << in1;

        PinCodeDialog *dialog = PinCodeDialog::instance(in1, false);
        if (!dialog->isVisible()) {
            dialog->exec();
            QMetaObject::invokeMethod(dialog, "deleteLater", Qt::QueuedConnection);
        }
    });

    m_bluetoothInter->setSync(false, false);

    refresh();
}

BluetoothWorker::~BluetoothWorker()
{

}

BluetoothWorker &BluetoothWorker::Instance()
{
    static BluetoothWorker worker(new BluetoothModel);
    return worker;
}

void BluetoothWorker::activate()
{
    blockDBusSignals(false);

    if (!m_bluetoothInter->isValid()) {
        return;
    }

    m_bluetoothInter->ClearUnpairedDevice();

    refresh();
}

void BluetoothWorker::deactivate()
{
    blockDBusSignals(true);
}

void BluetoothWorker::blockDBusSignals(bool block)
{
    m_bluetoothInter->blockSignals(block);
}

void BluetoothWorker::setAdapterPowered(const Adapter *adapter, const bool &powered)
{
    QDBusObjectPath path(adapter->id());
    QDBusPendingCall call  = m_bluetoothInter->SetAdapterPowered(path, powered);

    if (powered) {
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
        connect(watcher, &QDBusPendingCallWatcher::finished, [this, call, adapter] {
            if (!call.isError()) {
                setAdapterDiscoverable(adapter->id());
            } else {
                qWarning() << call.error().message();
            }
        });
    }
}

void BluetoothWorker::disconnectDevice(const Device *device)
{
    QDBusObjectPath path(device->id());
    m_bluetoothInter->DisconnectDevice(path);
    qDebug() << "disconnect from device: " << device->name();
}

void BluetoothWorker::ignoreDevice(const Adapter *adapter, const Device *device)
{
    m_bluetoothInter->RemoveDevice(QDBusObjectPath(adapter->id()), QDBusObjectPath(device->id()));
    m_bluetoothInter->ClearUnpairedDevice();
    qDebug() << "ignore device: " << device->name();
}

void BluetoothWorker::connectDevice(const Device *device)
{
    for (const Adapter *a : m_model->adapters()) {
        for (const Device *d : a->devices()) {
            Device *vd = const_cast<Device*>(d);
            if (vd) vd->setConnecting(d == device);
        }
    }

    QDBusObjectPath path(device->id());
    m_bluetoothInter->ConnectDevice(path);
    qDebug() << "connect to device: " << device->name();
}

void BluetoothWorker::inflateAdapter(Adapter *adapter, const QJsonObject &adapterObj)
{
    const QString path = adapterObj["Path"].toString();
    const QString alias = adapterObj["Alias"].toString();
    const bool powered = adapterObj["Powered"].toBool();

    adapter->setId(path);
    adapter->setName(alias);
    adapter->setPowered(powered);

    Q_EMIT deviceEnableChanged();

    QPointer<Adapter> adapterPointer(adapter);

    QDBusObjectPath dPath(path);
    QDBusPendingCall call = m_bluetoothInter->GetDevices(dPath);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, adapterPointer, call] {
        if (!adapterPointer)
            return;

        Adapter *adapter = adapterPointer.data();

        if (!call.isError())  {
            QStringList tmpList;

            QDBusReply<QString> reply = call.reply();
            const QString replyStr = reply.value();
            QJsonDocument doc = QJsonDocument::fromJson(replyStr.toUtf8());
            QJsonArray arr = doc.array();
            for (QJsonValue val : arr) {
                const QString id = val.toObject()["Path"].toString();

                const Device *result = adapter->deviceById(id);
                Device *device = const_cast<Device*>(result);
                if (!device) device = new Device(adapter);
                inflateDevice(device, val.toObject());
                adapter->addDevice(device);

                tmpList << id;
            }

            for (const Device *device : adapter->devices()) {
                if (!tmpList.contains(device->id())) {
                    adapter->removeDevice(device->id());

                    Device *target = const_cast<Device*>(device);
                    if (target) target->deleteLater();
                }
            }
        } else {
            qWarning() << call.error().message();
        }
    });
}

void BluetoothWorker::inflateDevice(Device *device, const QJsonObject &deviceObj)
{
    const QString id = deviceObj["Path"].toString();
    const QString name = deviceObj["Alias"].toString();
    const bool paired = deviceObj["Paired"].toBool();
    const Device::State state = Device::State(deviceObj["State"].toInt());

    device->setId(id);
    device->setName(name);
    device->setPaired(paired);
    device->setState(state);
}

void BluetoothWorker::onAdapterPropertiesChanged(const QString &json)
{
    const QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    const QJsonObject obj = doc.object();
    const QString id = obj["Path"].toString();

    Adapter *adapter = const_cast<Adapter*>(m_model->adapterById(id));
    if (adapter) inflateAdapter(adapter, obj);
}

void BluetoothWorker::onDevicePropertiesChanged(const QString &json)
{
    const QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    const QJsonObject obj = doc.object();
    const QString id = obj["Path"].toString();
    for (const Adapter *adapter : m_model->adapters()) {
        Device *device = const_cast<Device*>(adapter->deviceById(id));
        if (device) inflateDevice(device, obj);
    }
}

void BluetoothWorker::addAdapter(const QString &json)
{
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject obj = doc.object();

    Adapter *adapter = new Adapter(m_model);
    inflateAdapter(adapter, obj);
    m_model->addAdapter(adapter);
}

void BluetoothWorker::removeAdapter(const QString &json)
{
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject obj = doc.object();
    const QString id = obj["Path"].toString();

    const Adapter *result = m_model->removeAdapater(id);
    Adapter *adapter = const_cast<Adapter*>(result);
    if (adapter) {
        adapter->deleteLater();
    }
}

void BluetoothWorker::addDevice(const QString &json)
{
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject obj = doc.object();
    const QString adapterId = obj["AdapterPath"].toString();
    const QString id = obj["Path"].toString();

    const Adapter *result = m_model->adapterById(adapterId);
    Adapter *adapter = const_cast<Adapter*>(result);
    if (adapter) {
        const Device *result1 = adapter->deviceById(id);
        Device *device = const_cast<Device*>(result1);
        if (!device) device = new Device(adapter);
        inflateDevice(device, obj);
        adapter->addDevice(device);
    }
}

void BluetoothWorker::removeDevice(const QString &json)
{
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject obj = doc.object();
    const QString adapterId = obj["AdapterPath"].toString();
    const QString id = obj["Path"].toString();

    const Adapter *result = m_model->adapterById(adapterId);
    Adapter *adapter = const_cast<Adapter*>(result);
    if (adapter) {
        adapter->removeDevice(id);
    }
}

void BluetoothWorker::refresh()
{
    if (!m_bluetoothInter->isValid()) return;

    QDBusPendingCall call = m_bluetoothInter->GetAdapters();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, call] {
        if (!call.isError()) {
            QDBusReply<QString> reply = call.reply();
            const QString replyStr = reply.value();
            QJsonDocument doc = QJsonDocument::fromJson(replyStr.toUtf8());
            QJsonArray arr = doc.array();
            for (QJsonValue val : arr) {
                Adapter *adapter = new Adapter(m_model);
                inflateAdapter(adapter, val.toObject());

                m_model->addAdapter(adapter);
            }
        } else {
            qWarning() << call.error().message();
        }
    });
}

void BluetoothWorker::setAlias(const Adapter *adapter, const QString &alias)
{
    m_bluetoothInter->SetAdapterAlias(QDBusObjectPath(adapter->id()), alias);
}

void BluetoothWorker::setAdapterDiscoverable(const QString &path)
{
    QDBusObjectPath dPath(path);
    m_bluetoothInter->SetAdapterDiscoverableTimeout(dPath, 60 * 5);
    m_bluetoothInter->SetAdapterDiscoverable(dPath, true);

    m_bluetoothInter->RequestDiscovery(dPath);
}

void BluetoothWorker::pinCodeConfirm(const QDBusObjectPath &path, bool value)
{
    m_bluetoothInter->Confirm(path, value);
}
void BluetoothWorker::setAdapterDiscovering(const QDBusObjectPath &path, bool enable)
{
    m_bluetoothInter->SetAdapterDiscovering(path, enable);
}

} // namespace bluetooth
} // namespace dcc
