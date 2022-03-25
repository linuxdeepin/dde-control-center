/*
* Copyright (C) 2021 ~ 2023 Deepin Technology Co., Ltd.
*
* Author:     caixiangrong <caixiangrong@uniontech.com>
*
* Maintainer: caixiangrong <caixiangrong@uniontech.com>
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
#include "bluetoothdbusproxy.h"
#include <QMetaObject>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingReply>

const static QString BluetoothService = "com.deepin.daemon.Bluetooth";
const static QString BluetoothPath = "/com/deepin/daemon/Bluetooth";
const static QString BluetoothInterface = "com.deepin.daemon.Bluetooth";

const static QString AirPlaneModeService = "com.deepin.daemon.AirplaneMode";
const static QString AirPlaneModePath = "/com/deepin/daemon/AirplaneMode";
const static QString AirPlaneModeInterface = "com.deepin.daemon.AirplaneMode";

const static QString PropertiesInterface = "org.freedesktop.DBus.Properties";
const static QString PropertiesChanged = "PropertiesChanged";

BluetoothDBusProxy::BluetoothDBusProxy(QObject *parent)
    : QObject(parent)
    , m_bluetoothInter(new QDBusInterface(BluetoothService, BluetoothPath, BluetoothInterface, QDBusConnection::sessionBus(), this))
    , m_airPlaneModeInter(new QDBusInterface(AirPlaneModeService, AirPlaneModePath, AirPlaneModeInterface, QDBusConnection::systemBus(), this))
{
    QDBusConnection::sessionBus().connect(BluetoothService, BluetoothPath, PropertiesInterface, PropertiesChanged, this, SLOT(onPropertiesChanged(QDBusMessage)));
    QDBusConnection::systemBus().connect(AirPlaneModeService, AirPlaneModePath, PropertiesInterface, PropertiesChanged, this, SLOT(onPropertiesChanged(QDBusMessage)));

    m_bluetoothInter->connect(m_bluetoothInter, SIGNAL(AdapterAdded(const QString &)), this, SIGNAL(AdapterAdded(const QString &)));
    m_bluetoothInter->connect(m_bluetoothInter, SIGNAL(AdapterRemoved(const QString &)), this, SIGNAL(AdapterRemoved(const QString &)));
    m_bluetoothInter->connect(m_bluetoothInter, SIGNAL(AdapterPropertiesChanged(const QString &)), this, SIGNAL(AdapterPropertiesChanged(const QString &)));
    m_bluetoothInter->connect(m_bluetoothInter, SIGNAL(DeviceAdded(const QString &)), this, SIGNAL(DeviceAdded(const QString &)));
    m_bluetoothInter->connect(m_bluetoothInter, SIGNAL(DeviceRemoved(const QString &)), this, SIGNAL(DeviceRemoved(const QString &)));
    m_bluetoothInter->connect(m_bluetoothInter, SIGNAL(DevicePropertiesChanged(const QString &)), this, SIGNAL(DevicePropertiesChanged(const QString &)));
    m_bluetoothInter->connect(m_bluetoothInter, SIGNAL(Cancelled(const QDBusObjectPath &)), this, SIGNAL(Cancelled(const QDBusObjectPath &)));
    m_bluetoothInter->connect(m_bluetoothInter, SIGNAL(RequestAuthorization(const QDBusObjectPath &)), this, SIGNAL(RequestAuthorization(const QDBusObjectPath &)));
    m_bluetoothInter->connect(m_bluetoothInter, SIGNAL(RequestConfirmation(const QDBusObjectPath &, const QString &)), this, SIGNAL(RequestConfirmation(const QDBusObjectPath &, const QString &)));
    m_bluetoothInter->connect(m_bluetoothInter, SIGNAL(RequestPasskey(const QDBusObjectPath &)), this, SIGNAL(RequestPasskey(const QDBusObjectPath &)));
    m_bluetoothInter->connect(m_bluetoothInter, SIGNAL(RequestPinCode(const QDBusObjectPath &)), this, SIGNAL(RequestPinCode(const QDBusObjectPath &)));
    m_bluetoothInter->connect(m_bluetoothInter, SIGNAL(DisplayPasskey(const QDBusObjectPath &, uint, uint)), this, SIGNAL(DisplayPasskey(const QDBusObjectPath &, uint, uint)));
    m_bluetoothInter->connect(m_bluetoothInter, SIGNAL(DisplayPinCode(const QDBusObjectPath &, const QString &)), this, SIGNAL(DisplayPinCode(const QDBusObjectPath &, const QString &)));
}

void BluetoothDBusProxy::onPropertiesChanged(const QDBusMessage &message)
{
    QVariantMap changedProps = qdbus_cast<QVariantMap>(message.arguments().at(1).value<QDBusArgument>());
    for (QVariantMap::const_iterator it = changedProps.begin(); it != changedProps.end(); ++it) {
        QMetaObject::invokeMethod(this, it.key().toLatin1() + "Changed", Qt::DirectConnection, QGenericArgument(it.value().typeName(), it.value().data()));
    }
}

void BluetoothDBusProxy::showBluetoothTransDialog(const QString &address, const QStringList &files)
{
    QDBusInterface inter("com.deepin.filemanager.filedialog", "/com/deepin/filemanager/filedialogmanager", "com.deepin.filemanager.filedialogmanager", QDBusConnection::sessionBus());
    inter.asyncCall("showBluetoothTransDialog", address, files);
}

bool BluetoothDBusProxy::bluetoothIsValid()
{
    return m_bluetoothInter->isValid();
}
// Bluetooth PROPERTY
uint BluetoothDBusProxy::state()
{
    return qvariant_cast<uint>(m_bluetoothInter->property("State"));
}

bool BluetoothDBusProxy::transportable()
{
    return qvariant_cast<bool>(m_bluetoothInter->property("Transportable"));
}

bool BluetoothDBusProxy::canSendFile()
{
    return qvariant_cast<bool>(m_bluetoothInter->property("CanSendFile"));
}

bool BluetoothDBusProxy::displaySwitch()
{
    return qvariant_cast<bool>(m_bluetoothInter->property("DisplaySwitch"));
}

void BluetoothDBusProxy::setDisplaySwitch(bool value)
{
    m_bluetoothInter->setProperty("DisplaySwitch", QVariant::fromValue(value));
}
// AirplaneMode PROPERTY
bool BluetoothDBusProxy::enabled()
{
    return qvariant_cast<bool>(m_airPlaneModeInter->property("Enabled"));
}

void BluetoothDBusProxy::ClearUnpairedDevice()
{
    m_bluetoothInter->asyncCall(QStringLiteral("ClearUnpairedDevice"));
}

void BluetoothDBusProxy::ClearUnpairedDevice(QObject *receiver, const char *member)
{
    QList<QVariant> argumentList;
    m_bluetoothInter->callWithCallback(QStringLiteral("ClearUnpairedDevice"), argumentList, receiver, member);
}

void BluetoothDBusProxy::SetAdapterPowered(const QDBusObjectPath &adapter, bool powered)
{
    m_bluetoothInter->asyncCall(QStringLiteral("SetAdapterPowered"), QVariant::fromValue(adapter), QVariant::fromValue(powered));
}

void BluetoothDBusProxy::SetAdapterPowered(const QDBusObjectPath &adapter, bool powered, QObject *receiver, const char *member, const char *errorSlot)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(adapter);
    argumentList << QVariant::fromValue(powered);
    m_bluetoothInter->callWithCallback(QStringLiteral("SetAdapterPowered"), argumentList, receiver, member, errorSlot);
}

void BluetoothDBusProxy::DisconnectDevice(const QDBusObjectPath &device)
{
    m_bluetoothInter->asyncCall(QStringLiteral("DisconnectDevice"), QVariant::fromValue(device));
}

void BluetoothDBusProxy::RemoveDevice(const QDBusObjectPath &adapter, const QDBusObjectPath &device)
{
    m_bluetoothInter->asyncCall(QStringLiteral("RemoveDevice"), QVariant::fromValue(adapter), QVariant::fromValue(device));
}

void BluetoothDBusProxy::ConnectDevice(const QDBusObjectPath &device, const QDBusObjectPath &adapter)
{
    m_bluetoothInter->asyncCall(QStringLiteral("ConnectDevice"), QVariant::fromValue(device), QVariant::fromValue(adapter));
}

QString BluetoothDBusProxy::GetDevices(const QDBusObjectPath &adapter)
{
    return QDBusPendingReply<QString>(m_bluetoothInter->asyncCall(QStringLiteral("GetDevices"), QVariant::fromValue(adapter)));
}

void BluetoothDBusProxy::GetDevices(const QDBusObjectPath &adapter, QObject *receiver, const char *member)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(adapter);
    m_bluetoothInter->callWithCallback(QStringLiteral("GetDevices"), argumentList, receiver, member);
}

QString BluetoothDBusProxy::GetAdapters()
{
    return QDBusPendingReply<QString>(m_bluetoothInter->asyncCall(QStringLiteral("GetAdapters")));
}

void BluetoothDBusProxy::SetAdapterAlias(const QDBusObjectPath &adapter, const QString &alias)
{
    m_bluetoothInter->asyncCall(QStringLiteral("SetAdapterAlias"), QVariant::fromValue(adapter), QVariant::fromValue(alias));
}

void BluetoothDBusProxy::SetDeviceAlias(const QDBusObjectPath &device, const QString &alias)
{
    m_bluetoothInter->asyncCall(QStringLiteral("SetDeviceAlias"), QVariant::fromValue(device), QVariant::fromValue(alias));
}

void BluetoothDBusProxy::RequestDiscovery(const QDBusObjectPath &adapter)
{
    m_bluetoothInter->asyncCall(QStringLiteral("RequestDiscovery"), QVariant::fromValue(adapter));
}

void BluetoothDBusProxy::Confirm(const QDBusObjectPath &device, bool accept)
{
    m_bluetoothInter->asyncCall(QStringLiteral("Confirm"), QVariant::fromValue(device), QVariant::fromValue(accept));
}

void BluetoothDBusProxy::SetAdapterDiscovering(const QDBusObjectPath &adapter, bool discovering)
{
    m_bluetoothInter->asyncCall(QStringLiteral("SetAdapterDiscovering"), QVariant::fromValue(adapter), QVariant::fromValue(discovering));
}

void BluetoothDBusProxy::SetAdapterDiscoverable(const QDBusObjectPath &adapter, bool discoverable)
{
    m_bluetoothInter->asyncCall(QStringLiteral("SetAdapterDiscoverable"), QVariant::fromValue(adapter), QVariant::fromValue(discoverable));
}
