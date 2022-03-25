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
#include "bluetoothdbusproxy.h"

#include <QDBusObjectPath>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>

BluetoothWorker::BluetoothWorker(BluetoothModel *model, QObject *parent)
    : QObject(parent)
    , m_bluetoothDBusProxy(new BluetoothDBusProxy(this))
    , m_model(model)
    , m_connectingAudioDevice(false)
    , m_state(m_bluetoothDBusProxy->state())
{
    connect(m_bluetoothDBusProxy, &BluetoothDBusProxy::StateChanged, this, &BluetoothWorker::onStateChanged);
    connect(m_bluetoothDBusProxy, &BluetoothDBusProxy::AdapterAdded, this, &BluetoothWorker::addAdapter);
    connect(m_bluetoothDBusProxy, &BluetoothDBusProxy::AdapterRemoved, this, &BluetoothWorker::removeAdapter);
    connect(m_bluetoothDBusProxy, &BluetoothDBusProxy::AdapterPropertiesChanged, this, &BluetoothWorker::onAdapterPropertiesChanged);
    connect(m_bluetoothDBusProxy, &BluetoothDBusProxy::DeviceAdded, this, &BluetoothWorker::addDevice);
    connect(m_bluetoothDBusProxy, &BluetoothDBusProxy::DeviceRemoved, this, &BluetoothWorker::removeDevice);
    connect(m_bluetoothDBusProxy, &BluetoothDBusProxy::DevicePropertiesChanged, this, &BluetoothWorker::onDevicePropertiesChanged);
    connect(m_bluetoothDBusProxy, &BluetoothDBusProxy::Cancelled, this, [=](const QDBusObjectPath &device) {
        PinCodeDialog *dialog = m_dialogs[device];
        if (dialog != nullptr) {
            m_dialogs.remove(device);
            QMetaObject::invokeMethod(dialog, "deleteLater", Qt::QueuedConnection);
        } else {
            Q_EMIT pinCodeCancel(device);
        }
    });

    connect(m_bluetoothDBusProxy, &BluetoothDBusProxy::RequestAuthorization, this, [](const QDBusObjectPath &in0) {
        qDebug() << "request authorization: " << in0.path();
    });

    connect(m_bluetoothDBusProxy, &BluetoothDBusProxy::RequestConfirmation, this, &BluetoothWorker::requestConfirmation);

    connect(m_bluetoothDBusProxy, &BluetoothDBusProxy::RequestPasskey, this, [](const QDBusObjectPath &in0) {
        qDebug() << "request passkey: " << in0.path();
    });

    connect(m_bluetoothDBusProxy, &BluetoothDBusProxy::RequestPinCode, this, [](const QDBusObjectPath &in0) {
        qDebug() << "request pincode: " << in0.path();
    });

    connect(m_bluetoothDBusProxy, &BluetoothDBusProxy::DisplayPasskey, this, [=](const QDBusObjectPath &in0, uint in1, uint in2) {
        qDebug() << "request display passkey: " << in0.path() << in1 << in2;

        PinCodeDialog *dialog = PinCodeDialog::instance(QString::number(in1), false);
        m_dialogs[in0] = dialog;
        if (!dialog->isVisible()) {
            dialog->exec();
            QMetaObject::invokeMethod(dialog, "deleteLater", Qt::QueuedConnection);
        }
    });

    connect(m_bluetoothDBusProxy, &BluetoothDBusProxy::DisplayPinCode, this, [=](const QDBusObjectPath &in0, const QString &in1) {
        qDebug() << "request display pincode: " << in0.path() << in1;

        PinCodeDialog *dialog = PinCodeDialog::instance(in1, false);
        m_dialogs[in0] = dialog;
        if (!dialog->isVisible()) {
            dialog->exec();
            QMetaObject::invokeMethod(dialog, "deleteLater", Qt::QueuedConnection);
        }
    });

    connect(m_bluetoothDBusProxy, &BluetoothDBusProxy::TransportableChanged, m_model, &BluetoothModel::setTransportable);
    connect(m_bluetoothDBusProxy, &BluetoothDBusProxy::CanSendFileChanged, m_model, &BluetoothModel::setCanSendFile);
    connect(m_bluetoothDBusProxy, &BluetoothDBusProxy::DisplaySwitchChanged, m_model, &BluetoothModel::setDisplaySwitch);

    m_model->setTransportable(m_bluetoothDBusProxy->transportable());
    m_model->setCanSendFile(m_bluetoothDBusProxy->canSendFile());
    m_model->setDisplaySwitch(m_bluetoothDBusProxy->displaySwitch());

    connect(m_bluetoothDBusProxy, &BluetoothDBusProxy::EnabledChanged, m_model, &BluetoothModel::setAirplaneEnable);
    m_model->setAirplaneEnable(m_bluetoothDBusProxy->enabled());

    //第一次调用时传true，refresh 函数会使用同步方式去获取蓝牙设备数据
    //避免出现当dbus调用控制中心接口直接显示蓝牙模块时，
    //因为异步的数据获取使控制中心设置了蓝牙模块不可见，
    //而出现没办法显示蓝牙模块
    refresh(true);
}

