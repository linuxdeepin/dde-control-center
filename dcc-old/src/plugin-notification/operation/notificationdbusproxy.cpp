//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "notificationdbusproxy.h"

#include <QMetaObject>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QDBusMetaType>
#include <QDebug>

const static QString NotificationService = "org.deepin.dde.Notification1";
const static QString NotificationPath = "/org/deepin/dde/Notification1";
const static QString NotificationInterface = "org.deepin.dde.Notification1";

const static QString PropertiesInterface = "org.freedesktop.DBus.Properties";
const static QString PropertiesChanged = "PropertiesChanged";

NotificationDBusProxy::NotificationDBusProxy(QObject *parent)
    : QObject(parent)
{
    init();
}

void NotificationDBusProxy::init()
{
    m_dBusNotificationPropertiesInter = new QDBusInterface(NotificationService, NotificationPath, PropertiesInterface, QDBusConnection::sessionBus(), this);
    m_dBusNotificationInter = new QDBusInterface(NotificationService, NotificationPath, NotificationInterface, QDBusConnection::sessionBus(), this);

    QDBusConnection dbusConnection = m_dBusNotificationInter->connection();
    dbusConnection.connect(NotificationService, NotificationPath, PropertiesInterface, PropertiesChanged, this, SLOT(onPropertiesChanged(QDBusMessage)));
    dbusConnection.connect(NotificationService, NotificationPath, NotificationInterface, "ActionInvoked", this, SIGNAL(ActionInvoked(uint, QString)));
    dbusConnection.connect(NotificationService, NotificationPath, NotificationInterface, "AppAddedSignal", this, SIGNAL(AppAddedSignal(QString)));
    dbusConnection.connect(NotificationService, NotificationPath, NotificationInterface, "AppInfoChanged", this, SIGNAL(AppInfoChanged(QString, uint, QDBusVariant)));
    dbusConnection.connect(NotificationService, NotificationPath, NotificationInterface, "AppRemovedSignal", this, SIGNAL(AppRemovedSignal(QString)));
    dbusConnection.connect(NotificationService, NotificationPath, NotificationInterface, "NotificationClosed", this, SIGNAL(NotificationClosed(uint, uint)));
    dbusConnection.connect(NotificationService, NotificationPath, NotificationInterface, "RecordAdded", this, SIGNAL(RecordAdded(QString)));
    dbusConnection.connect(NotificationService, NotificationPath, NotificationInterface, "SystemInfoChanged", this, SIGNAL(SystemInfoChanged(uint, QDBusVariant)));
    dbusConnection.connect(NotificationService, NotificationPath, NotificationInterface, "appAdded", this, SIGNAL(appAdded(QString)));
    dbusConnection.connect(NotificationService, NotificationPath, NotificationInterface, "appRemoved", this, SIGNAL(appRemoved(QString)));
    dbusConnection.connect(NotificationService, NotificationPath, NotificationInterface, "appSettingChanged", this, SIGNAL(appSettingChanged(QString)));
    dbusConnection.connect(NotificationService, NotificationPath, NotificationInterface, "systemSettingChanged", this, SIGNAL(systemSettingChanged(QString)));
}

QString NotificationDBusProxy::allSetting()
{
    return qvariant_cast<QString>(m_dBusNotificationInter->property("allSetting"));
}

void NotificationDBusProxy::setAllSetting(const QString &value)
{
    m_dBusNotificationInter->setProperty("allSetting", QVariant::fromValue(value));
}

QString NotificationDBusProxy::systemSetting()
{
    return qvariant_cast<QString>(m_dBusNotificationInter->property("systemSetting"));
}

void NotificationDBusProxy::setSystemSetting(const QString &value)
{
    m_dBusNotificationInter->setProperty("systemSetting", QVariant::fromValue(value));
}


QDBusPendingReply<> NotificationDBusProxy::ClearRecords()
{
    QList<QVariant> argumentList;
    return m_dBusNotificationInter->asyncCallWithArgumentList(QStringLiteral("ClearRecords"), argumentList);
}


QDBusPendingReply<> NotificationDBusProxy::CloseNotification(uint in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusNotificationInter->asyncCallWithArgumentList(QStringLiteral("CloseNotification"), argumentList);
}

QDBusPendingReply<QString> NotificationDBusProxy::GetAllRecords()
{
    QList<QVariant> argumentList;
    return m_dBusNotificationInter->asyncCallWithArgumentList(QStringLiteral("GetAllRecords"), argumentList);
}



QDBusPendingReply<QDBusVariant> NotificationDBusProxy::GetAppInfo(const QString &in0, uint in1)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1);
    return m_dBusNotificationInter->asyncCallWithArgumentList(QStringLiteral("GetAppInfo"), argumentList);
}



QDBusPendingReply<QStringList> NotificationDBusProxy::GetAppList()
{
    QList<QVariant> argumentList;
    return m_dBusNotificationInter->asyncCallWithArgumentList(QStringLiteral("GetAppList"), argumentList);
}



