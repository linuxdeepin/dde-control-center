// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "bluetoothworker.h"

#include "bluetoothdbusproxy.h"

#include <QDBusInterface>
#include <QDBusObjectPath>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>
#include <QTimer>
#include <DDesktopServices>

DGUI_USE_NAMESPACE
Q_LOGGING_CATEGORY(DdcBluetoothWorkder, "dcc-bluetooth-worker")

BluetoothWorker::BluetoothWorker(BluetoothModel *model, QObject *parent)
    : QObject(parent)
    , m_bluetoothDBusProxy(new BluetoothDBusProxy(this))
    , m_model(model)
    , m_connectingAudioDevice(false)
    , m_state(m_bluetoothDBusProxy->state())
{
    // clang-format off
    connect(m_bluetoothDBusProxy, &BluetoothDBusProxy::StateChanged, this, &BluetoothWorker::onStateChanged);
    connect(m_bluetoothDBusProxy, &BluetoothDBusProxy::AdapterAdded, this, &BluetoothWorker::addAdapter);
    connect(m_bluetoothDBusProxy, &BluetoothDBusProxy::AdapterRemoved, this, &BluetoothWorker::removeAdapter);
    connect(m_bluetoothDBusProxy, &BluetoothDBusProxy::AdapterPropertiesChanged, this, &BluetoothWorker::onAdapterPropertiesChanged);
    connect(m_bluetoothDBusProxy, &BluetoothDBusProxy::DeviceAdded, this, &BluetoothWorker::addDevice);
    connect(m_bluetoothDBusProxy, &BluetoothDBusProxy::DeviceRemoved, this, &BluetoothWorker::removeDevice);
    connect(m_bluetoothDBusProxy, &BluetoothDBusProxy::DevicePropertiesChanged, this, &BluetoothWorker::onDevicePropertiesChanged);

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
    // clang-format on
}

BluetoothWorker::~BluetoothWorker() { }

void BluetoothWorker::onStateChanged(uint state)
{
    // 当蓝牙状态由0变成大于0时，强制刷新蓝牙列表
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

void BluetoothWorker::disconnectDevice(const QString & deviceId)
{
    QDBusObjectPath path(deviceId);
    m_bluetoothDBusProxy->DisconnectDevice(path);
    qCDebug(DdcBluetoothWorkder) << "disconnect from device: " << deviceId;
}

void BluetoothWorker::ignoreDevice(const QString &deviceId, const QString adapterId)
{
    m_bluetoothDBusProxy->RemoveDevice(QDBusObjectPath(adapterId),
                                       QDBusObjectPath(deviceId));
    qCDebug(DdcBluetoothWorkder) << "ignore device: " << deviceId;
}

void BluetoothWorker::onAdapterPropertiesChanged(const QString &json)
{
    const QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    const QJsonObject obj = doc.object();
    const QString id = obj["Path"].toString();

    BluetoothAdapter *adapter = const_cast<BluetoothAdapter *>(m_model->adapterById(id));
    if (adapter) {
        adapter->inflate(obj);
        m_model->updateAdaptersModel(adapter);
    }
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
                adapterPointer->updateDeviceData(device);
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
    const int battery = obj["Battery"].toInt();

    const BluetoothAdapter *result = m_model->adapterById(adapterId);
    BluetoothAdapter *adapter = const_cast<BluetoothAdapter *>(result);
    if (adapter) {
        const BluetoothDevice *result1 = adapter->deviceById(id);
        BluetoothDevice *device = const_cast<BluetoothDevice *>(result1);
        if (!device)
            device = new BluetoothDevice(adapter);
        device->setBattery(battery);
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


void BluetoothWorker::setDeviceAlias(const QString &deviceId, const QString &alias)
{
    m_bluetoothDBusProxy->SetDeviceAlias(QDBusObjectPath(deviceId), alias);
}

void BluetoothWorker::setAdapterDiscoverable(const QString &path)
{
    m_bluetoothDBusProxy->RequestDiscovery(QDBusObjectPath(path));
}


void BluetoothWorker::setAdapterDiscovering(const QString &path, bool enable)
{
    m_bluetoothDBusProxy->SetAdapterDiscovering(QDBusObjectPath(path), enable);
}

void BluetoothWorker::playErrorSound()
{
    DDesktopServices::playSystemSoundEffect(DDesktopServices::SSE_Error);
}

bool BluetoothWorker::displaySwitch()
{
    return m_bluetoothDBusProxy->property("DisplaySwitch").toBool();
}

void BluetoothWorker::setAdapterPowered(const QString adapterId, bool powered)
{
    const BluetoothAdapter  *adapter = m_model->adapterById(adapterId);
    if (adapter) {
        const_cast<BluetoothAdapter *>(adapter)->setAdapterPowered(powered);
    }
}

void BluetoothWorker::setAdapterDiscoverable(const QString adapterId, bool discoverable)
{
    QDBusObjectPath path(adapterId);
    m_bluetoothDBusProxy->SetAdapterDiscoverable(path, discoverable);
}

void BluetoothWorker::setAdapterAlias(const QString adapterId, const QString &alias)
{
    m_bluetoothDBusProxy->SetAdapterAlias(QDBusObjectPath(adapterId), alias);
}

void BluetoothWorker::connectDevice(const QString &deviceId, const QString adapterId)
{
    // INFO: when is headset, not connect twice
    const BluetoothAdapter *adapter = m_model->adapterById(adapterId);
    if (adapter == nullptr)
        return;

    const BluetoothDevice *device = adapter->deviceById(deviceId);
    if (device
        && (device->deviceType() == "audio-headset" || device->deviceType() == "autio-headphones")
        && device->state() == BluetoothDevice::StateAvailable) {
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
    qCDebug(DdcBluetoothWorkder) << "connect to device: " << device->name();
}

void BluetoothWorker::jumpToAirPlaneMode()
{
    QDBusMessage message = QDBusMessage::createMethodCall("com.deepin.dde.ControlCenter", // 服务名
                                                           "/com/deepin/dde/ControlCenter", // 对象路径
                                                           "com.deepin.dde.ControlCenter", // 接口名
                                                           "ShowPage"); // 方法名

    message << "network/airplaneMode";

    QDBusConnection::sessionBus().asyncCall(message);
}

void BluetoothWorker::setDisplaySwitch(const bool &on)
{
    m_bluetoothDBusProxy->setDisplaySwitch(on);
}

void BluetoothWorker::showBluetoothTransDialog(const QString &address, const QStringList &files)
{
    qDebug() << " showBluetoothTransDialog:  " << address << files;
    QStringList fileList;
    for (auto filePath : files) {
        filePath = filePath.remove("file://");
        fileList.append(filePath);
    }

    m_bluetoothDBusProxy->showBluetoothTransDialog(address, fileList);
}

void BluetoothWorker::ignoreDevice(const BluetoothAdapter *adapter, const BluetoothDevice *device)
{
    Q_UNUSED(adapter)
    Q_UNUSED(device)
}
