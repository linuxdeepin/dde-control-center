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
#include "widgets/dccdbusinterface.h"
#include <QMetaObject>
#include <qmetaobject.h>
#include <QDBusConnection>
#include <QDBusMetaType>
#include <QDBusPendingReply>
#include <QDebug>

const QString PropertiesInterface = QStringLiteral("org.freedesktop.DBus.Properties");
const QString PropertiesChanged = QStringLiteral("PropertiesChanged");
const static char *PropertyName = "propname";

#define D_D(classname) classname##Private *d = property(#classname "Private").value<classname##Private *>();
#define BIND_PRIVATE_CLASS(classname, object) setProperty(#classname "Private", QVariant::fromValue(object));

DCC_USE_NAMESPACE
class DCCDBusInterfacePrivate
{
public:
    explicit DCCDBusInterfacePrivate(DCCDBusInterface *interface, QObject *parent)
        : m_p(interface)
        , m_parent(parent)
    {
    }

    QVariant demarshall(const QMetaProperty &metaProperty, const QVariant &value)
    {
        if (value.userType() == metaProperty.userType()) {
            return value;
        }
        QVariant result = QVariant(metaProperty.userType(), nullptr);
        if (value.userType() == qMetaTypeId<QDBusArgument>()) {
            QDBusArgument dbusArg = value.value<QDBusArgument>();
            QDBusMetaType::demarshall(dbusArg, metaProperty.userType(), result.data());
        }
        return result;
    }

    QVariant updateProp(const char *propname, const QVariant &value)
    {
        QVariant result;
        const QMetaObject *metaObj = m_parent->metaObject();
        int i = metaObj->indexOfProperty(propname);
        if (i != -1) {
            QMetaProperty metaProperty = metaObj->property(i);
            result = demarshall(metaProperty, value);
            m_propertyMap.insert(propname, result);
            QMetaObject::invokeMethod(m_parent, propname + QString("Changed").toLatin1(), Qt::DirectConnection, QGenericArgument(result.typeName(), result.data()));
        } else
            qInfo() << "invalid property changed:" << propname << value;

        return result;
    }

public:
    DCCDBusInterface *m_p;
    QObject *m_parent;
    QString m_suffix;
    QVariantMap m_propertyMap;
};

Q_DECLARE_METATYPE(DCCDBusInterfacePrivate *)
Q_DECLARE_METATYPE(const DCCDBusInterfacePrivate *)

DCCDBusInterface::DCCDBusInterface(const QString &service, const QString &path, const QString &interface, const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(service, path, interface.toLatin1(), connection, parent)
{
    BIND_PRIVATE_CLASS(DCCDBusInterface, new DCCDBusInterfacePrivate(this, parent));
    if (parent) {
        QStringList argumentMatch;
        argumentMatch << interface;
        this->connection().connect(service, path, PropertiesInterface, PropertiesChanged, argumentMatch, QString(), this, SLOT(onPropertiesChanged(QString, QVariantMap, QStringList)));
    }
}

DCCDBusInterface::~DCCDBusInterface()
{
    D_D(DCCDBusInterface);
    delete d;
}

QString DCCDBusInterface::suffix() const
{
    D_D(const DCCDBusInterface);
    return d->m_suffix;
}

void DCCDBusInterface::setSuffix(const QString &suffix)
{
    D_D(DCCDBusInterface);
    d->m_suffix = suffix;
}

inline QString originalPropname(const char *propname, QString suffix)
{
    QString propStr(propname);
    return propStr.left(propStr.length() - suffix.length());
}

QVariant DCCDBusInterface::internalPropGet(const char *propname)
{
    D_D(DCCDBusInterface);
    if (d->m_propertyMap.contains(propname)) {
        return d->m_propertyMap.value(propname);
    } else {
        QDBusMessage msg = QDBusMessage::createMethodCall(service(), path(), PropertiesInterface, QStringLiteral("Get"));
        msg << interface() << originalPropname(propname, d->m_suffix);
        QDBusPendingReply<QVariant> prop = connection().asyncCall(msg);
        if (prop.value().isValid()) {
            return d->updateProp(propname, prop.value());
        } else {
            QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(prop, this);
            watcher->setProperty(PropertyName, propname);
            connect(watcher, &QDBusPendingCallWatcher::finished, this, &DCCDBusInterface::onAsyncPropertyFinished);
            if (d->m_propertyMap.contains(propname)) {
                return d->m_propertyMap.value(propname);
            }
        }
    }
    return QVariant();
}

void DCCDBusInterface::internalPropSet(const char *propname, const QVariant &value)
{
    D_D(const DCCDBusInterface);
    QDBusMessage msg = QDBusMessage::createMethodCall(service(), path(), PropertiesInterface, QStringLiteral("Set"));
    msg << interface() << originalPropname(propname, d->m_suffix) << value;
    connection().asyncCall(msg);
}

void DCCDBusInterface::onAsyncPropertyFinished(QDBusPendingCallWatcher *w)
{
    D_D(DCCDBusInterface);
    QDBusPendingReply<QVariant> reply = *w;
    if (!reply.isError()) {
        d->updateProp(w->property(PropertyName).toString().toLatin1(), reply.value());
    }
    w->deleteLater();
}

void DCCDBusInterface::onPropertiesChanged(const QString &interfaceName, const QVariantMap &changedProperties, const QStringList &invalidatedProperties)
{
    Q_UNUSED(interfaceName)
    Q_UNUSED(invalidatedProperties)
    D_D(DCCDBusInterface);
    for (QVariantMap::const_iterator it = changedProperties.cbegin(); it != changedProperties.cend(); ++it) {
        d->updateProp((it.key() + d->m_suffix).toLatin1(), it.value());
    }
}
