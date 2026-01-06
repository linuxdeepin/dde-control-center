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
#include <QJSPrimitiveValue>
#include <QLoggingCategory>

namespace dccV25 {
static Q_LOGGING_CATEGORY(dccLog, "dde.dcc.quickDBus");
static const QString &PropertiesInterface = QStringLiteral("org.freedesktop.DBus.Properties");
static const QString &PropertiesChanged = QStringLiteral("PropertiesChanged");

DccQuickDBusCallback::DccQuickDBusCallback(QJSValue member, QJSValue errorSlot, QObject *parent)
    : QObject(parent)
    , m_member(member)
    , m_errorSlot(errorSlot)
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
    deleteLater();
}

void DccQuickDBusCallback::errorMethod(const QDBusError &error, const QDBusMessage &)
{
    QJSValueList arglist;
    arglist.append(QJSValue(error.name() + ":" + error.message()));
    if (m_member.isCallable()) {
        m_errorSlot.call(arglist);
    }
    deleteLater();
}

DccDBusSignalCallback::DccDBusSignalCallback(const QMetaMethod &method, QObject *parent)
    : QObject(parent)
    , m_method(method)
    , m_target(parent)
{
}

DccDBusSignalCallback::~DccDBusSignalCallback() { }

void DccDBusSignalCallback::returnMethod(const QDBusMessage &msg)
{
    QVariantList arglist(m_method.parameterCount());
    int i = 0;
    for (auto &&v : msg.arguments()) {
        arglist[i] = DccQuickDBusCallback::toValue(v);
        i++;
        if (i > arglist.size()) {
            break;
        }
    }

    switch (arglist.size()) {
    case 0:
        m_method.invoke(m_target);
        break;
    case 1:
        m_method.invoke(m_target, arglist.at(0));
        break;
    case 2:
        m_method.invoke(m_target, arglist.at(0), arglist.at(1));
        break;
    case 3:
        m_method.invoke(m_target, arglist.at(0), arglist.at(1), arglist.at(2));
        break;
    case 4:
        m_method.invoke(m_target, arglist.at(0), arglist.at(1), arglist.at(2), arglist.at(3));
        break;
    case 5:
        m_method.invoke(m_target, arglist.at(0), arglist.at(1), arglist.at(2), arglist.at(3), arglist.at(4));
        break;
    case 6:
        m_method.invoke(m_target, arglist.at(0), arglist.at(1), arglist.at(2), arglist.at(3), arglist.at(4), arglist.at(5));
        break;
    case 7:
        m_method.invoke(m_target, arglist.at(0), arglist.at(1), arglist.at(2), arglist.at(3), arglist.at(4), arglist.at(5), arglist.at(6));
        break;
    case 8:
        m_method.invoke(m_target, arglist.at(0), arglist.at(1), arglist.at(2), arglist.at(3), arglist.at(4), arglist.at(5), arglist.at(6), arglist.at(7));
        break;
    default:
        m_method.invoke(m_target, arglist.at(0), arglist.at(1), arglist.at(2), arglist.at(3), arglist.at(4), arglist.at(5), arglist.at(6), arglist.at(7), arglist.at(8));
        break;
    }
}

class DBusPropertySignalSpy : public QObject
{
public:
    typedef std::function<void(const char *name, const QVariant &value)> PropertyCallbackFunc;

    explicit DBusPropertySignalSpy(PropertyCallbackFunc fun, QObject *parent = nullptr)
        : QObject(parent)
        , m_propertyCallbackFunc(fun)
    {
    }

    int qt_metacall(QMetaObject::Call c, int id, void **arguments) override
    {
        QObject *target = sender();
        int signalIndex = id;
        id = QObject::qt_metacall(c, id, arguments);
        if (id < 0 || c != QMetaObject::InvokeMetaMethod)
            return id;
        const QMetaProperty prop = target->metaObject()->property(signalIndex);
        m_propertyCallbackFunc(prop.name(), prop.read(target));
        return id;
    }

protected:
    PropertyCallbackFunc m_propertyCallbackFunc;
};

