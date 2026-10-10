// SPDX-FileCopyrightText: 2020 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "wirelesscastingmodel.h"

#include <ddbusinterface.h>

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDebug>

const QString dbusService = "com.deepin.Cooperation.NetworkDisplay";
const QString dbusPath = "/com/deepin/Cooperation/NetworkDisplay";
const QString dbusInterface = "com.deepin.Cooperation.NetworkDisplay";
const QString sinkInterface = "com.deepin.Cooperation.NetworkDisplay.Sink";

DCORE_USE_NAMESPACE

WirelessCastingModel::WirelessCastingModel(QObject *parent)
    : QObject(parent)
    , m_dbus(new DDBusInterface(dbusService, dbusPath, dbusInterface, QDBusConnection::sessionBus(), this))
    , m_dbusNM(nullptr)
    , m_state(NoMonitor)
    , m_curConnected(nullptr)
    , m_wirelessDevCheck(false)
    , m_wirelessEnabled(false)
    , m_needRefresh(false)
    , m_connecting(false)
    , m_hasMultiscreens(false)
{
#ifdef WIRELESS_CASTING_ENABLED
    checkServiceAvailable();
    connect(this, &WirelessCastingModel::serviceAvailableChanged, this, [this](bool available) {
        if (!available)
            return;

        prepareDbus();
        initData();
        startTimer(std::chrono::seconds (30));
    });
#endif
}

WirelessCastingModel::~WirelessCastingModel()
{
    m_dbus->deleteLater();
}

void WirelessCastingModel::refresh()
{
    if (!m_needRefresh || !m_serviceAvailable) {
        return;
    }
    QDBusMessage const reply = m_dbus->call("Refresh");
    if (reply.type() == QDBusMessage::ReplyMessage) {
        qInfo() << reply.arguments().value(0);
    } else {
        qWarning() << "Method call failed. Error:" << reply.errorMessage();
    }
}

void WirelessCastingModel::disconnectMonitor()
{
    if (nullptr != m_curConnected)
        m_curConnected->disconnectMonitor();
}

const QString WirelessCastingModel::curMonitorName()
{
    if (nullptr != m_curConnected)
        return m_curConnected->name();
    else
        return QString();
}

void WirelessCastingModel::initData()
{
    QVariant var = m_dbus->property("SinkList");
    if (!var.isValid())
        return;
    updateSinkList(var);
    var = m_dbus->property("MissingCapabilities");
    if (!var.isValid())
        return;
    updateWarningInfo(var);
    connect(this, &WirelessCastingModel::SinkListChanged, this, [this](const QList<QDBusObjectPath> &sinkList) {
        if (m_needRefresh)
            updateSinkList(QVariant::fromValue(sinkList));
    });
    connect(this, &WirelessCastingModel::MissingCapabilitiesChanged, this, [this](const QStringList &missingList) {
        updateWarningInfo(missingList);
    });
}

void WirelessCastingModel::updateSinkList(const QVariant &sinkList)
{
    QList<QDBusObjectPath> const list = qdbus_cast<QList<QDBusObjectPath>>(sinkList);

    QStringList tmpList;

    bool hasConnected = false;

    foreach (const QDBusObjectPath &var, list) {
        QString const path = var.path();
        tmpList.append(path);
        if (!m_monitors.contains(path)) {
            Monitor *mon = new Monitor(path);
            m_monitors.insert(path, mon);
            connect(mon, &Monitor::stateChanged, this, &WirelessCastingModel::handleMonitorStateChanged);
            connect(mon, &Monitor::stateChangeFinished, this, &WirelessCastingModel::stateChangeFinished);
            Q_EMIT addMonitor(path, mon);
        }
    }

    auto it = m_monitors.begin();
    while (it != m_monitors.end()) {
        if (tmpList.contains(it.key())) {
            if (it.value()->state() == Monitor::ND_SINK_STATE_STREAMING) {
                m_curConnected = it.value();
                hasConnected = true;
            }
            ++it;
            continue;
        }
        disconnect(it.value(), &Monitor::stateChanged, this, &WirelessCastingModel::handleMonitorStateChanged);
        disconnect(it.value(), &Monitor::stateChangeFinished, this, &WirelessCastingModel::stateChangeFinished);
        Q_EMIT removeMonitor(it.key());
        it.value()->deleteLater();
        it = m_monitors.erase(it);
    }

    if (!hasConnected)
        m_curConnected = nullptr;
    checkState();
}

void WirelessCastingModel::updateWarningInfo(const QVariant &var)
{
    Q_UNUSED(var)
    checkState();
}

void WirelessCastingModel::setState(CastingState state)
{
    if (m_state != state) {
        m_state = state;
        Q_EMIT stateChanged(m_state);
    }
}

