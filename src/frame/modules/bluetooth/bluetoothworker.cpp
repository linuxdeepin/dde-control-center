// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "bluetoothworker.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>

namespace dcc {
namespace bluetooth {

BluetoothWorker::BluetoothWorker(BluetoothModel *model, bool sync)
    : QObject()
    , m_bluetoothInter(new DBusBluetooth("com.deepin.daemon.Bluetooth", "/com/deepin/daemon/Bluetooth", QDBusConnection::sessionBus(), this))
    , m_airPlaneModeInter(new DBusAirplaneMode("com.deepin.daemon.AirplaneMode", "/com/deepin/daemon/AirplaneMode", QDBusConnection::systemBus(), this))
    , m_model(model)
    , m_connectingAudioDevice(false)
    , m_state(m_bluetoothInter->state())
    , m_powerSwitchTimer(new QTimer(this))
{
    m_powerSwitchTimer->setSingleShot(true);
    m_powerSwitchTimer->setInterval(500);

    connect(m_bluetoothInter, &DBusBluetooth::StateChanged, this, &BluetoothWorker::onStateChanged);
    connect(m_bluetoothInter, &DBusBluetooth::AdapterAdded, this, &BluetoothWorker::addAdapter);
    connect(m_bluetoothInter, &DBusBluetooth::AdapterRemoved, this, &BluetoothWorker::removeAdapter);
    connect(m_bluetoothInter, &DBusBluetooth::AdapterPropertiesChanged, this, &BluetoothWorker::onAdapterPropertiesChanged);
    connect(m_bluetoothInter, &DBusBluetooth::DeviceAdded, this, &BluetoothWorker::addDevice);
    connect(m_bluetoothInter, &DBusBluetooth::DeviceRemoved, this, &BluetoothWorker::removeDevice);
    connect(m_bluetoothInter, &DBusBluetooth::DevicePropertiesChanged, this, &BluetoothWorker::onDevicePropertiesChanged);
    connect(m_bluetoothInter, &DBusBluetooth::Cancelled, this, [=] (const QDBusObjectPath &device) {
        PinCodeDialog *dialog = m_dialogs[device.path()];
        if (dialog != nullptr) {
            m_dialogs.remove(device.path());
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
        auto pinCode = QString::number(in1).rightJustified(6, '0');
        PinCodeDialog *dialog = PinCodeDialog::instance(pinCode, false);
        if (m_dialogs.keys().contains(in0.path()) && m_dialogs[in0.path()] && m_dialogs[in0.path()]->pinCode() != pinCode) {
            qDebug() << "not repeat dialog" ;
            m_dialogs[in0.path()]->hide();
            m_dialogs[in0.path()]->deleteLater();
            m_dialogs[in0.path()] = nullptr;
            m_dialogs.remove(in0.path());
        }
        m_dialogs[in0.path()] = dialog;
        if (!dialog->isVisible()) {
            dialog->exec();
            QMetaObject::invokeMethod(dialog, "deleteLater", Qt::QueuedConnection);
        }
    });

    connect(m_bluetoothInter, &DBusBluetooth::DisplayPinCode, this, [ = ] (const QDBusObjectPath &in0, const QString &in1) {
        qDebug() << "request display pincode: " << in0.path() << in1;

        PinCodeDialog *dialog = PinCodeDialog::instance(in1, false);
        m_dialogs[in0.path()] = dialog;
        if (!dialog->isVisible()) {
            dialog->exec();
            QMetaObject::invokeMethod(dialog, "deleteLater", Qt::QueuedConnection);
        }
    });

    connect(m_bluetoothInter, &DBusBluetooth::TransportableChanged, m_model, &BluetoothModel::setTransportable);
    connect(m_bluetoothInter, &DBusBluetooth::CanSendFileChanged, m_model, &BluetoothModel::setCanSendFile);

    /*FIXME
     * 这里关联的信号有时候收不到是因为 qt-dbus-factory 中的 changed 的信号有时候会发不出来，
     * qt-dbus-factory 中的 DBusExtendedAbstractInterface::internalPropGet 在同步调用情况下，会将缓存中的数据写入属性中，
     * 导致后面 onPropertyChanged 中的判断认为属性值没变，就没有发出 changed 信号。
     * 建议：前端仅在初始化时主动获取一次 dbus 中的值存储在成员变量中，并建立 changed 信号连接，后面所有用到那个值的地方，均获取成员变量;
    */

#if 0
//    connect(m_bluetoothInter, &DBusBluetooth::DisplaySwitchChanged, m_model, &BluetoothModel::setDisplaySwitch);
#else
    QDBusConnection::sessionBus().connect("com.deepin.daemon.Bluetooth",
                                          "/com/deepin/daemon/Bluetooth",
                                          "org.freedesktop.DBus.Properties",
                                          "PropertiesChanged",
                                          "sa{sv}as",
                                          this, SLOT(handleDbusSignal(QDBusMessage)));
#endif

    m_model->setTransportable(m_bluetoothInter->transportable());
    m_model->setCanSendFile(m_bluetoothInter->canSendFile());
    m_model->setDisplaySwitch(m_bluetoothInter->displaySwitch());

    connect(m_airPlaneModeInter, &DBusAirplaneMode::EnabledChanged, m_model, &BluetoothModel::setAirplaneEnable);
    m_model->setAirplaneEnable(m_airPlaneModeInter->enabled());

    m_bluetoothInter->setSync(sync);
    m_airPlaneModeInter->setSync(sync);

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

void BluetoothWorker::onStateChanged(uint state)
{
    //当蓝牙状态由0变成大于0时，强制刷新蓝牙列表
    if (!m_state && state > 0)
        refresh(true);

    m_state = state;
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
    if (!adapter) {
        return;
    }

    connect(m_powerSwitchTimer, &QTimer::timeout, adapter, &Adapter::loadStatus);
    m_powerSwitchTimer->start();

    connect(adapter, &Adapter::poweredChanged, [=]() {
        int dealyTime = 0;
        if (!powered) {
            dealyTime = 150;
        }
        QTimer::singleShot(dealyTime, this, [=] {
            if (powered == adapter->powered()) {
                Q_EMIT m_model->adpaterPowerChanged(adapter->powered());
                m_powerSwitchTimer->stop();
                disconnect(m_powerSwitchTimer, &QTimer::timeout, adapter, &Adapter::loadStatus);
            }
        });
    });

    QDBusObjectPath path(adapter->id());
    // 关闭蓝牙之前删除历史蓝牙设备列表，确保完全是删除后再设置开关
    if (!powered) {
        QDBusPendingCall clearUnpairedCall = m_bluetoothInter->ClearUnpairedDevice();
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(clearUnpairedCall, this);
        connect(watcher, &QDBusPendingCallWatcher::finished, [ = ] {
            if (clearUnpairedCall.isError()) {
                qWarning() << clearUnpairedCall.error().message();
            } else {
                m_bluetoothInter->SetAdapterPowered(path, false);
            }
        });

        adapter->loadStatus();
    } else {
        QDBusPendingCall adapterPoweredOnCall  = m_bluetoothInter->SetAdapterPowered(path, true);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(adapterPoweredOnCall, this);
        connect(watcher, &QDBusPendingCallWatcher::finished, [adapterPoweredOnCall, adapter] {
            if (adapterPoweredOnCall.isError()) {
                qWarning() << adapterPoweredOnCall.error().message();
                Q_EMIT adapter->poweredChanged(adapter->powered(), adapter->discovering());
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
    //m_bluetoothInter->ClearUnpairedDevice();
    qDebug() << "ignore device: " << device->name();
}

void BluetoothWorker::connectDevice(const Device *device, const Adapter *adapter)
{
    if (m_connectingAudioDevice && device->deviceType() == "pheadset") {
            return;
    }
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

    if (icon == "audio-card") {
        m_connectingAudioDevice = (Device::StateAvailable == state);
    }

    // FIXME: If the name and alias of the Bluetooth device are both empty, it will not be updated by default.
    // To solve the problem of blank device name display.
    if (alias.isEmpty() && name.isEmpty())
        return ;

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
        adapter = nullptr;
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

void BluetoothWorker::setDisplaySwitch(const bool &on)
{
    m_bluetoothInter->setDisplaySwitch(on);
}

void BluetoothWorker::handleDbusSignal(QDBusMessage mes)
{
    QList<QVariant> arguments = mes.arguments();
    if (3 != arguments.count()) {
        return;
    }
    QString interfaceName = mes.arguments().at(0).toString();
    if (interfaceName == "com.deepin.daemon.Bluetooth") {
        QVariantMap changedProps = qdbus_cast<QVariantMap>(arguments.at(1).value<QDBusArgument>());
        QStringList keys = changedProps.keys();
        for (int i = 0; i < keys.size(); i++) {
            if (keys.at(i) == "DisplaySwitch") {
                bool state = static_cast<bool>(changedProps.value(keys.at(i)).toBool());
                m_model->setDisplaySwitch(state);
                return;
            }
        }
    }
}

} // namespace bluetooth
} // namespace dcc
