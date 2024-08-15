// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "mimedbusproxy.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDBusPendingReply>
#include <QDBusReply>
#include <QDebug>
#include <QMetaObject>

const QString ApplicationManagerServer = QStringLiteral("org.desktopspec.ApplicationManager1");
const QString MimePath = QStringLiteral("/org/desktopspec/ApplicationManager1/MimeManager1");
const QString MimeInterface = QStringLiteral("org.desktopspec.MimeManager1");
const QString AMApplicationInterface =
        QStringLiteral("org.desktopspec.ApplicationManager1.Application");

const QString ObjectManagerInterface = QStringLiteral("org.desktopspec.DBus.ObjectManager");
const QString ApplicationManager1Path = QStringLiteral("/org/desktopspec/ApplicationManager1");

MimeDBusProxy::MimeDBusProxy(QObject *parent)
    : QObject(parent)
    , m_mimeInter(new QDBusInterface(ApplicationManagerServer,
                                     MimePath,
                                     MimeInterface,
                                     QDBusConnection::sessionBus(),
                                     this))
    , m_applicationManagerInter(new QDBusInterface(ApplicationManagerServer,
                                                   ApplicationManager1Path,
                                                   ObjectManagerInterface,
                                                   QDBusConnection::sessionBus(),
                                                   this))
{
    qRegisterMetaType<ObjectInterfaceMap>();
    qDBusRegisterMetaType<ObjectInterfaceMap>();
    qRegisterMetaType<ObjectMap>();
    qDBusRegisterMetaType<ObjectMap>();
    qRegisterMetaType<QStringMap>();
    qDBusRegisterMetaType<QStringMap>();
    qRegisterMetaType<PropMap>();
    qDBusRegisterMetaType<PropMap>();
}

QDBusPendingReply<ObjectMap> MimeDBusProxy::GetManagedObjects()
{
    return m_applicationManagerInter->asyncCall("GetManagedObjects");
}

QDBusPendingReply<void> MimeDBusProxy::SetDefaultApp(const QString &mimeType,
                                                     const QString &desktopId)
{
    QStringMap map;
    map.insert(mimeType, desktopId);
    return m_mimeInter->asyncCallWithArgumentList("setDefaultApplication",
                                                  { QVariant::fromValue(map) });
}

void MimeDBusProxy::DeleteApp([[maybe_unused]] const QStringList &mimeTypes,
                              [[maybe_unused]] const QString &desktopId)
{
    // QDBusPendingReply<QString>(m_mimeInter->asyncCall("DeleteApp", mimeTypes, desktopId));
}

void MimeDBusProxy::DeleteUserApp([[maybe_unused]] const QString &desktopId)
{
    // QDBusPendingReply<QString>(m_mimeInter->asyncCall("DeleteUserApp", desktopId));
}

void MimeDBusProxy::AddUserApp([[maybe_unused]] const QStringList &mimeTypes,
                               [[maybe_unused]] const QString &desktopId)
{
    // QDBusPendingReply<QString>(m_mimeInter->asyncCall("AddUserApp", mimeTypes, desktopId));
}

QString MimeDBusProxy::getAppId(const QDBusObjectPath &appPath)
{
    auto interface = QDBusInterface(ApplicationManagerServer,
                                    appPath.path(),
                                    AMApplicationInterface,
                                    QDBusConnection::sessionBus(),
                                    this);
    return interface.property("ID").toString();
}

QDBusPendingReply<QString, QDBusObjectPath> MimeDBusProxy::GetDefaultApp(const QString &mimeType)
{
    return m_mimeInter->asyncCallWithArgumentList("queryDefaultApplication", { mimeType });
}

QDBusPendingReply<ObjectMap> MimeDBusProxy::ListApps(const QString &mimeType)
{
    return m_mimeInter->asyncCallWithArgumentList("listApplications", { mimeType });
}
