//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "datetimedbusproxy.h"

#include <QMetaObject>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QDateTime>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(DdcDateTimeDbusProxy, "dcc-datetime-dbusproxy")

const QString TimedateService = QStringLiteral("org.deepin.dde.Timedate1");
const QString TimedatePath = QStringLiteral("/org/deepin/dde/Timedate1");
const QString TimedateInterface = QStringLiteral("org.deepin.dde.Timedate1");

const QString SystemTimedatedService = QStringLiteral("org.deepin.dde.Timedate1");
const QString SystemTimedatedPath = QStringLiteral("/org/deepin/dde/Timedate1");
const QString SystemTimedatedInterface = QStringLiteral("org.deepin.dde.Timedate1");

const QString PropertiesInterface = QStringLiteral("org.freedesktop.DBus.Properties");
const QString PropertiesChanged = QStringLiteral("PropertiesChanged");

const QString LangSelectorService = QStringLiteral("org.deepin.dde.LangSelector1");
const QString LangSelectorPath = QStringLiteral("/org/deepin/dde/LangSelector1");
const QString LangSelectorInterface = QStringLiteral("org.deepin.dde.LangSelector1");

const QString FormatService = QStringLiteral("org.deepin.dde.Format1");
const QString FormatPath = QStringLiteral("/org/deepin/dde/Format1");
const QString FormatInterface = QStringLiteral("org.deepin.dde.Format1");

DatetimeDBusProxy::DatetimeDBusProxy(QObject *parent)
    : QObject(parent)
    , m_localeInter(new QDBusInterface(LangSelectorService, LangSelectorPath, LangSelectorInterface, QDBusConnection::sessionBus(), this))
    , m_timedateInter(new QDBusInterface(TimedateService, TimedatePath, TimedateInterface, QDBusConnection::sessionBus(), this))
    , m_systemtimedatedInter(new QDBusInterface(SystemTimedatedService, SystemTimedatedPath, SystemTimedatedInterface, QDBusConnection::systemBus(), this))
    , m_formatInter(new QDBusInterface(FormatService, FormatPath, FormatInterface, QDBusConnection::sessionBus(), this))
{
    registerZoneInfoMetaType();

    qRegisterMetaType<LocaleInfo>("LocaleInfo");
    qDBusRegisterMetaType<LocaleInfo>();

    qRegisterMetaType<LocaleList>("LocaleList");
    qDBusRegisterMetaType<LocaleList>();
    QDBusConnection::sessionBus().connect(TimedateService, TimedatePath, PropertiesInterface, PropertiesChanged, this, SLOT(onPropertiesChanged(QDBusMessage)));
    QDBusConnection::sessionBus().connect(FormatService, FormatPath, PropertiesInterface, PropertiesChanged, this, SLOT(onPropertiesChanged(QDBusMessage)));
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
    QDBusInterface dbus(LangSelectorService, LangSelectorPath, LangSelectorInterface, QDBusConnection::sessionBus());
    return qvariant_cast<QString>(dbus.property("CurrentLocale"));
}

std::optional<LocaleList> DatetimeDBusProxy::getLocaleListMap()
{
    QDBusPendingReply<LocaleList> reply = m_localeInter->asyncCall(QStringLiteral("GetLocaleList"));
    reply.waitForFinished();
    if (reply.isError()) {
        qCDebug(DdcDateTimeDbusProxy) << "Can not get localeRegion: "<< reply.error();
        return std::nullopt;
    }
    return reply.value();
}

std::optional<QString> DatetimeDBusProxy::getLocaleRegion()
{
    QDBusPendingReply<QString> reply = m_localeInter->asyncCall(QStringLiteral("GetLocaleRegion"));
    reply.waitForFinished();
    if (reply.isError()) {
        qCDebug(DdcDateTimeDbusProxy) << "Can not get localeRegion: "<< reply.error();
        return std::nullopt;
    }
    if (reply.value().isEmpty()) {
        return std::nullopt;
    } else {
        return reply.value();
    }
}

void DatetimeDBusProxy::setLocaleRegion(const QString &locale)
{
    m_localeInter->asyncCall(QStringLiteral("SetLocaleRegion"), locale);
}

QString DatetimeDBusProxy::decimalSymbol() const
{
    return qvariant_cast<QString>(m_formatInter->property("DecimalSymbol"));
}

void DatetimeDBusProxy::setDecimalSymbol(const QString &newDecimalSymbol)
{
    m_formatInter->setProperty("DecimalSymbol", QVariant::fromValue(newDecimalSymbol));
}

QString DatetimeDBusProxy::digitGrouping() const
{
    return qvariant_cast<QString>(m_formatInter->property("DigitGrouping"));
}

void DatetimeDBusProxy::setDigitGrouping(const QString &newDigitGrouping)
{
    m_formatInter->setProperty("DigitGrouping", QVariant::fromValue(newDigitGrouping));
}

QString DatetimeDBusProxy::digitGroupingSymbol() const
{
    return qvariant_cast<QString>(m_formatInter->property("DigitGroupingSymbol"));
}

void DatetimeDBusProxy::setDigitGroupingSymbol(const QString &newDigitGroupingSymbol)
{
    m_formatInter->setProperty("DigitGroupingSymbol", QVariant::fromValue(newDigitGroupingSymbol));
}

QString DatetimeDBusProxy::currencySymbol() const
{
    return qvariant_cast<QString>(m_formatInter->property("CurrencySymbol"));
}

void DatetimeDBusProxy::setCurrencySymbol(const QString &newCurrencySymbol)
{
    m_formatInter->setProperty("CurrencySymbol", QVariant::fromValue(newCurrencySymbol));
}

QString DatetimeDBusProxy::negativeCurrencyFormat() const
{
    return qvariant_cast<QString>(m_formatInter->property("NegativeCurrencyFormat"));
}

void DatetimeDBusProxy::setNegativeCurrencyFormat(const QString &newNegativeCurrencyFormat)
{
    m_formatInter->setProperty("NegativeCurrencyFormat", QVariant::fromValue(newNegativeCurrencyFormat));
}

QString DatetimeDBusProxy::positiveCurrencyFormat() const
{
    return qvariant_cast<QString>(m_formatInter->property("PositiveCurrencyFormat"));
}

void DatetimeDBusProxy::setPositiveCurrencyFormat(const QString &newPositiveCurrencyFormat)
{
    m_formatInter->setProperty("PositiveCurrencyFormat", QVariant::fromValue(newPositiveCurrencyFormat));
}

void DatetimeDBusProxy::GenLocale(const QString &locale)
{
    m_localeInter->asyncCall(QStringLiteral("GenLocale"), locale);
}
