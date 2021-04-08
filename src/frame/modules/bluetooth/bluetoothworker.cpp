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
#include <QTimer>

namespace dcc {
namespace bluetooth {

BluetoothWorker::BluetoothWorker(BluetoothModel *model, bool sync) :
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
    connect(m_bluetoothInter, &DBusBluetooth::Cancelled, this, [=] (const QDBusObjectPath &device) {
        PinCodeDialog *dialog = m_dialogs[device];
        if (dialog != nullptr) {
            m_dialogs.remove(device);
            QMetaObject::invokeMethod(dialog, "deleteLater", Qt::QueuedConnection);
        } else {
            Q_EMIT pinCodeCancel(device);
        }
    });

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

    connect(m_bluetoothInter, &DBusBluetooth::DisplayPasskey, this, [ = ] (const QDBusObjectPath &in0, uint in1, uint in2) {
        qDebug() << "request display passkey: " << in0.path() << in1 << in2;

        PinCodeDialog *dialog = PinCodeDialog::instance(QString::number(in1), false);
        m_dialogs[in0] = dialog;
        if (!dialog->isVisible()) {
            dialog->exec();
            QMetaObject::invokeMethod(dialog, "deleteLater", Qt::QueuedConnection);
        }
    });

    connect(m_bluetoothInter, &DBusBluetooth::DisplayPinCode, this, [ = ] (const QDBusObjectPath &in0, const QString &in1) {
        qDebug() << "request display pincode: " << in0.path() << in1;

        PinCodeDialog *dialog = PinCodeDialog::instance(in1, false);
        m_dialogs[in0] = dialog;
        if (!dialog->isVisible()) {
            dialog->exec();
            QMetaObject::invokeMethod(dialog, "deleteLater", Qt::QueuedConnection);
        }
    });

    connect(m_bluetoothInter, &DBusBluetooth::TransportableChanged, m_model, &BluetoothModel::setTransportable);
    m_model->setTransportable(m_bluetoothInter->transportable());

    m_bluetoothInter->setSync(sync);

    //第一次调用时传true，refresh 函数会使用同步方式去获取蓝牙设备数据
    //避免出现当dbus调用控制中心接口直接显示蓝牙模块时，
    //因为异步的数据获取使控制中心设置了蓝牙模块不可见，
    //而出现没办法显示蓝牙模块
    refresh(true);

