// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once
#include "ddbusinterface.h"

class QDBusPendingCallWatcher;

// DCORE_BEGIN_NAMESPACE
class DDBusInterfacePrivate : public QObject
{
    Q_OBJECT

public:
    explicit DDBusInterfacePrivate(DDBusInterface *interface, QObject *parent);
    void updateProp(const char *propName, const QVariant &value);
    void initDBusConnection();
    void setServiceValid(bool valid);

private Q_SLOTS:
    void onPropertiesChanged(const QString &interfaceName,
                             const QVariantMap &changedProperties,
                             const QStringList &invalidatedProperties);
    void onAsyncPropertyFinished(QDBusPendingCallWatcher *w);
    void onDBusNameHasOwner(bool valid);
    void onDBusNameOwnerChanged(const QString &name, const QString &oldOwner, const QString &newOwner);

public:
    QObject *m_parent;
    QString m_suffix;
    bool m_serviceValid;

    DDBusInterface *q_ptr;
    Q_DECLARE_PUBLIC(DDBusInterface)
};
// DCORE_END_NAMESPACE
