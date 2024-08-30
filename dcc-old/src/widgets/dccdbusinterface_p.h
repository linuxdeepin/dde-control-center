//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCCDBUSINTERFACE_P_H
#define DCCDBUSINTERFACE_P_H

#include "widgets/dccdbusinterface.h"

class QDBusPendingCallWatcher;

namespace DCC_NAMESPACE {

class DCCDBusInterfacePrivate : public QObject
{
    Q_OBJECT

public:
    explicit DCCDBusInterfacePrivate(DCCDBusInterface *interface);
    void init(QObject *parent);
    QVariant demarshall(const QMetaProperty &metaProperty, const QVariant &value);
    QVariant updateProp(const char *propname, const QVariant &value);
    void initDBusConnection();
    void setServiceValid(bool valid);

private Q_SLOTS:
    void onPropertiesChanged(const QString &interfaceName, const QVariantMap &changedProperties, const QStringList &invalidatedProperties);
    void onAsyncPropertyFinished(QDBusPendingCallWatcher *w);
    void onDBusNameHasOwner(bool valid);
    void onDBusNameOwnerChanged(const QString &name, const QString &oldOwner, const QString &newOWner);

public:
    QObject *m_parent;
    QString m_suffix;
    QVariantMap m_propertyMap;
    bool m_serviceValid;

    DCCDBusInterface *q_ptr;
    Q_DECLARE_PUBLIC(DCCDBusInterface)
};

}

#endif // DCCDBUSINTERFACE_P_H