BluetoothWorker::~BluetoothWorker()
{
}

void BluetoothWorker::onStateChanged(uint state)
{
    //当蓝牙状态由0变成大于0时，强制刷新蓝牙列表
    if (!m_state && state > 0)
        refresh(true);

    m_state = state;
}

void BluetoothWorker::activate()
{
    if (!m_bluetoothDBusProxy->bluetoothIsValid()) {
        return;
    }

    blockDBusSignals(false);
    m_bluetoothDBusProxy->ClearUnpairedDevice();

    refresh();
}

void BluetoothWorker::deactivate()
{
    blockDBusSignals(true);
}

void BluetoothWorker::blockDBusSignals(bool block)
{
    if (!m_bluetoothDBusProxy->bluetoothIsValid()) {
        return;
    }

    m_bluetoothDBusProxy->blockSignals(block);
}

void BluetoothWorker::setAdapterPowered(const BluetoothAdapter *adapter, const bool &powered)
{
    QTimer *timer = new QTimer;
    timer->setSingleShot(true);
    // 500后后端还不响应,前端就显示一个加载中的状态
    timer->setInterval(500);

    connect(timer, &QTimer::timeout, adapter, &BluetoothAdapter::loadStatus);

    timer->start();

    const_cast<BluetoothAdapter *>(adapter)->setAdapterPowered(powered);
}

void BluetoothWorker::disconnectDevice(const BluetoothDevice *device)
{
    QDBusObjectPath path(device->id());
    m_bluetoothDBusProxy->DisconnectDevice(path);
    qDebug() << "disconnect from device: " << device->name();
}

void BluetoothWorker::ignoreDevice(const BluetoothAdapter *adapter, const BluetoothDevice *device)
{
    m_bluetoothDBusProxy->RemoveDevice(QDBusObjectPath(adapter->id()), QDBusObjectPath(device->id()));
    qDebug() << "ignore device: " << device->name();
}

void BluetoothWorker::connectDevice(const BluetoothDevice *device, const BluetoothAdapter *adapter)
{
    if (device->deviceType() == "audio-card"
        && device->state() == BluetoothDevice::StateAvailable
        && device->deviceType() == "pheadset") {
        return;
    }
    for (const BluetoothAdapter *a : m_model->adapters()) {
        for (const BluetoothDevice *d : a->devices()) {
            BluetoothDevice *vd = const_cast<BluetoothDevice *>(d);
            if (vd)
                vd->setConnecting(d == device);
        }
    }

    QDBusObjectPath path(device->id());
    m_bluetoothDBusProxy->ConnectDevice(path, QDBusObjectPath(adapter->id()));
    qDebug() << "connect to device: " << device->name();
}

void BluetoothWorker::onAdapterPropertiesChanged(const QString &json)
{
    const QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    const QJsonObject obj = doc.object();
    const QString id = obj["Path"].toString();

    BluetoothAdapter *adapter = const_cast<BluetoothAdapter *>(m_model->adapterById(id));
    if (adapter)
        adapter->inflate(obj);
}

void BluetoothWorker::onDevicePropertiesChanged(const QString &json)
{
    const QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    const QJsonObject obj = doc.object();
    const QString id = obj["Path"].toString();
    const QString name = obj["Name"].toString();
    for (const BluetoothAdapter *adapter : m_model->adapters()) {
        BluetoothAdapter *adapterPointer = const_cast<BluetoothAdapter *>(adapter);
        BluetoothDevice *device = const_cast<BluetoothDevice *>(adapter->deviceById(id));
        if (device) {
            if (device->name() == name) {
                adapterPointer->inflateDevice(device, obj);
            } else {
                if (!adapterPointer)
                    return;
                adapterPointer->removeDevice(device->id());
                adapterPointer->inflateDevice(device, obj);
                adapterPointer->addDevice(device);
            }
        }
    }
}

