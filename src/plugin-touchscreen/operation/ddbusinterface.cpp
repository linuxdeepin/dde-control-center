// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ddbusinterface.h"
#include "ddbusinterface_p.h"

#include <QMetaObject>
#include <qmetaobject.h>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDBusPendingReply>
#include <QDebug>

// DCORE_BEGIN_NAMESPACE

static const QString &FreedesktopService = QStringLiteral("org.freedesktop.DBus");
static const QString &FreedesktopPath = QStringLiteral("/org/freedesktop/DBus");
static const QString &FreedesktopInterface = QStringLiteral("org.freedesktop.DBus");
static const QString &NameOwnerChanged = QStringLiteral("NameOwnerChanged");

static const QString &PropertiesInterface = QStringLiteral("org.freedesktop.DBus.Properties");
static const QString &PropertiesChanged = QStringLiteral("PropertiesChanged");
static const char *PropertyName = "propname";

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    #define PropType(metaProperty) metaProperty.metaType()
#else
    #define PropType(metaProperty) metaProperty.userType()
#endif

static QVariant demarshall(const QMetaProperty &metaProperty, const QVariant &value)
{
    // if the value is the same with parent one, return value
    if (value.userType() == metaProperty.userType())
        return value;

    // unwrap the value with parent one
    QVariant result = QVariant(PropType(metaProperty), nullptr);


    if (value.userType() == qMetaTypeId<QDBusArgument>()) {
        QDBusArgument dbusArg = value.value<QDBusArgument>();
        QDBusMetaType::demarshall(dbusArg, PropType(metaProperty), result.data());
    }

    return result;
}

DDBusInterfacePrivate::DDBusInterfacePrivate(DDBusInterface *interface, QObject *parent)
    : QObject(interface)
    , m_parent(parent)
    , m_serviceValid(false)
    , q_ptr(interface)
{
    QDBusMessage message = QDBusMessage::createMethodCall(FreedesktopService, FreedesktopPath, FreedesktopInterface, "NameHasOwner");
    message << interface->service();
    interface->connection().callWithCallback(message, this, SLOT(onDBusNameHasOwner(bool)));

    interface->connection().connect(interface->service(),
                                    interface->path(),
                                    PropertiesInterface,
                                    PropertiesChanged,
                                    {interface->interface()},
                                    QString(),
                                    this,
                                    SLOT(onPropertiesChanged(QString, QVariantMap, QStringList)));
}

void DDBusInterfacePrivate::updateProp(const char *propName, const QVariant &value)
{
    if (!m_parent)
        return;

    const QMetaObject *metaObj = m_parent->metaObject();
    const char *typeName(value.typeName());
    void *data = const_cast<void *>(value.data());
    int propertyIndex = metaObj->indexOfProperty(propName);
    QVariant result = value;

    // TODO: it now cannot convert right, Like QMap
    // if there is property, then try to convert with property
    if (propertyIndex != -1) {
        QMetaProperty metaProperty = metaObj->property(propertyIndex);
        result = demarshall(metaProperty, value);
        data = const_cast<void *>(result.data());
        typeName = result.typeName();
    } else if (value.canConvert<QDBusArgument>()) {
        auto dbusType = qvariant_cast<QDBusArgument>(value);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        auto dbusMetaType = QDBusMetaType::signatureToMetaType(dbusType.currentSignature().toUtf8());
        typeName = dbusMetaType.name();

        void *dbusData = dbusMetaType.create();
        QDBusMetaType::demarshall(dbusType, dbusMetaType, dbusData);
        data = dbusData;
        QObject dbusDataDeleter;
        QObject::connect(&dbusDataDeleter, &QObject::destroyed, m_parent, [dbusData, dbusMetaType]() {
            dbusMetaType.destroy(dbusData);
        }, Qt::QueuedConnection);
#else
        auto dbusMetaType = QDBusMetaType::signatureToType(dbusType.currentSignature().toUtf8());
        typeName = QMetaType::typeName(dbusMetaType);

        void *dbusData = QMetaType::create(dbusMetaType);
        QDBusMetaType::demarshall(dbusType, dbusMetaType, dbusData);
        data = dbusData;
        // release dbus data of `QMetaType::create`.
        QObject dbusDataDeleter;
        QObject::connect(&dbusDataDeleter, &QObject::destroyed, m_parent, [dbusData, dbusMetaType]() {
            QMetaType::destroy(dbusMetaType, dbusData);
        }, Qt::QueuedConnection);
#endif
    }
    QByteArray baSignal = QStringLiteral("%1Changed(%2)").arg(propName).arg(typeName).toLatin1();
    int i = metaObj->indexOfSignal(baSignal.data());
    if (i != -1) {
        auto method = metaObj->method(i);
        if (method.parameterCount() == 1) {
            method.invoke(m_parent, Qt::DirectConnection, QGenericArgument(method.parameterTypeName(0), data));
        } else {
            method.invoke(m_parent, Qt::DirectConnection);
        }
    } else {
        qDebug() << "It's not exist the property:[" << propName << "] for parent:" << m_parent << ", interface:" << q_ptr->interface() << ", and It's changed value is:" << value;
    }
}

