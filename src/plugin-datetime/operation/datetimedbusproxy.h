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
#ifndef DATETIMEDBUSPROXY_H
#define DATETIMEDBUSPROXY_H

#include <QObject>
#include "zoneinfo.h"
class QDBusInterface;
class QDBusMessage;
class QDateTime;

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
    void SetTimezone(const QString &timezone, const QString &message);
    void SetNTPServer(const QString &server, const QString &message);
    void SetNTPServer(const QString &server, const QString &message, QObject *receiver, const char *member, const char *errorSlot);

private Q_SLOTS:
    void onPropertiesChanged(const QDBusMessage &message);

private:
    QDBusInterface *m_timedateInter;
    QDBusInterface *m_systemtimedatedInter;
};

#endif // DATETIMEDBUSPROXY_H