void BluetoothWorker::addAdapter(const QString &json)
{
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject obj = doc.object();

    BluetoothAdapter *adapter = new BluetoothAdapter(m_bluetoothDBusProxy, m_model);
    adapter->inflate(obj);
    m_model->addAdapter(adapter);
}

void BluetoothWorker::removeAdapter(const QString &json)
{
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject obj = doc.object();
    const QString id = obj["Path"].toString();

    const BluetoothAdapter *result = m_model->removeAdapater(id);
    BluetoothAdapter *adapter = const_cast<BluetoothAdapter *>(result);
    if (adapter) {
        adapter->deleteLater();
        adapter = nullptr;
    }
}

void BluetoothWorker::addDevice(const QString &json)
{
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject obj = doc.object();
    const QString adapterId = obj["AdapterPath"].toString();
    const QString id = obj["Path"].toString();

    const BluetoothAdapter *result = m_model->adapterById(adapterId);
    BluetoothAdapter *adapter = const_cast<BluetoothAdapter *>(result);
    if (adapter) {
        const BluetoothDevice *result1 = adapter->deviceById(id);
        BluetoothDevice *device = const_cast<BluetoothDevice *>(result1);
        if (!device)
            device = new BluetoothDevice(adapter);
        adapter->inflateDevice(device, obj);
        adapter->addDevice(device);
    }
}

void BluetoothWorker::removeDevice(const QString &json)
{
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject obj = doc.object();
    const QString adapterId = obj["AdapterPath"].toString();
    const QString id = obj["Path"].toString();

    const BluetoothAdapter *result = m_model->adapterById(adapterId);
    BluetoothAdapter *adapter = const_cast<BluetoothAdapter *>(result);
    if (adapter) {
        adapter->removeDevice(id);
    }
}

void BluetoothWorker::refresh(bool beFirst)
{
    Q_UNUSED(beFirst)
    if (!m_bluetoothDBusProxy->bluetoothIsValid())
        return;

    const QString replyStr = m_bluetoothDBusProxy->GetAdapters();
    QJsonDocument doc = QJsonDocument::fromJson(replyStr.toUtf8());
    QJsonArray arr = doc.array();
    for (QJsonValue val : arr) {
        BluetoothAdapter *adapter = new BluetoothAdapter(m_bluetoothDBusProxy, m_model);
        adapter->inflate(val.toObject());

        m_model->addAdapter(adapter);
    }
}

void BluetoothWorker::setAlias(const BluetoothAdapter *adapter, const QString &alias)
{
    m_bluetoothDBusProxy->SetAdapterAlias(QDBusObjectPath(adapter->id()), alias);
}

void BluetoothWorker::setDeviceAlias(const BluetoothDevice *device, const QString &alias)
{
    QDBusObjectPath path(device->id());
    m_bluetoothDBusProxy->SetDeviceAlias(QDBusObjectPath(device->id()), alias);
}

void BluetoothWorker::setAdapterDiscoverable(const QString &path)
{
    m_bluetoothDBusProxy->RequestDiscovery(QDBusObjectPath(path));
}

void BluetoothWorker::pinCodeConfirm(const QDBusObjectPath &path, bool value)
{
    m_bluetoothDBusProxy->Confirm(path, value);
}
void BluetoothWorker::setAdapterDiscovering(const QDBusObjectPath &path, bool enable)
{
    m_bluetoothDBusProxy->SetAdapterDiscovering(path, enable);
}

void BluetoothWorker::onRequestSetDiscoverable(const BluetoothAdapter *adapter, const bool &discoverable)
{
    QDBusObjectPath path(adapter->id());
    m_bluetoothDBusProxy->SetAdapterDiscoverable(path, discoverable);
}

bool BluetoothWorker::displaySwitch()
{
    return m_bluetoothDBusProxy->property("DisplaySwitch").toBool();
}

void BluetoothWorker::setDisplaySwitch(const bool &on)
{
    m_bluetoothDBusProxy->setDisplaySwitch(on);
}

void BluetoothWorker::showBluetoothTransDialog(const QString &address, const QStringList &files)
{
    m_bluetoothDBusProxy->showBluetoothTransDialog(address, files);
}
