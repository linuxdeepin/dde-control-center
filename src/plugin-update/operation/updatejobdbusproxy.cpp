// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "updatejobdbusproxy.h"

#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDBusPendingReply>
#include <QDBusReply>
#include <QDebug>

// Updater Job
const static QString UpdaterService = QStringLiteral("org.deepin.dde.Lastore1");
const static QString UpdaterJobInterface = QStringLiteral("org.deepin.dde.Lastore1.Job");

// Atomic Upgrade
const static QString AtomicUpdaterService = QStringLiteral("org.deepin.AtomicUpgrade1");
const static QString AtomicUpdaterJobInterface = QStringLiteral("org.deepin.AtomicUpgrade1");

const static QString PropertiesInterface = QStringLiteral("org.freedesktop.DBus.Properties");
const static QString PropertiesChanged = QStringLiteral("PropertiesChanged");

UpdateJobDBusProxy::UpdateJobDBusProxy(const QString &jobPath, QObject *parent)
    : QObject(parent)
    , m_updateJobInter(new DDBusInterface(
              UpdaterService, jobPath, UpdaterJobInterface, QDBusConnection::systemBus(), this))
{
}

UpdateJobDBusProxy::~UpdateJobDBusProxy()
{
    m_updateJobInter->deleteLater();
    m_updateJobInter = nullptr;
}

bool UpdateJobDBusProxy::isValid() const
{
    return m_updateJobInter->isValid();
}

bool UpdateJobDBusProxy::cancelable()
{
    return qvariant_cast<bool>(m_updateJobInter->property("Cancelable"));
}

qlonglong UpdateJobDBusProxy::createTime()
{
    return qvariant_cast<qlonglong>(m_updateJobInter->property("CreateTime"));
}

QString UpdateJobDBusProxy::description()
{
    return qvariant_cast<QString>(m_updateJobInter->property("Description"));
}

int UpdateJobDBusProxy::elapsedTime()
{
    return qvariant_cast<int>(m_updateJobInter->property("ElapsedTime"));
}

QString UpdateJobDBusProxy::id()
{
    return qvariant_cast<QString>(m_updateJobInter->property("Id"));
}

QString UpdateJobDBusProxy::name()
{
    return qvariant_cast<QString>(m_updateJobInter->property("Name"));
}

QString UpdateJobDBusProxy::packageId()
{
    return qvariant_cast<QString>(m_updateJobInter->property("PackageId"));
}

QStringList UpdateJobDBusProxy::packages()
{
    return qvariant_cast<QStringList>(m_updateJobInter->property("Packages"));
}

double UpdateJobDBusProxy::progress()
{
    return qvariant_cast<double>(m_updateJobInter->property("Progress"));
}

qlonglong UpdateJobDBusProxy::speed()
{
    return qvariant_cast<qlonglong>(m_updateJobInter->property("Speed"));
}

QString UpdateJobDBusProxy::status()
{
    return qvariant_cast<QString>(m_updateJobInter->property("Status"));
}

QString UpdateJobDBusProxy::type()
{
    return qvariant_cast<QString>(m_updateJobInter->property("Type"));
}
