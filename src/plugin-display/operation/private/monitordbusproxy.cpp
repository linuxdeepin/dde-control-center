//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "monitordbusproxy.h"

#include <QMetaObject>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QDBusMetaType>
#include <QDebug>

const static QString MonitorService = "org.deepin.dde.Display1";
const static QString MonitorInterface = "org.deepin.dde.Display1.Monitor";

const static QString PropertiesInterface = "org.freedesktop.DBus.Properties";
const static QString PropertiesChanged = "PropertiesChanged";

DCORE_USE_NAMESPACE

MonitorDBusProxy::MonitorDBusProxy(QString monitorPath, QObject *parent)
    : QObject(parent)
    , m_monitorUserPath(monitorPath)
{
    registerResolutionMetaType();
    registerReflectListMetaType();
    registerRotationListMetaType();
    registerResolutionListMetaType();

    init();
}

void MonitorDBusProxy::init()
{
    m_dBusMonitorInter = new DDBusInterface(MonitorService, m_monitorUserPath, MonitorInterface, QDBusConnection::sessionBus(), this);
    m_dBusMonitorPropertiesInter = new DDBusInterface(MonitorService, m_monitorUserPath, PropertiesInterface, QDBusConnection::sessionBus(), this);
    QDBusConnection dbusConnection = m_dBusMonitorInter->connection();
    dbusConnection.connect(MonitorService, m_monitorUserPath, PropertiesInterface, PropertiesChanged, this, SLOT(onPropertiesChanged(QDBusMessage)));
}

QStringList MonitorDBusProxy::availableFillModes()
{
    return qvariant_cast<QStringList>(m_dBusMonitorInter->property("AvailableFillModes"));
}

Resolution MonitorDBusProxy::bestMode()
{
    Resolution val;
    m_dBusMonitorPropertiesInter->call("Get", MonitorInterface, "BestMode").arguments().at(0).value<QDBusVariant>().variant().value<QDBusArgument>() >> val;
    return val;
}

bool MonitorDBusProxy::connected()
{
    return qvariant_cast<bool>(m_dBusMonitorInter->property("Connected"));
}

QString MonitorDBusProxy::currentFillMode()
{
    return qvariant_cast<QString>(m_dBusMonitorInter->property("CurrentFillMode"));
}

void MonitorDBusProxy::setCurrentFillMode(const QString &value)
{
    m_dBusMonitorInter->setProperty("CurrentFillMode", QVariant::fromValue(value));
}

Resolution MonitorDBusProxy::currentMode()
{
    Resolution val;
    m_dBusMonitorPropertiesInter->call("Get", MonitorInterface, "CurrentMode").arguments().at(0).value<QDBusVariant>().variant().value<QDBusArgument>() >> val;
    return val;
}

uchar MonitorDBusProxy::currentRotateMode()
{
    return qvariant_cast<uchar>(m_dBusMonitorInter->property("CurrentRotateMode"));
}

bool MonitorDBusProxy::enabled()
{
    return qvariant_cast<bool>(m_dBusMonitorInter->property("Enabled"));
}

ushort MonitorDBusProxy::height()
{
    return qvariant_cast<ushort>(m_dBusMonitorInter->property("Height"));
}

QString MonitorDBusProxy::manufacturer()
{
    return qvariant_cast<QString>(m_dBusMonitorInter->property("Manufacturer"));
}

uint MonitorDBusProxy::mmHeight()
{
    return qvariant_cast<uint>(m_dBusMonitorInter->property("MmHeight"));
}

uint MonitorDBusProxy::mmWidth()
{
    return qvariant_cast<uint>(m_dBusMonitorInter->property("MmWidth"));
}

QString MonitorDBusProxy::model()
{
    return qvariant_cast<QString>(m_dBusMonitorInter->property("Model"));
}

ResolutionList MonitorDBusProxy::modes()
{
    ResolutionList val;
    m_dBusMonitorPropertiesInter->call("Get", MonitorInterface, "Modes").arguments().at(0).value<QDBusVariant>().variant().value<QDBusArgument>() >> val;
    return val;
}

QString MonitorDBusProxy::name()
{
    return qvariant_cast<QString>(m_dBusMonitorInter->property("Name"));
}

ushort MonitorDBusProxy::reflect()
{
    return qvariant_cast<ushort>(m_dBusMonitorInter->property("Reflect"));
}

ReflectList MonitorDBusProxy::reflects()
{
    return qvariant_cast<ReflectList>(m_dBusMonitorPropertiesInter->call("Get", MonitorInterface, "Reflects").arguments().at(0).value<QDBusVariant>().variant());
}

double MonitorDBusProxy::refreshRate()
{
    return qvariant_cast<double>(m_dBusMonitorInter->property("RefreshRate"));
}

ushort MonitorDBusProxy::rotation()
{
    return qvariant_cast<ushort>(m_dBusMonitorInter->property("Rotation"));
}

RotationList MonitorDBusProxy::rotations()
{
    return qvariant_cast<RotationList>(m_dBusMonitorPropertiesInter->call("Get", MonitorInterface, "Rotations").arguments().at(0).value<QDBusVariant>().variant());
}

ushort MonitorDBusProxy::width()
{
    return qvariant_cast<ushort>(m_dBusMonitorInter->property("Width"));
}

short MonitorDBusProxy::x()
{
    return qvariant_cast<short>(m_dBusMonitorInter->property("X"));
}

short MonitorDBusProxy::y()
{
    return qvariant_cast<short>(m_dBusMonitorInter->property("Y"));
}

QDBusPendingReply<> MonitorDBusProxy::Enable(bool in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusMonitorInter->asyncCallWithArgumentList(QStringLiteral("Enable"), argumentList);
}

QDBusPendingReply<> MonitorDBusProxy::SetMode(uint in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusMonitorInter->asyncCallWithArgumentList(QStringLiteral("SetMode"), argumentList);
}

QDBusPendingReply<> MonitorDBusProxy::SetModeBySize(ushort in0, ushort in1)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1);
    return m_dBusMonitorInter->asyncCallWithArgumentList(QStringLiteral("SetModeBySize"), argumentList);
}

QDBusPendingReply<> MonitorDBusProxy::SetPosition(short in0, short in1)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1);
    return m_dBusMonitorInter->asyncCallWithArgumentList(QStringLiteral("SetPosition"), argumentList);
}

QDBusPendingReply<> MonitorDBusProxy::SetReflect(ushort in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusMonitorInter->asyncCallWithArgumentList(QStringLiteral("SetReflect"), argumentList);
}

QDBusPendingReply<> MonitorDBusProxy::SetRotation(ushort in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusMonitorInter->asyncCallWithArgumentList(QStringLiteral("SetRotation"), argumentList);
}

void MonitorDBusProxy::onPropertiesChanged(const QDBusMessage &message)
{
    QVariantMap changedProps = qdbus_cast<QVariantMap>(message.arguments().at(1).value<QDBusArgument>());
    for (QVariantMap::const_iterator it = changedProps.begin(); it != changedProps.end(); ++it) {
        if(it.key() =="CurrentMode") {
            emit CurrentModeChanged(qdbus_cast<Resolution>(changedProps.value(it.key())));
        } else if(it.key() =="BestMode") {
            emit BestModeChanged(qdbus_cast<Resolution>(changedProps.value(it.key())));
        } else if(it.key() =="Modes") {
            emit ModesChanged(qdbus_cast<ResolutionList>(changedProps.value(it.key())));
        }
    }
}