//////////////////////////////////////
DccQuickDBusInterface::Private::Private(DccQuickDBusInterface *q)
    : q_ptr(q)
    , m_propertySpy(new DBusPropertySignalSpy(std::bind(&DccQuickDBusInterface::Private::onPropertySpy, this, std::placeholders::_1, std::placeholders::_2), this))
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
    for (QVariantMap::const_iterator it = changedProperties.cbegin(); it != changedProperties.cend(); ++it) {
        QString propName;
        if (m_suffix.isEmpty()) {
            propName = it.key();
            propName[0] = propName.at(0).toLower();
        } else {
            propName = m_suffix + it.key();
        }
        if (m_mapProperties.contains(propName)) {
            if (m_mapProperties.value(propName).isEmpty()) {
                m_mapProperties[propName] = it.key();
            }
            QVariant v = DccQuickDBusCallback::toValue(it.value());
            m_propertyMap.insert(propName, v);
            // setProperty会触发属性变化信号
            q_ptr->setProperty(propName.toLatin1(), v);
        }
    }
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

QVariant DccQuickDBusInterface::Private::getProperty(const QString &propname)
{
    return m_propertyMap.value(propname, QVariant());
}

void DccQuickDBusInterface::Private::setProperty(const QString &propname, const QVariant &value)
{
    const QString prop = m_mapProperties.value(propname, propname);
    QDBusMessage msg = QDBusMessage::createMethodCall(m_service, m_path, PropertiesInterface, QStringLiteral("Set"));
    msg << m_interface << prop << QVariant::fromValue(QDBusVariant(value));
    m_connection.asyncCall(msg);
}

void DccQuickDBusInterface::Private::onPropertySpy(const char *propname, const QVariant &value)
{
    QVariant v = value;
    if (v.canConvert<QJSValue>()) {
        v = value.value<QJSValue>().toVariant();
    }
    if (m_propertyMap.value(propname) != v) {
        setProperty(propname, v);
    }
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

QString DccQuickDBusInterface::suffix() const
{
    return p_ptr->m_suffix;
}

void DccQuickDBusInterface::setSuffix(const QString &suffix)
{
    if (p_ptr->m_suffix != suffix) {
        p_ptr->m_suffix = suffix;
        Q_EMIT suffixChanged(p_ptr->m_suffix);
    }
}

bool DccQuickDBusInterface::callWithCallback(const QString &method, const QList<QVariant> &args, const QJSValue member, const QJSValue errorSlot)
{
    DccQuickDBusCallback *callback = new DccQuickDBusCallback(member, errorSlot, this);
    QDBusMessage msg = QDBusMessage::createMethodCall(p_ptr->m_service, p_ptr->m_path, p_ptr->m_interface, method);
    msg.setArguments(args);
    return p_ptr->m_connection.callWithCallback(msg, callback, SLOT(returnMethod(QDBusMessage)), SLOT(errorMethod(QDBusError, QDBusMessage)));
}

void DccQuickDBusInterface::connectNotify(const QMetaMethod &signal)
{
    QObject::connectNotify(signal);
}

void DccQuickDBusInterface::disconnectNotify(const QMetaMethod &signal)
{
    QObject::disconnectNotify(signal);
}

void DccQuickDBusInterface::classBegin() { }

void DccQuickDBusInterface::componentComplete()
{
    static const QStringList ReservedPropertyNames{ "service", "path", "inter", "connection", "suffix" };
    const QMetaObject *mo = this->metaObject();
    const int count = mo->propertyCount();
    for (int i = mo->propertyOffset(); i < count; ++i) {
        const QMetaProperty &property = mo->property(i);
        if (!ReservedPropertyNames.contains(property.name())) {
            p_ptr->m_mapProperties.insert(property.name(), QString());
            if (property.hasNotifySignal()) {
                QMetaObject::connect(this, property.notifySignalIndex(), p_ptr->m_propertySpy, i);
            }
        }
    }
    const int mcount = mo->methodCount();
    for (int i = mo->methodOffset(); i < mcount; ++i) {
        const QMetaMethod &m = mo->method(i);
        if (m.methodType() == 2 && m.name().startsWith("on")) {
            DccDBusSignalCallback *callback = new DccDBusSignalCallback(m, this);
            p_ptr->m_connection.connect(p_ptr->m_service, p_ptr->m_path, p_ptr->m_interface, m.name().mid(2), callback, SLOT(returnMethod(QDBusMessage)));
        }
    }
    if (!p_ptr->m_mapProperties.isEmpty()) {
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
}

} // namespace dccV25