    m_bluetoothInter->setSync(false);
}

BluetoothWorker::~BluetoothWorker()
{

}

BluetoothWorker &BluetoothWorker::Instance(bool sync)
{
    static BluetoothWorker worker(new BluetoothModel, sync);
    return worker;
}

void BluetoothWorker::activate()
{
    if (!m_bluetoothInter->isValid()) {
        return;
    }

    blockDBusSignals(false);
    m_bluetoothInter->ClearUnpairedDevice();

    refresh();
}

void BluetoothWorker::deactivate()
{
    blockDBusSignals(true);
}

void BluetoothWorker::blockDBusSignals(bool block)
{
    if (!m_bluetoothInter->isValid()) {
        return;
    }

    m_bluetoothInter->blockSignals(block);
}

void BluetoothWorker::setAdapterPowered(const Adapter *adapter, const bool &powered)
{
    QTimer *timer = new QTimer;
    timer->setSingleShot(true);
    // 500后后端还不响应,前端就显示一个加载中的状态
    timer->setInterval(500);

    connect(timer, &QTimer::timeout, adapter, &Adapter::loadStatus);

    timer->start();

    QDBusObjectPath path(adapter->id());
    // 关闭蓝牙之前删除历史蓝牙设备列表，确保完全是删除后再设置开关
    if (!powered) {
        QDBusPendingCall call = m_bluetoothInter->ClearUnpairedDevice();
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
        connect(watcher, &QDBusPendingCallWatcher::finished, [ = ] {
            if (!call.isError()) {
                QDBusPendingCall adapterPoweredOffCall  = m_bluetoothInter->SetAdapterPowered(path, false);
                QDBusPendingCallWatcher *watchers = new QDBusPendingCallWatcher(adapterPoweredOffCall, this);
                connect(watchers, &QDBusPendingCallWatcher::finished, [this, adapterPoweredOffCall, adapter, timer, powered] {
                    if (adapterPoweredOffCall.isError()) {
                        qDebug() << adapterPoweredOffCall.error().message();
                        adapter->poweredChanged(adapter->powered(), adapter->discovering());
                    }
                    adapter->loadStatus();
                    connect(adapter, &Adapter::poweredChanged, [=](const bool &receivePowerd, const bool &) {
                        if (powered == receivePowerd) {
                            m_model->adpaterPowerChanged(adapter->powered());
                        }
                    });
                    delete timer;
                });
            } else {
                qDebug() << call.error().message();
            }
        });
    } else {
        QDBusPendingCall adapterPoweredOnCall  = m_bluetoothInter->SetAdapterPowered(path, true);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(adapterPoweredOnCall, this);
        connect(watcher, &QDBusPendingCallWatcher::finished, [this, adapterPoweredOnCall, adapter, timer, powered] {
            if (adapterPoweredOnCall.isError()) {
                qDebug() << adapterPoweredOnCall.error().message();
                adapter->poweredChanged(adapter->powered(), adapter->discovering());
                return;
            }
            adapter->loadStatus();
            connect(adapter, &Adapter::poweredChanged, [=](const bool &receivePowerd, const bool &) {
                if (powered == receivePowerd) {
                    m_model->adpaterPowerChanged(adapter->powered());
                }
            });
            delete timer;
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
    //m_bluetoothInter->ClearUnpairedDevice();
    qDebug() << "ignore device: " << device->name();
}

void BluetoothWorker::connectDevice(const Device *device, const Adapter *adapter)
{
    for (const Adapter *a : m_model->adapters()) {
        for (const Device *d : a->devices()) {
            Device *vd = const_cast<Device*>(d);
            if (vd) vd->setConnecting(d == device);
        }
    }

    QDBusObjectPath path(device->id());
    m_bluetoothInter->ConnectDevice(path, QDBusObjectPath(adapter->id()));
    qDebug() << "connect to device: " << device->name();
}

void BluetoothWorker::inflateAdapter(Adapter *adapter, const QJsonObject &adapterObj)
{
    const QString path = adapterObj["Path"].toString();
    const QString alias = adapterObj["Alias"].toString();
    const bool powered = adapterObj["Powered"].toBool();
    const bool discovering = adapterObj["Discovering"].toBool();
    const bool discovered = adapterObj["Discoverable"].toBool();

    adapter->setDiscoverabled(discovered);
    adapter->setId(path);
    adapter->setName(alias);
    adapter->setPowered(powered, discovering);

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
                const QString name = val.toObject()["Name"].toString();

                const Device *result = adapter->deviceById(id);
                Device *device = const_cast<Device*>(result);
                if (!device) {
                    device = new Device(adapter);
                } else {
                    if (device->name() != name) adapter->removeDevice(device->id());
                }
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
            qDebug() << call.error().message();
        }
    });
}

void BluetoothWorker::inflateDevice(Device *device, const QJsonObject &deviceObj)
{
    const QString id = deviceObj["Path"].toString();
    const QString addr = deviceObj["Address"].toString();
    const QString alias = deviceObj["Alias"].toString();
    const QString name = deviceObj["Name"].toString();
    const bool paired = deviceObj["Paired"].toBool();
    const Device::State state = Device::State(deviceObj["State"].toInt());
    const bool connectState = deviceObj["ConnectState"].toBool();
    const QString icon = deviceObj["Icon"].toString();

    device->setId(id);
    device->setAddress(addr);
    device->setName(name);
    device->setAlias(alias);
    device->setPaired(paired);
    device->setState(state, connectState);
    device->setDeviceType(icon);
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
    const QString name = obj["Name"].toString();
    for (const Adapter *adapter : m_model->adapters()) {
        Adapter *adapterPointer = const_cast<Adapter*>(adapter);
        Device *device = const_cast<Device*>(adapter->deviceById(id));
        if (device) {
            if (device->name() == name) {
                inflateDevice(device, obj);
            } else {
                if (!adapterPointer)
                    return;
                adapterPointer->removeDevice(device->id());
                inflateDevice(device, obj);
                adapterPointer->addDevice(device);
            }
        }
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

void BluetoothWorker::refresh(bool beFirst)
{
    if (!m_bluetoothInter->isValid()) return;

    auto resol = [this](const QDBusReply<QString> &req){
        const QString replyStr = req.value();
        QJsonDocument doc = QJsonDocument::fromJson(replyStr.toUtf8());
        QJsonArray arr = doc.array();
        for (QJsonValue val : arr) {
            Adapter *adapter = new Adapter(m_model);
            inflateAdapter(adapter, val.toObject());

            m_model->addAdapter(adapter);
        }
    };

    if (beFirst) {
        QDBusInterface *inter = new QDBusInterface("com.deepin.daemon.Bluetooth",
                                                   "/com/deepin/daemon/Bluetooth",
                                                   "com.deepin.daemon.Bluetooth",
                                                   QDBusConnection::sessionBus());
        QDBusReply<QString> reply = inter->call("GetAdapters");
        resol(reply);

        return;
    }

    QDBusPendingCall call = m_bluetoothInter->GetAdapters();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [ = ] {
        if (!call.isError()) {
            QDBusReply<QString> reply = call.reply();
            resol(reply);
        } else {
            qDebug() << call.error().message();
        }
    });
}

void BluetoothWorker::setAlias(const Adapter *adapter, const QString &alias)
{
    m_bluetoothInter->SetAdapterAlias(QDBusObjectPath(adapter->id()), alias);
}

void BluetoothWorker::setDeviceAlias(const Device *device, const QString &alias)
{
    QDBusObjectPath path(device->id());
    m_bluetoothInter->SetDeviceAlias(path, alias);
}

void BluetoothWorker::setAdapterDiscoverable(const QString &path)
{
    QDBusObjectPath dPath(path);

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

void BluetoothWorker::onRequestSetDiscoverable(const Adapter *adapter, const bool &discoverable)
{
    QDBusObjectPath path(adapter->id());
    m_bluetoothInter->SetAdapterDiscoverable(path, discoverable);
}

} // namespace bluetooth
} // namespace dcc
