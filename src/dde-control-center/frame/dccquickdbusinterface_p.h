// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCCQUICKDBUSINTERFACE_P_H
#define DCCQUICKDBUSINTERFACE_P_H
#include "dccquickdbusinterface.h"

#include <QDBusConnection>
#include <QJSValue>
#include <QObject>

namespace dccV25 {
class DccQuickDBusCallback : public QObject
{
    Q_OBJECT
public:
    explicit DccQuickDBusCallback(QJSValue member, QJSValue errorSlot, bool autoDelete, QObject *parent);
    ~DccQuickDBusCallback() override;

    static QVariant toValue(const QVariant &value);

public Q_SLOTS:
    void returnMethod(const QDBusMessage &msg);
    void errorMethod(const QDBusError &error, const QDBusMessage &msg);

protected:
    QJSValue m_member;
    QJSValue m_errorSlot;
    bool m_autoDelete;
};

class DccQuickDBusInterface::Private : public QObject
{
    Q_OBJECT
public:
    explicit Private(DccQuickDBusInterface *q);
    ~Private() override;

    void getAllPropertys();

private Q_SLOTS:
    void onAllProperties(const QVariantMap &changedProperties);
    void onGetPropertiesErr(const QDBusError &e);
    void onPropertiesChanged(const QString &interfaceName, const QVariantMap &changedProperties, const QStringList &invalidatedProperties);

public:
    DccQuickDBusInterface *q_ptr;
    QString m_service;
    QString m_path;
    QString m_interface;
    BusType m_connectionType;
    QDBusConnection m_connection;
    QVariantMap m_propertyMap;

    friend class DccQuickDBusCallback;
};
} // namespace dccV25
#endif // DCCQUICKDBUSINTERFACE_P_H