void DDBusInterfacePrivate::initDBusConnection()
{
    if (!m_parent)
        return;

    Q_Q(DDBusInterface);
    QDBusConnection connection = q->connection();
    QStringList signalList;
    QDBusInterface inter(q->service(), q->path(), q->interface(), connection);
    const QMetaObject *meta = inter.metaObject();
    for (int i = meta->methodOffset(); i < meta->methodCount(); ++i) {
        const QMetaMethod &method = meta->method(i);
        if (method.methodType() == QMetaMethod::Signal) {
            signalList << method.methodSignature();
        }
    }
    const QMetaObject *parentMeta = m_parent->metaObject();
    for (const QString &signal : signalList) {
        int i = parentMeta->indexOfSignal(QMetaObject::normalizedSignature(signal.toLatin1()));
        if (i != -1) {
            const QMetaMethod &parentMethod = parentMeta->method(i);
            connection.connect(q->service(),
                               q->path(),
                               q->interface(),
                               parentMethod.name(),
                               m_parent,
                               QT_STRINGIFY(QSIGNAL_CODE) + parentMethod.methodSignature());
        }
    }
}

void DDBusInterfacePrivate::onPropertiesChanged(const QString &interfaceName,
                                                const QVariantMap &changedProperties,
                                                const QStringList &invalidatedProperties)
{
    Q_UNUSED(interfaceName)
    Q_UNUSED(invalidatedProperties)
    for (QVariantMap::const_iterator it = changedProperties.cbegin(); it != changedProperties.cend(); ++it)
        updateProp((it.key() + m_suffix).toLatin1(), it.value());
}

void DDBusInterfacePrivate::onAsyncPropertyFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QVariant> reply = *w;
    if (!reply.isError()) {
        updateProp(w->property(PropertyName).toString().toLatin1(), reply.value());
    }
    w->deleteLater();
}

void DDBusInterfacePrivate::setServiceValid(bool valid)
{
    if (m_serviceValid != valid) {
        Q_Q(DDBusInterface);
        m_serviceValid = valid;
        Q_EMIT q->serviceValidChanged(m_serviceValid);
    }
}

void DDBusInterfacePrivate::onDBusNameHasOwner(bool valid)
{
    Q_Q(DDBusInterface);
    setServiceValid(valid);
    if (valid)
        initDBusConnection();
    else
        q->connection().connect(FreedesktopService,
                                FreedesktopPath,
                                FreedesktopInterface,
                                NameOwnerChanged,
                                this,
                                SLOT(onDBusNameOwnerChanged(QString, QString, QString)));
}

void DDBusInterfacePrivate::onDBusNameOwnerChanged(const QString &name, const QString &oldOwner, const QString &newOwner)
{
    Q_Q(DDBusInterface);
    if (name == q->service() && oldOwner.isEmpty()) {
        initDBusConnection();
        q->connection().disconnect(FreedesktopService,
                                   FreedesktopPath,
                                   FreedesktopInterface,
                                   NameOwnerChanged,
                                   this,
                                   SLOT(onDBusNameOwnerChanged(QString, QString, QString)));
        setServiceValid(true);
    } else if (name == q->service() && newOwner.isEmpty())
        setServiceValid(false);
}

//////////////////////////////////////////////////////////
// class DDBusInterface
//////////////////////////////////////////////////////////

DDBusInterface::DDBusInterface(const QString &service, const QString &path, const QString &interface, const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(service, path, interface.toLatin1(), connection, parent)
    , d_ptr(new DDBusInterfacePrivate(this, parent))
{
}

DDBusInterface::~DDBusInterface() {}

bool DDBusInterface::serviceValid() const
{
    Q_D(const DDBusInterface);
    return d->m_serviceValid;
}

QString DDBusInterface::suffix() const
{
    Q_D(const DDBusInterface);
    return d->m_suffix;
}

void DDBusInterface::setSuffix(const QString &suffix)
{
    Q_D(DDBusInterface);
    d->m_suffix = suffix;
}

inline QString originalPropname(const char *propname, QString suffix)
{
    QString propStr(propname);
    return propStr.left(propStr.length() - suffix.length());
}

QVariant DDBusInterface::property(const char *propName)
{
    Q_D(DDBusInterface);

    QDBusMessage msg = QDBusMessage::createMethodCall(service(), path(), PropertiesInterface, QStringLiteral("Get"));
    msg << interface() << originalPropname(propName, d->m_suffix);
    QDBusPendingReply<QVariant> prop = connection().asyncCall(msg);
    if (prop.value().isValid()) {
        // if there is no parent, return value
        if (!parent()) {
            qWarning() << "you use it without parent, and if the value is not valid, you may get nothing";
            return prop.value();
        }
        auto metaObject = parent()->metaObject();
        QVariant propresult = prop.value();
        int i = metaObject->indexOfProperty(propName);
        if (i != -1) {
            QMetaProperty metaProperty = metaObject->property(i);
            // try to use property in parent to unwrap the value
            propresult = demarshall(metaProperty, propresult);
        }
        return propresult;
    }

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(prop, this);
    watcher->setProperty(PropertyName, propName);
    connect(watcher, &QDBusPendingCallWatcher::finished, d, &DDBusInterfacePrivate::onAsyncPropertyFinished);

    return QVariant();
}

void DDBusInterface::setProperty(const char *propName, const QVariant &value)
{
    Q_D(const DDBusInterface);
    QDBusMessage msg = QDBusMessage::createMethodCall(service(), path(), PropertiesInterface, QStringLiteral("Set"));
    msg << interface() << originalPropname(propName, d->m_suffix) << QVariant::fromValue(QDBusVariant(value));

    QDBusPendingReply<void> reply = connection().asyncCall(msg);
    reply.waitForFinished();
    if (!reply.isValid()) {
        qWarning() << reply.error().message();
    }
}
// DCORE_END_NAMESPACE