QDBusPendingReply<QStringList> NotificationDBusProxy::GetCapbilities()
{
    QList<QVariant> argumentList;
    return m_dBusNotificationInter->asyncCallWithArgumentList(QStringLiteral("GetCapbilities"), argumentList);
}



QDBusPendingReply<QString> NotificationDBusProxy::GetRecordById(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusNotificationInter->asyncCallWithArgumentList(QStringLiteral("GetRecordById"), argumentList);
}



QDBusPendingReply<QString> NotificationDBusProxy::GetRecordsFromId(int in0, const QString &in1)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1);
    return m_dBusNotificationInter->asyncCallWithArgumentList(QStringLiteral("GetRecordsFromId"), argumentList);
}



QDBusPendingReply<QString, QString, QString, QString> NotificationDBusProxy::GetServerInformation()
{
    QList<QVariant> argumentList;
    return m_dBusNotificationInter->asyncCallWithArgumentList(QStringLiteral("GetServerInformation"), argumentList);
}


QDBusReply<QString> NotificationDBusProxy::GetServerInformation(QString &out1, QString &out2, QString &out3)
{
    QList<QVariant> argumentList;
    QDBusMessage reply = m_dBusNotificationInter->callWithArgumentList(QDBus::Block, QStringLiteral("GetServerInformation"), argumentList);
    if (reply.type() == QDBusMessage::ReplyMessage && reply.arguments().count() == 4) {
        out1 = qdbus_cast<QString>(reply.arguments().at(1));
        out2 = qdbus_cast<QString>(reply.arguments().at(2));
        out3 = qdbus_cast<QString>(reply.arguments().at(3));
    }
    return reply;
}

QDBusPendingReply<QDBusVariant> NotificationDBusProxy::GetSystemInfo(uint in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusNotificationInter->asyncCallWithArgumentList(QStringLiteral("GetSystemInfo"), argumentList);
}

QDBusPendingReply<> NotificationDBusProxy::Hide()
{
    QList<QVariant> argumentList;
    return m_dBusNotificationInter->asyncCallWithArgumentList(QStringLiteral("Hide"), argumentList);
}

QDBusPendingReply<uint> NotificationDBusProxy::Notify(const QString &in0, uint in1, const QString &in2, const QString &in3, const QString &in4, const QStringList &in5, const QVariantMap &in6, int in7)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1) << QVariant::fromValue(in2) << QVariant::fromValue(in3) << QVariant::fromValue(in4) << QVariant::fromValue(in5) << QVariant::fromValue(in6) << QVariant::fromValue(in7);
    return m_dBusNotificationInter->asyncCallWithArgumentList(QStringLiteral("Notify"), argumentList);
}

QDBusPendingReply<> NotificationDBusProxy::RemoveRecord(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusNotificationInter->asyncCallWithArgumentList(QStringLiteral("RemoveRecord"), argumentList);
}

QDBusPendingReply<> NotificationDBusProxy::SetAppInfo(const QString &in0, uint in1, const QDBusVariant &in2)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1) << QVariant::fromValue(in2);
    return m_dBusNotificationInter->asyncCallWithArgumentList(QStringLiteral("SetAppInfo"), argumentList);
}

QDBusPendingReply<> NotificationDBusProxy::SetSystemInfo(uint in0, const QDBusVariant &in1)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1);
    return m_dBusNotificationInter->asyncCallWithArgumentList(QStringLiteral("SetSystemInfo"), argumentList);
}

QDBusPendingReply<> NotificationDBusProxy::Show()
{
    QList<QVariant> argumentList;
    return m_dBusNotificationInter->asyncCallWithArgumentList(QStringLiteral("Show"), argumentList);
}

QDBusPendingReply<> NotificationDBusProxy::Toggle()
{
    QList<QVariant> argumentList;
    return m_dBusNotificationInter->asyncCallWithArgumentList(QStringLiteral("Toggle"), argumentList);
}

QDBusPendingReply<QString> NotificationDBusProxy::getAppSetting(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusNotificationInter->asyncCallWithArgumentList(QStringLiteral("getAppSetting"), argumentList);
}

QDBusPendingReply<uint> NotificationDBusProxy::recordCount()
{
    QList<QVariant> argumentList;
    return m_dBusNotificationInter->asyncCallWithArgumentList(QStringLiteral("recordCount"), argumentList);
}



QDBusPendingReply<> NotificationDBusProxy::setAppSetting(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusNotificationInter->asyncCallWithArgumentList(QStringLiteral("setAppSetting"), argumentList);
}


void NotificationDBusProxy::onPropertiesChanged(const QDBusMessage &message)
{
    QVariantMap changedProps = qdbus_cast<QVariantMap>(message.arguments().at(1).value<QDBusArgument>());
    for (QVariantMap::const_iterator it = changedProps.begin(); it != changedProps.end(); ++it) {
        QMetaObject::invokeMethod(this, it.key().toLatin1() + "Changed", Qt::DirectConnection, QGenericArgument(it.value().typeName(), it.value().data()));
    }
}