void WirelessCastingModel::checkState()
{
    // 判断无线设备是否存在
    if (!m_wirelessDevCheck || !m_serviceAvailable) {
        setState(NoWirelessDevice);
        return;
    }

    // 判断无线设备是否缺失必要能力
    auto var = m_dbus->property("MissingCapabilities");
    if (!var.isValid())
        return;
    if (!var.toStringList().isEmpty()) {
        if (var.toStringList().contains("NotSupportP2P", Qt::CaseInsensitive)) {
            setState(NotSupportP2P);
        } else {
            // 判断无线设备是否启用
            if (!m_wirelessEnabled) {
                setState(DisabledWirelessDevice);
            } else {
                setState(WarningInfo);
            }
        }
        return;
    } else {
        if (!m_wirelessEnabled) {
            setState(DisabledWirelessDevice);
            return;
        }
    }

    // 正常显示：已连接、设备列表或无设备
    if (nullptr == m_curConnected) {
        if (m_monitors.empty()) {
            if (NoMonitor != m_state) {
                setState(NoMonitor);
            }
        } else {
            if (List != m_state) {
                setState(List);
            }
        }
    } else {
        setState(Connected);
    }
}

void WirelessCastingModel::prepareDbus()
{
    auto checkWirelessDev = [this]() {
        m_wirelessDevCheck = false;
        m_wirelessEnabled = false;
        // 获取设备列表
        QList<QDBusObjectPath> const devices = qdbus_cast<QList<QDBusObjectPath>>(m_dbusNM->property("AllDevices"));

        // 先检查是否有无线设备，有无线设备的情况再检查无线设备是否启用
        // 遍历设备列表并检查是否有无线网卡设备
        foreach (const QDBusObjectPath &devicePath, devices) {
            DDBusInterface deviceInterface("org.freedesktop.NetworkManager", devicePath.path(), "org.freedesktop.NetworkManager.Device", QDBusConnection::systemBus());

            // 获取设备类型
            auto reply = deviceInterface.property("DeviceType");
            if (reply.isValid() && reply.toUInt() == 2) {
                m_wirelessDevCheck = true;
                break;
            }
        }
        // 检查设备是否启用
        if (m_wirelessDevCheck) {
            DDBusInterface deviceInterface("org.freedesktop.NetworkManager", "/org/freedesktop/NetworkManager", "org.freedesktop.NetworkManager", QDBusConnection::systemBus());
            auto reply = deviceInterface.property("WirelessEnabled");
            if (reply.isValid() && reply.toBool()) {
                m_wirelessEnabled = true;
            }
        }
        checkState();
    };

    // 创建 NetworkManager D-Bus 接口
    m_dbusNM = new DDBusInterface("org.freedesktop.NetworkManager", "/org/freedesktop/NetworkManager", "org.freedesktop.NetworkManager", QDBusConnection::systemBus(), this);

    QDBusConnection::sessionBus().connect("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "NameOwnerChanged", this, SLOT(onDBusNameOwnerChanged(QString, QString, QString)));

    checkWirelessDev();
    // 设置监听事件
    connect(this, &WirelessCastingModel::DeviceEnabled, this, [=](const QString &, bool) {
        checkWirelessDev();
    });
    connect(this, &WirelessCastingModel::AllDevicesChanged, this, [=](const QList<QDBusObjectPath> &devList) {
        Q_UNUSED(devList)
        checkWirelessDev();
    });
    connect(this, &WirelessCastingModel::WirelessEnabledChanged, this, [=](bool) {
        checkWirelessDev();
    });
}

void WirelessCastingModel::onDBusNameOwnerChanged(const QString &name, const QString &oldOwner, const QString &newOwner)
{
    Q_UNUSED(oldOwner)
    if ("com.deepin.Cooperation.NetworkDisplay" == name && !newOwner.isEmpty()) {
        resetNetworkDisplayData();
        checkState();
    }
    // 不做重启后端进程的操作,如果需要处理该异常,在此处对前端进行处理
}

void WirelessCastingModel::resetNetworkDisplayData()
{
    if (m_dbus) {
        delete m_dbus;
        m_dbus = nullptr;
    }
    auto it = m_monitors.begin();
    while (it != m_monitors.end()) {
        disconnect(it.value(), &Monitor::stateChanged, this, &WirelessCastingModel::handleMonitorStateChanged);
        disconnect(it.value(), &Monitor::stateChangeFinished, this, &WirelessCastingModel::stateChangeFinished);
        Q_EMIT removeMonitor(it.key());
        it.value()->deleteLater();
        it = m_monitors.erase(it);
    }
    m_dbus = new DDBusInterface(dbusService, dbusPath, dbusInterface, QDBusConnection::sessionBus(), this);
    if (!m_serviceAvailable) {
        checkServiceAvailable();
    } else {
        initData();
    }
}

void WirelessCastingModel::handleMonitorStateChanged(const Monitor::NdSinkState state)
{
    setConnectState(false);
    switch (state) {
    case Monitor::ND_SINK_STATE_DISCONNECTED:
    case Monitor::ND_SINK_STATE_ERROR:
        if (sender() == m_curConnected)
            m_curConnected = nullptr;
        break;
    case Monitor::ND_SINK_STATE_WAIT_P2P:
    case Monitor::ND_SINK_STATE_WAIT_SOCKET:
    case Monitor::ND_SINK_STATE_WAIT_STREAMING:
        setConnectState(true);
        break;
    case Monitor::ND_SINK_STATE_STREAMING:
        if (sender() != m_curConnected)
            m_curConnected = qobject_cast<Monitor *>(sender());
        break;
    default:
        break;
    }
    checkState();
}

void WirelessCastingModel::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
    refresh();
}

