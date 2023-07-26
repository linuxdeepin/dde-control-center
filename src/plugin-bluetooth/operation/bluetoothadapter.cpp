//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "bluetoothadapter.h"
#include "bluetoothdbusproxy.h"

#include <QDBusObjectPath>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(DdcBluetoothAdapter, "dcc-bluetooth-adapter")

BluetoothAdapter::BluetoothAdapter(BluetoothDBusProxy *proxy, QObject *parent)
    : QObject(parent)
    , m_id("")
    , m_name("")
    , m_powered(false)
    , m_discovering(false)
    , m_discoverable(false)
    , m_bluetoothDBusProxy(proxy)
{
}

void BluetoothAdapter::setName(const QString &name)
{
    if (name != m_name) {
        m_name = name;
        Q_EMIT nameChanged(name);
    }
}

void BluetoothAdapter::addDevice(const BluetoothDevice *device)
{
    if (!deviceById(device->id())) {
        m_devicesId << device->id();
        m_devices[device->id()] = device;
        //打印配对设备信息,方便查看设备显示顺序
        if (!device->name().isEmpty() && device->paired())
            qCDebug(DdcBluetoothAdapter) << "BluetoothAdapter add device " << device->name();
        Q_EMIT deviceAdded(device);
    }
}

void BluetoothAdapter::removeDevice(const QString &deviceId)
{
    const BluetoothDevice *device = nullptr;

    device = deviceById(deviceId);
    if (device) {
        m_devicesId.removeOne(deviceId);
        m_devices.remove(deviceId);
        Q_EMIT deviceRemoved(deviceId);
    }
}

void BluetoothAdapter::setPowered(bool powered, bool discovering)
{
    if (!powered) {
        Q_EMIT closeDetailPage();
    }
    if (powered != m_powered || (powered && m_powered && discovering != m_discovering)) {
        m_powered = powered;
        m_discovering = discovering;
        Q_EMIT poweredChanged(powered, discovering);
    }
}

void BluetoothAdapter::setAdapterPowered(const bool &powered)
{
    // 关闭蓝牙之前删除历史蓝牙设备列表，确保完全是删除后再设置开关
    if (powered) {
        m_bluetoothDBusProxy->SetAdapterPowered(QDBusObjectPath(id()), true, this, SLOT(onSetAdapterPowered()), SLOT(onSetAdapterPoweredError()));
    } else {
        m_bluetoothDBusProxy->ClearUnpairedDevice(this, SLOT(onClearUnpairedDevice()));
    }
}

void BluetoothAdapter::onClearUnpairedDevice()
{
    m_bluetoothDBusProxy->SetAdapterPowered(QDBusObjectPath(id()), false, this, SLOT(onSetAdapterPowered()), SLOT(onSetAdapterPoweredError()));
}

void BluetoothAdapter::onSetAdapterPowered()
{
    Q_EMIT loadStatus();
}

void BluetoothAdapter::onSetAdapterPoweredError()
{
    Q_EMIT poweredChanged(powered(), discovering());
}

void BluetoothAdapter::setDiscoverabled(const bool discoverable)
{
    if (m_discoverable == discoverable) {
        return;
    }
    m_discoverable = discoverable;
    Q_EMIT discoverableChanged(discoverable);
}

QMap<QString, const BluetoothDevice *> BluetoothAdapter::devices() const
{
    return m_devices;
}

QList<QString> BluetoothAdapter::devicesId() const
{
    return m_devicesId;
}

const BluetoothDevice *BluetoothAdapter::deviceById(const QString &id) const
{
    return m_devices.keys().contains(id) ? m_devices[id] : nullptr;
}

void BluetoothAdapter::setId(const QString &id)
{
    m_id = id;
}

void BluetoothAdapter::inflate(const QJsonObject &obj)
{
    const QString path = obj["Path"].toString();
    const QString alias = obj["Alias"].toString();
    const bool powered = obj["Powered"].toBool();
    const bool discovering = obj["Discovering"].toBool();
    const bool discovered = obj["Discoverable"].toBool();

    setDiscoverabled(discovered);
    setId(path);
    setName(alias);
    setPowered(powered, discovering);

    QDBusObjectPath dPath(path);
    m_bluetoothDBusProxy->GetDevices(dPath, this, SLOT(onGetDevices(QString)));
}

void BluetoothAdapter::inflateDevice(BluetoothDevice *device, const QJsonObject &deviceObj)
{
    const QString id = deviceObj["Path"].toString();
    const QString addr = deviceObj["Address"].toString();
    const QString alias = deviceObj["Alias"].toString();
    const QString name = deviceObj["Name"].toString();
    const bool paired = deviceObj["Paired"].toBool();
    const BluetoothDevice::State state = BluetoothDevice::State(deviceObj["State"].toInt());
    const bool connectState = deviceObj["ConnectState"].toBool();
    const QString icon = deviceObj["Icon"].toString();
    const int battery = deviceObj["Battery"].toInt();

    //    if (icon == "audio-card") {
    //        m_connectingAudioDevice = (BluetoothDevice::StateAvailable == state);
    //    }

    // FIXME: If the name and alias of the Bluetooth device are both empty, it will not be updated by default.
    // To solve the problem of blank device name display.
    if (alias.isEmpty() && name.isEmpty())
        return;

    device->setId(id);
    device->setAddress(addr);
    device->setName(name);
    device->setAlias(alias);
    device->setPaired(paired);
    device->setState(state, connectState);
    device->setDeviceType(icon);
    device->setBattery(battery);
}

void BluetoothAdapter::onGetDevices(QString replyStr)
{
    QStringList tmpList;
    QJsonDocument doc = QJsonDocument::fromJson(replyStr.toUtf8());
    QJsonArray arr = doc.array();
    for (QJsonValue val : arr) {
        const QString id = val.toObject()["Path"].toString();
        const QString name = val.toObject()["Name"].toString();

        const BluetoothDevice *result = deviceById(id);
        BluetoothDevice *device = const_cast<BluetoothDevice *>(result);
        if (!device) {
            device = new BluetoothDevice(this);
        } else {
            if (device->name() != name)
                removeDevice(device->id());
        }
        inflateDevice(device, val.toObject());
        addDevice(device);

        tmpList << id;
    }

    for (const BluetoothDevice *device : devices()) {
        if (!tmpList.contains(device->id())) {
            removeDevice(device->id());

            BluetoothDevice *target = const_cast<BluetoothDevice *>(device);
            if (target)
                target->deleteLater();
        }
    }
}
