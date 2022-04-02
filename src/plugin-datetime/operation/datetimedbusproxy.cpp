/*
* Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
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
#include "datetimedbusproxy.h"

#include <QMetaObject>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QDateTime>

const QString TimedateService = QStringLiteral("com.deepin.daemon.Timedate");
const QString TimedatePath = QStringLiteral("/com/deepin/daemon/Timedate");
const QString TimedateInterface = QStringLiteral("com.deepin.daemon.Timedate");

const QString SystemTimedatedService = QStringLiteral("com.deepin.daemon.Timedated");
const QString SystemTimedatedPath = QStringLiteral("/com/deepin/daemon/Timedated");
const QString SystemTimedatedInterface = QStringLiteral("com.deepin.daemon.Timedated");

const QString PropertiesInterface = QStringLiteral("org.freedesktop.DBus.Properties");
const QString PropertiesChanged = QStringLiteral("PropertiesChanged");

DatetimeDBusProxy::DatetimeDBusProxy(QObject *parent)
    : QObject(parent)
    , m_timedateInter(new QDBusInterface(TimedateService, TimedatePath, TimedateInterface, QDBusConnection::sessionBus(), this))
    , m_systemtimedatedInter(new QDBusInterface(SystemTimedatedService, SystemTimedatedPath, SystemTimedatedInterface, QDBusConnection::systemBus(), this))
{
    registerZoneInfoMetaType();
    QDBusConnection::sessionBus().connect(TimedateService, TimedatePath, PropertiesInterface, PropertiesChanged, this, SLOT(onPropertiesChanged(QDBusMessage)));
}

bool DatetimeDBusProxy::use24HourFormat()
{
    return qvariant_cast<bool>(m_timedateInter->property("Use24HourFormat"));
}
void DatetimeDBusProxy::setUse24HourFormat(bool value)
{
    m_timedateInter->setProperty("Use24HourFormat", QVariant::fromValue(value));
}

int DatetimeDBusProxy::weekdayFormat()
{
    return qvariant_cast<int>(m_timedateInter->property("WeekdayFormat"));
}
void DatetimeDBusProxy::setWeekdayFormat(int value)
{
    m_timedateInter->setProperty("WeekdayFormat", QVariant::fromValue(value));
}

int DatetimeDBusProxy::longDateFormat()
{
    return qvariant_cast<int>(m_timedateInter->property("LongDateFormat"));
}
void DatetimeDBusProxy::setLongDateFormat(int value)
{
    m_timedateInter->setProperty("LongDateFormat", QVariant::fromValue(value));
}

int DatetimeDBusProxy::shortDateFormat()
{
    return qvariant_cast<int>(m_timedateInter->property("ShortDateFormat"));
}
void DatetimeDBusProxy::setShortDateFormat(int value)
{
    m_timedateInter->setProperty("ShortDateFormat", QVariant::fromValue(value));
}

int DatetimeDBusProxy::longTimeFormat()
{
    return qvariant_cast<int>(m_timedateInter->property("LongTimeFormat"));
}
void DatetimeDBusProxy::setLongTimeFormat(int value)
{
    m_timedateInter->setProperty("LongTimeFormat", QVariant::fromValue(value));
}

int DatetimeDBusProxy::shortTimeFormat()
{
    return qvariant_cast<int>(m_timedateInter->property("ShortTimeFormat"));
}
void DatetimeDBusProxy::setShortTimeFormat(int value)
{
    m_timedateInter->setProperty("ShortTimeFormat", QVariant::fromValue(value));
}

int DatetimeDBusProxy::weekBegins()
{
    return qvariant_cast<int>(m_timedateInter->property("WeekBegins"));
}
void DatetimeDBusProxy::setWeekBegins(int value)
{
    m_timedateInter->setProperty("WeekBegins", QVariant::fromValue(value));
}

QString DatetimeDBusProxy::nTPServer()
{
    return qvariant_cast<QString>(m_timedateInter->property("NTPServer"));
}

QString DatetimeDBusProxy::timezone()
{
    return qvariant_cast<QString>(m_timedateInter->property("Timezone"));
}

bool DatetimeDBusProxy::nTP()
{
    return qvariant_cast<bool>(m_timedateInter->property("NTP"));
}

QStringList DatetimeDBusProxy::userTimezones()
{
    return qvariant_cast<QStringList>(m_timedateInter->property("UserTimezones"));
}

void DatetimeDBusProxy::onPropertiesChanged(const QDBusMessage &message)
{
    QVariantMap changedProps = qdbus_cast<QVariantMap>(message.arguments().at(1).value<QDBusArgument>());
    for (QVariantMap::const_iterator it = changedProps.cbegin(); it != changedProps.cend(); ++it) {
        QMetaObject::invokeMethod(this, it.key().toLatin1() + "Changed", Qt::DirectConnection, QGenericArgument(it.value().typeName(), it.value().data()));
    }
}

// Timedate
void DatetimeDBusProxy::SetNTP(bool useNTP)
{
    m_timedateInter->asyncCall(QStringLiteral("SetNTP"), useNTP);
}
void DatetimeDBusProxy::SetNTP(bool useNTP, QObject *receiver, const char *member, const char *errorSlot)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(useNTP);
    m_timedateInter->callWithCallback(QStringLiteral("SetNTP"), argumentList, receiver, member, errorSlot);
}

void DatetimeDBusProxy::SetDate(int year, int month, int day, int hour, int min, int sec, int nsec)
{
    m_timedateInter->asyncCall(QStringLiteral("SetDate"), year, month, day, hour, min, sec, nsec);
}
void DatetimeDBusProxy::SetDate(const QDateTime &datetime, QObject *receiver, const char *member)
{
    const QDate date = datetime.date();
    const QTime time = datetime.time();
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(date.year()) << QVariant::fromValue(date.month()) << QVariant::fromValue(date.day());
    argumentList << QVariant::fromValue(time.hour()) << QVariant::fromValue(time.minute()) << QVariant::fromValue(time.second()) << 0;
    m_timedateInter->callWithCallback(QStringLiteral("SetDate"), argumentList, receiver, member);
}
void DatetimeDBusProxy::DeleteUserTimezone(const QString &zone)
{
    m_timedateInter->asyncCall(QStringLiteral("DeleteUserTimezone"), zone);
}

void DatetimeDBusProxy::AddUserTimezone(const QString &zone)
{
    m_timedateInter->asyncCall(QStringLiteral("AddUserTimezone"), zone);
}

QStringList DatetimeDBusProxy::GetSampleNTPServers()
{
    return QDBusPendingReply<QStringList>(m_timedateInter->asyncCall(QStringLiteral("GetSampleNTPServers")));
}

bool DatetimeDBusProxy::GetSampleNTPServers(QObject *receiver, const char *member)
{
    QList<QVariant> argumentList;
    return m_timedateInter->callWithCallback(QStringLiteral("GetSampleNTPServers"), argumentList, receiver, member);
}

ZoneInfo DatetimeDBusProxy::GetZoneInfo(const QString &zone)
{
    return QDBusPendingReply<ZoneInfo>(m_timedateInter->asyncCall(QStringLiteral("GetZoneInfo"), zone));
}
bool DatetimeDBusProxy::GetZoneInfo(const QString &zone, QObject *receiver, const char *member)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(zone);
    return m_timedateInter->callWithCallback(QStringLiteral("GetZoneInfo"), argumentList, receiver, member);
}
// System Timedate
void DatetimeDBusProxy::SetTimezone(const QString &timezone, const QString &message)
{
    m_systemtimedatedInter->asyncCall(QStringLiteral("SetTimezone"), timezone, message);
}

void DatetimeDBusProxy::SetNTPServer(const QString &server, const QString &message)
{
    m_systemtimedatedInter->asyncCall(QStringLiteral("SetNTPServer"), server, message);
}

void DatetimeDBusProxy::SetNTPServer(const QString &server, const QString &message, QObject *receiver, const char *member, const char *errorSlot)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(server) << QVariant::fromValue(message);
    m_systemtimedatedInter->callWithCallback(QStringLiteral("SetNTPServer"), argumentList, receiver, member, errorSlot);
}

QString DatetimeDBusProxy::currentLocale()
{
    QDBusInterface dbus("com.deepin.daemon.LangSelector", "/com/deepin/daemon/LangSelector", "com.deepin.daemon.LangSelector", QDBusConnection::sessionBus());
    return qvariant_cast<QString>(dbus.property("CurrentLocale"));
}