void WirelessCastingModel::enableRefresh(bool enable)
{
    if (!m_serviceAvailable)
        return;

    m_needRefresh = enable;
    refresh();
    if (enable) {
        QVariant var = m_dbus->property("SinkList");
        if (!var.isValid())
            return;
        updateSinkList(var);
    }
}

void WirelessCastingModel::setConnectState(bool connecting)
{
    if (m_connecting != connecting) {
        m_connecting = connecting;
        Q_EMIT connectStateChanged(m_connecting);
    }
}

void WirelessCastingModel::checkServiceAvailable()
{
    QDBusMessage message = QDBusMessage::createMethodCall(dbusService, dbusPath, dbusInterface, "Refresh");

    // 3s timeout
    QDBusPendingCall pendingCall = QDBusConnection::sessionBus().asyncCall(message, 3000);
    auto *watcher = new QDBusPendingCallWatcher(pendingCall);
    connect(watcher, &QDBusPendingCallWatcher::finished, [watcher, this](const QDBusPendingCallWatcher *call) {
        bool available = true;
        if (call->isError()) {
            available = false;
            qWarning() << "DBus call timeout or has error. Service may not be available:" << dbusService << " error:" << call->error().message();
        }

        if (m_serviceAvailable != available) {
            m_serviceAvailable = available;
            Q_EMIT serviceAvailableChanged(m_serviceAvailable);
        }

        watcher->deleteLater();
    });
}

void WirelessCastingModel::setMultiscreensFlag(bool hasMultiscreens)
{
    if (m_hasMultiscreens != hasMultiscreens) {
        m_hasMultiscreens = hasMultiscreens;
        Q_EMIT multiscreensFlagChanged(m_hasMultiscreens);
    }
}

Monitor::Monitor(const QString &monitorPath, QObject *parent)
    : QObject(parent)
    , m_dbus(nullptr)
    , m_state(ND_SINK_STATE_DISCONNECTED)
    , m_strength(0)
{
    m_dbus = new DDBusInterface(dbusService, monitorPath, sinkInterface, QDBusConnection::sessionBus(), this);

    initData();
}

Monitor::~Monitor()
{
    disconnect();
    m_dbus->deleteLater();
}

void Monitor::connMonitor()
{
    QDBusMessage reply = m_dbus->call("Connect");
    if (reply.type() == QDBusMessage::ReplyMessage) {
        qInfo() << reply.arguments().value(0);
    } else {
        qWarning() << "Method call failed. Error:" << reply.errorMessage();
    }
}

void Monitor::disconnectMonitor()
{
    QDBusMessage reply = m_dbus->call("Cancel");
    if (reply.type() == QDBusMessage::ReplyMessage) {
        qInfo() << reply.arguments().value(0);
    } else {
        qWarning() << "Method call failed. Error:" << reply.errorMessage();
    }
}

void Monitor::initData()
{
    QVariant var = m_dbus->property("Name");
    if (var.isValid())
        m_name = var.toString();

    var = m_dbus->property("HwAddress");
    if (var.isValid())
        m_hwAddress = var.toString();

    var = m_dbus->property("Status");
    if (var.isValid())
        m_state = static_cast<NdSinkState>(var.toUInt());

    var = m_dbus->property("Strength");
    if (var.isValid())
        m_strength = var.toUInt();

    connect(this, &Monitor::StatusChanged, this, [this](int status) {
        qInfo() << "Monitor" << m_name << "status changed to " << status;
        checkState(status);
    });
}

void Monitor::checkState(const QVariant &var)
{
    if (!var.isValid())
        return;
    auto const state = static_cast<NdSinkState>(var.toUInt());
    if (state != m_state) {
        m_state = state;
        Q_EMIT stateChanged(m_state);
    }
}
