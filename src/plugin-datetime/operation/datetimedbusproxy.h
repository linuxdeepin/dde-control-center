// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DATETIMEDBUSPROXY_H
#define DATETIMEDBUSPROXY_H

#include "zoneinfo.h"
#include <QDBusArgument>
#include <QObject>

#include <optional>
class QDBusInterface;
class QDBusMessage;
class QDateTime;

class LocaleInfo
{
public:
    LocaleInfo() { }

    friend QDBusArgument &operator<<(QDBusArgument &arg, const LocaleInfo &info)
    {
        arg.beginStructure();
        arg << info.id << info.name;
        arg.endStructure();

        return arg;
    }

    friend const QDBusArgument &operator>>(const QDBusArgument &arg, LocaleInfo &info)
    {
        arg.beginStructure();
        arg >> info.id >> info.name;
        arg.endStructure();

        return arg;
    }

    friend QDataStream &operator<<(QDataStream &ds, const LocaleInfo &info)
    {
        return ds << info.id << info.name;
    }

    friend const QDataStream &operator>>(QDataStream &ds, LocaleInfo &info)
    {
        return ds >> info.id >> info.name;
    }

    bool operator==(const LocaleInfo &info) { return id == info.id && name == info.name; }

public:
    QString id{ "" };
    QString name{ "" };
};

typedef QList<LocaleInfo> LocaleList;
Q_DECLARE_METATYPE(LocaleInfo)
Q_DECLARE_METATYPE(LocaleList)

class DatetimeDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit DatetimeDBusProxy(QObject *parent = nullptr);
    static QString currentLocale();
    // Timedate
    Q_PROPERTY(bool Use24HourFormat READ use24HourFormat WRITE setUse24HourFormat NOTIFY Use24HourFormatChanged)
    bool use24HourFormat();
    void setUse24HourFormat(bool value);
    Q_PROPERTY(int WeekdayFormat READ weekdayFormat WRITE setWeekdayFormat NOTIFY WeekdayFormatChanged)
    int weekdayFormat();
    void setWeekdayFormat(int value);
    Q_PROPERTY(int LongDateFormat READ longDateFormat WRITE setLongDateFormat NOTIFY LongDateFormatChanged)
    int longDateFormat();
    void setLongDateFormat(int value);
    Q_PROPERTY(int ShortDateFormat READ shortDateFormat WRITE setShortDateFormat NOTIFY ShortDateFormatChanged)
    int shortDateFormat();
    void setShortDateFormat(int value);
    Q_PROPERTY(int LongTimeFormat READ longTimeFormat WRITE setLongTimeFormat NOTIFY LongTimeFormatChanged)
    int longTimeFormat();
    void setLongTimeFormat(int value);
    Q_PROPERTY(int ShortTimeFormat READ shortTimeFormat WRITE setShortTimeFormat NOTIFY ShortTimeFormatChanged)
    int shortTimeFormat();
    void setShortTimeFormat(int value);
    Q_PROPERTY(int WeekBegins READ weekBegins WRITE setWeekBegins NOTIFY WeekBeginsChanged)
    int weekBegins();
    void setWeekBegins(int value);
    Q_PROPERTY(QString NTPServer READ nTPServer NOTIFY NTPServerChanged)
    QString nTPServer();
    Q_PROPERTY(QString Timezone READ timezone NOTIFY TimezoneChanged)
    QString timezone();
    Q_PROPERTY(bool NTP READ nTP NOTIFY NTPChanged)
    bool nTP();
    Q_PROPERTY(QStringList UserTimezones READ userTimezones NOTIFY UserTimezonesChanged)
    QStringList userTimezones();

    // Locale
    std::optional<LocaleList> getLocaleListMap();
    std::optional<QString> getLocaleRegion();

    void setLocaleRegion(const QString &locale);

Q_SIGNALS: // SIGNALS
    // Timedate
    // begin property changed signals
    void CanNTPChanged(bool value) const;
    void DSTOffsetChanged(int value) const;
    void LocalRTCChanged(bool value) const;
    void LongDateFormatChanged(int value) const;
    void LongTimeFormatChanged(int value) const;
    void NTPChanged(bool value) const;
    void NTPServerChanged(const QString &value) const;
    void ShortDateFormatChanged(int value) const;
    void ShortTimeFormatChanged(int value) const;
    void TimezoneChanged(const QString &value) const;
    void Use24HourFormatChanged(bool value) const;
    void UserTimezonesChanged(const QStringList &value) const;
    void WeekBeginsChanged(int value) const;
    void WeekdayFormatChanged(int value) const;

public Q_SLOTS:
    // Timedate
    void SetNTP(bool useNTP);
    void SetNTP(bool useNTP, QObject *receiver, const char *member, const char *errorSlot);
    void SetDate(int year, int month, int day, int hour, int min, int sec, int nsec);
    void SetDate(const QDateTime &datetime, QObject *receiver, const char *member);
    void DeleteUserTimezone(const QString &zone);
    void AddUserTimezone(const QString &zone);
    QStringList GetSampleNTPServers();
    bool GetSampleNTPServers(QObject *receiver, const char *member);
    ZoneInfo GetZoneInfo(const QString &zone);
    bool GetZoneInfo(const QString &zone, QObject *receiver, const char *member);
    // System Timedate
    void SetTimezone(const QString &timezone);
    void SetNTPServer(const QString &server, const QString &message);
    void SetNTPServer(const QString &server,
                      const QString &message,
                      QObject *receiver,
                      const char *member,
                      const char *errorSlot);

private Q_SLOTS:
    void onPropertiesChanged(const QDBusMessage &message);

private:
    QDBusInterface *m_localeInter;
    QDBusInterface *m_timedateInter;
    QDBusInterface *m_systemtimedatedInter;
};

#endif // DATETIMEDBUSPROXY_H
