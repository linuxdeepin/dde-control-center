// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "lastoremanagerjobdbusproxy.h"

#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDBusPendingReply>
#include <QDBusReply>
#include <QDebug>
#include <QLoggingCategory>

// Updater Job
const static QString UpdaterService = QStringLiteral("org.deepin.dde.Lastore1");
const static QString UpdaterJobInterface = QStringLiteral("org.deepin.dde.Lastore1.Job");

// Atomic Upgrade
const static QString AtomicUpdaterService = QStringLiteral("org.deepin.AtomicUpgrade1");
const static QString AtomicUpdaterJobInterface = QStringLiteral("org.deepin.AtomicUpgrade1");

const static QString PropertiesInterface = QStringLiteral("org.freedesktop.DBus.Properties");
const static QString PropertiesChanged = QStringLiteral("PropertiesChanged");

LastoreManagerJobDBusProxy::LastoreManagerJobDBusProxy(const QString &jobPath, QObject *parent)
    : QObject(parent)
    , m_updateJobInter(new DDBusInterface(
              UpdaterService, jobPath, UpdaterJobInterface, QDBusConnection::systemBus(), this))
    , m_path(jobPath)
{
}

LastoreManagerJobDBusProxy::~LastoreManagerJobDBusProxy()
{
    m_updateJobInter->deleteLater();
    m_updateJobInter = nullptr;
}

bool LastoreManagerJobDBusProxy::isValid() const
{
    return m_updateJobInter->isValid();
}

bool LastoreManagerJobDBusProxy::cancelable()
{
    return qvariant_cast<bool>(m_updateJobInter->property("Cancelable"));
}

qlonglong LastoreManagerJobDBusProxy::createTime()
{
    return qvariant_cast<qlonglong>(m_updateJobInter->property("CreateTime"));
}

QString LastoreManagerJobDBusProxy::description()
{
    return qvariant_cast<QString>(m_updateJobInter->property("Description"));
}

int LastoreManagerJobDBusProxy::elapsedTime()
{
    return qvariant_cast<int>(m_updateJobInter->property("ElapsedTime"));
}

QString LastoreManagerJobDBusProxy::id()
{
    return qvariant_cast<QString>(m_updateJobInter->property("Id"));
}

QString LastoreManagerJobDBusProxy::name()
{
    return qvariant_cast<QString>(m_updateJobInter->property("Name"));
}

QString LastoreManagerJobDBusProxy::packageId()
{
    return qvariant_cast<QString>(m_updateJobInter->property("PackageId"));
}

QStringList LastoreManagerJobDBusProxy::packages()
{
    return qvariant_cast<QStringList>(m_updateJobInter->property("Packages"));
}

double LastoreManagerJobDBusProxy::progress()
{
    return qvariant_cast<double>(m_updateJobInter->property("Progress"));
}

qlonglong LastoreManagerJobDBusProxy::speed()
{
    return qvariant_cast<qlonglong>(m_updateJobInter->property("Speed"));
}

QString LastoreManagerJobDBusProxy::status()
{
    return qvariant_cast<QString>(m_updateJobInter->property("Status"));
}

QString LastoreManagerJobDBusProxy::type()
{
    return qvariant_cast<QString>(m_updateJobInter->property("Type"));
}
