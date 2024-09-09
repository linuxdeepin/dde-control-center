// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "dccquickdbusinterface.h"

#include "dccquickdbusinterface_p.h"

#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusMetaType>
#include <QDBusPendingCall>
#include <QDBusUnixFileDescriptor>
#include <QDBusVariant>
#include <QLoggingCategory>

namespace dccV25 {
static Q_LOGGING_CATEGORY(dccLog, "dde.dcc.quickDBus");
static const QString &PropertiesInterface = QStringLiteral("org.freedesktop.DBus.Properties");
static const QString &PropertiesChanged = QStringLiteral("PropertiesChanged");

DccQuickDBusCallback::DccQuickDBusCallback(QJSValue member, QJSValue errorSlot, bool autoDelete, QObject *parent)
    : QObject(parent)
    , m_member(member)
    , m_errorSlot(errorSlot)
    , m_autoDelete(autoDelete)
{
}

DccQuickDBusCallback::~DccQuickDBusCallback() { }

static QVariant DBusBasicToValue(const QDBusArgument &arg)
{
    switch (arg.currentSignature().at(0).unicode()) {
    case 'b': {
        bool v;
        arg >> v;
        return v;
    } break;
    case 'y': {
        unsigned char v;
        arg >> v;
        return v;
    } break;
    case 'n': {
        short v;
        arg >> v;
        return v;
    } break;
    case 'q': {
        unsigned short v;
        arg >> v;
        return v;
    } break;
    case 'i': {
        int v;
        arg >> v;
        return v;
    } break;
    case 'u': {
        unsigned int v;
        arg >> v;
        return v;
    } break;
    case 'x': {
        qint64 v;
        arg >> v;
        return v;
    } break;
    case 't': {
        quint64 v;
        arg >> v;
        return v;
    } break;
    case 'd': {
        double v;
        arg >> v;
        return v;
    } break;
    case 's': {
        QString v;
        arg >> v;
        return v;
    } break;
    case 'o': {
        QDBusObjectPath v;
        arg >> v;
        return v.path();
    } break;
    case 'g': {
        QDBusSignature v;
        arg >> v;
        return v.signature();
    } break;
    case 'h': {
        QDBusUnixFileDescriptor v;
        arg >> v;
        return v.fileDescriptor();
    } break;
    case 'v': {
        QDBusVariant v;
        arg >> v;
        return v.variant();
    } break;
    default:
        qCWarning(dccLog()) << "error basic type:" << arg.currentType();
        break;
    }
    return QVariant();
}

static QVariant DBusArgumentToValue(const QDBusArgument &arg)
{
    switch (arg.currentType()) {
    case QDBusArgument::BasicType:
        return DBusBasicToValue(arg);
        break;
    case QDBusArgument::VariantType: {
        QDBusVariant v;
        arg >> v;
        return v.variant();
    } break;
    case QDBusArgument::ArrayType: {
        QList<QVariant> ret;
        arg.beginArray();
        while (!arg.atEnd()) {
            ret.append(DBusArgumentToValue(arg));
        }
        arg.endStructure();
        return ret;

    } break;
    case QDBusArgument::StructureType: {
        QVariantList ret;
        arg.beginStructure();
        while (!arg.atEnd()) {
            ret.append(DBusArgumentToValue(arg));
        }
        arg.endStructure();
        return ret;
    } break;
    case QDBusArgument::MapType: {
        QVariantMap map;
        arg.beginMap();
        while (!arg.atEnd()) {
            arg.beginMapEntry();
            QVariant key = DBusBasicToValue(arg);
            QVariant value = DBusArgumentToValue(arg);
            map.insert(key.toString(), value);
            arg.endMapEntry();
        }
        arg.endMap();
        return QVariant::fromValue(map);
    } break;
    default:
        qCWarning(dccLog()) << "error type:" << arg.currentType();
        break;
    }
    return QVariant();
}

QVariant DccQuickDBusCallback::toValue(const QVariant &value)
{
    return value.canConvert<QDBusArgument>() ? DBusArgumentToValue(qvariant_cast<QDBusArgument>(value)) : value;
}

void DccQuickDBusCallback::returnMethod(const QDBusMessage &msg)
{
    QJSValueList arglist;
    for (auto &&v : msg.arguments()) {
        arglist.append(QJSValue(QJSPrimitiveValue(DccQuickDBusCallback::toValue(v))));
    }
    if (m_member.isCallable()) {
        m_member.call(arglist);
    }
    if (m_autoDelete) {
        deleteLater();
    }
}

void DccQuickDBusCallback::errorMethod(const QDBusError &error, const QDBusMessage &)
{
    QJSValueList arglist;
    arglist.append(QJSValue(error.name() + ":" + error.message()));
    if (m_member.isCallable()) {
        m_errorSlot.call(arglist);
    }
    if (m_autoDelete) {
        deleteLater();
    }
}

DccQuickDBusInterface::Private::Private(DccQuickDBusInterface *q)
    : q_ptr(q)
    , m_connectionType(SessionBus)
    , m_connection(QDBusConnection::connectToBus(QDBusConnection::SessionBus, QString::number((quintptr)q)))
{
}

DccQuickDBusInterface::Private::~Private() { }

void DccQuickDBusInterface::Private::getAllPropertys()
{
    QDBusMessage msg = QDBusMessage::createMethodCall(m_service, m_path, PropertiesInterface, "GetAll");
    msg << m_interface;
    m_connection.callWithCallback(msg, this, SLOT(onAllProperties(QVariantMap)), SLOT(onGetPropertiesErr(QDBusError)));
}

void DccQuickDBusInterface::Private::onAllProperties(const QVariantMap &changedProperties)
{
    QVariantMap properties;
    for (QVariantMap::const_iterator it = changedProperties.cbegin(); it != changedProperties.cend(); ++it) {
        QVariant v = DccQuickDBusCallback::toValue(it.value());
        m_propertyMap.insert(it.key(), v);
        properties.insert(it.key(), v);
    }
    Q_EMIT q_ptr->propertyChanged(properties);
}

void DccQuickDBusInterface::Private::onGetPropertiesErr(const QDBusError &e)
{
    qCWarning(dccLog()) << e << m_service << m_path << m_interface;
}

void DccQuickDBusInterface::Private::onPropertiesChanged(const QString &interfaceName, const QVariantMap &changedProperties, const QStringList &invalidatedProperties)
{
    Q_UNUSED(interfaceName)
    Q_UNUSED(invalidatedProperties)
    onAllProperties(changedProperties);
}

////////////////////////////////////////////////
DccQuickDBusInterface::DccQuickDBusInterface(QObject *parent)
    : QObject(parent)
    , p_ptr(new DccQuickDBusInterface::Private(this))
{
}

DccQuickDBusInterface::~DccQuickDBusInterface()
{
    delete p_ptr;
}

QString DccQuickDBusInterface::service() const
{
    return p_ptr->m_service;
}

void DccQuickDBusInterface::setService(const QString &service)
{
    if (p_ptr->m_service != service) {
        p_ptr->m_service = service;
        Q_EMIT serviceChanged(p_ptr->m_service);
    }
}

QString DccQuickDBusInterface::path() const
{
    return p_ptr->m_path;
}

void DccQuickDBusInterface::setPath(const QString &path)
{
    if (p_ptr->m_path != path) {
        p_ptr->m_path = path;
        Q_EMIT pathChanged(p_ptr->m_path);
    }
}

QString DccQuickDBusInterface::interface() const
{
    return p_ptr->m_interface;
}

void DccQuickDBusInterface::setInterface(const QString &interface)
{
    if (p_ptr->m_interface != interface) {
        p_ptr->m_interface = interface;
        Q_EMIT interfaceChanged(p_ptr->m_interface);
    }
}

DccQuickDBusInterface::BusType DccQuickDBusInterface::connection() const
{
    return p_ptr->m_connectionType;
}

void DccQuickDBusInterface::setConnection(const BusType &connection)
{
    if (p_ptr->m_connectionType != connection) {
        p_ptr->m_connectionType = connection;
        QDBusConnection::disconnectFromBus(p_ptr->m_connection.name());
        p_ptr->m_connection = QDBusConnection::connectToBus(QDBusConnection::BusType(p_ptr->m_connectionType), p_ptr->m_connection.name());
        Q_EMIT connectionChanged(p_ptr->m_connectionType);
    }
}

bool DccQuickDBusInterface::callWithCallback(const QString &method, const QList<QVariant> &args, const QJSValue member, const QJSValue errorSlot)
{
    DccQuickDBusCallback *callback = new DccQuickDBusCallback(member, errorSlot, true, this);
    QDBusMessage msg = QDBusMessage::createMethodCall(p_ptr->m_service, p_ptr->m_path, p_ptr->m_interface, method);
    msg.setArguments(args);
    return p_ptr->m_connection.callWithCallback(msg, callback, SLOT(returnMethod(QDBusMessage)), SLOT(errorMethod(QDBusError, QDBusMessage)));
}

bool DccQuickDBusInterface::connectDbus(const QString &signature, const QJSValue slot)
{
    DccQuickDBusCallback *callback = new DccQuickDBusCallback(slot, QJSValue(), false, this);
    return p_ptr->m_connection.connect(p_ptr->m_service, p_ptr->m_path, p_ptr->m_interface, signature, callback, SLOT(returnMethod(QDBusMessage)));
}

QVariant DccQuickDBusInterface::getProperty(const QString &propname)
{
    return p_ptr->m_propertyMap.value(propname, QVariant());
}

void DccQuickDBusInterface::setProperty(const QString &propname, const QVariant &value)
{
    QDBusMessage msg = QDBusMessage::createMethodCall(p_ptr->m_service, p_ptr->m_path, PropertiesInterface, QStringLiteral("Set"));
    msg << p_ptr->m_interface << propname << QVariant::fromValue(QDBusVariant(value));
    p_ptr->m_connection.asyncCall(msg);
}

void DccQuickDBusInterface::connectNotify(const QMetaMethod &signal)
{
    if (signal.name() == "propertyChanged") {
        p_ptr->getAllPropertys();
        p_ptr->m_connection.connect(p_ptr->m_service,
                                    p_ptr->m_path,
                                    PropertiesInterface,
                                    PropertiesChanged,
                                    { p_ptr->m_interface },
                                    QString(),
                                    p_ptr,
                                    SLOT(onPropertiesChanged(QString, QVariantMap, QStringList)));
    }
    QObject::connectNotify(signal);
}

void DccQuickDBusInterface::disconnectNotify(const QMetaMethod &signal)
{
    QObject::disconnectNotify(signal);
}

} // namespace dccV25
