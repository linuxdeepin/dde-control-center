/*
 * Copyright (C) 2022 ~ 2022 Deepin Technology Co., Ltd.
 *
 * Author:     guoyao <guoyao@uniontech.com>
 *
 * Maintainer: guoyao <guoyao@uniontech.com>
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
#include "systeminfodbusproxy.h"

#include "widgets/dccdbusinterface.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusPendingReply>
#include <QMetaObject>

const QString HostnameService = QStringLiteral("org.freedesktop.hostname1");
const QString HostnamePath = QStringLiteral("/org/freedesktop/hostname1");
const QString HostnameInterface = QStringLiteral("org.freedesktop.hostname1");

const QString LicenseInfoService = QStringLiteral("com.deepin.license");
const QString LicenseInfoPath = QStringLiteral("/com/deepin/license/Info");
const QString LicenseInfoInterface = QStringLiteral("com.deepin.license.Info");

const QString LicenseActivatorService = QStringLiteral("com.deepin.license.activator");
const QString LicenseActivatorPath = QStringLiteral("/com/deepin/license/activator");
const QString LicenseActivatorInterface = QStringLiteral("com.deepin.license.activator");

const QString PropertiesInterface = QStringLiteral("org.freedesktop.DBus.Properties");
const QString PropertiesChanged = QStringLiteral("PropertiesChanged");

SystemInfoDBusProxy::SystemInfoDBusProxy(QObject *parent)
    : QObject(parent)
    , m_hostname1Inter(new DCC_NAMESPACE::DCCDBusInterface(
              HostnameService, HostnamePath, HostnameInterface, QDBusConnection::systemBus(), this))
    , m_licenseInfoInter(new DCC_NAMESPACE::DCCDBusInterface(LicenseInfoService,
                                                             LicenseInfoPath,
                                                             LicenseInfoInterface,
                                                             QDBusConnection::systemBus(),
                                                             this))
    , m_licenseActivatorInter(new DCC_NAMESPACE::DCCDBusInterface(LicenseActivatorService,
                                                                  LicenseActivatorPath,
                                                                  LicenseActivatorInterface,
                                                                  QDBusConnection::sessionBus(),
                                                                  this))
{
}

QString SystemInfoDBusProxy::staticHostname()
{
    return qvariant_cast<QString>(m_hostname1Inter->property("StaticHostname"));
}

void SystemInfoDBusProxy::setStaticHostname(const QString &value)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(value) << QVariant::fromValue(true);
    m_hostname1Inter->asyncCallWithArgumentList("SetStaticHostname", argumentList);
}

void SystemInfoDBusProxy::setStaticHostname(const QString &value,
                                            QObject *receiver,
                                            const char *member,
                                            const char *errorSlot)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(value) << QVariant::fromValue(true);
    m_hostname1Inter->callWithCallback("SetStaticHostname",
                                       argumentList,
                                       receiver,
                                       member,
                                       errorSlot);
}

int SystemInfoDBusProxy::authorizationState()
{
    return qvariant_cast<int>(m_licenseInfoInter->property("AuthorizationState"));
}

void SystemInfoDBusProxy::setAuthorizationState(const int value)
{
    m_licenseInfoInter->setProperty("AuthorizationState", QVariant::fromValue(value));
}

void SystemInfoDBusProxy::Show()
{
    m_licenseActivatorInter->asyncCall("Show");
}
